#!/usr/bin/env python3
"""Provides the ability to find IPv4 ASCOM Alpaca servers on the local networks.

Uses the netifaces library to find the broadcast IPs that can be used for
sending the UDP discovery message.

Note that I've chosen to omit support for IPv6 because I don't need it for
testing Tiny Alpaca Server.
"""

import json
import pprint
import queue
import socket
import threading
import time
from typing import Dict, Generator, List

import dataclasses
# You can install it with:
#      pip install netifaces
import netifaces

ALPACA_DISCOVERY_PORT = 32227  # a temporary port that I choose for testing
ALPACA_DISCOVERY = 'alpacadiscovery1'
ALPACA_RESPONSE = 'alpacaport'


@dataclasses.dataclass
class DiscoverySource:
  """Addresses from and to which to send ASCOM Alpaca discovery packets."""
  interface_name: str
  src_address: str
  dst_address: str
  dst_is_broadcast: bool

  def get_name(self) -> str:
    return f'{self.dst_address} via {self.interface_name}'

  def create_bound_udp_socket(self) -> socket.socket:
    """Create UDP port for sending to dst_address."""
    # --------------------------------
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    if self.dst_is_broadcast:
      sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    try:
      sock.bind((self.src_address, 0))  # listen to any on a temporary port
    except:
      print('failure to bind')
      sock.close()
      raise
    # sock.setblocking(0)
    return sock

  def send_discovery_packet(self, sock: socket.socket):
    action = 'Broadcasting' if self.dst_is_broadcast else 'Sending'
    print('%s to %s' % (action, self.dst_address))
    sock.sendto(ALPACA_DISCOVERY.encode(encoding='ascii'),
                (self.dst_address, ALPACA_DISCOVERY_PORT))


@dataclasses.dataclass
class DiscoveryResponse:
  source: DiscoverySource
  data_bytes: bytes
  addr: str

  def get_port(self) -> int:
    data_str = str(self.data_bytes, 'ascii')
    jsondata = json.loads(data_str)
    return int(jsondata[ALPACA_RESPONSE])


def generate_addresses(address_family) -> Generator[Dict[str, str], None, None]:
  """docstring."""
  # netifaces.interfaces returns a list of interface names.
  for name in netifaces.interfaces():
    # netifaces.ifaddresses(interface_name) returns a dictionary mapping an
    # address family (e.g. netifaces.AF_INET for IPv4) to details about that
    # address family on that interface.
    for address_family, addr_groups in netifaces.ifaddresses(name).items():
      if address_family == netifaces.AF_INET:
        for address_group in addr_groups:
          if 'addr' not in address_group:
            continue
          result = dict(interface_name=name)
          result.update(address_group)
          yield result


def generate_discovery_sources() -> Generator[DiscoverySource, None, None]:
  """docstring."""
  for address_group in generate_addresses(netifaces.AF_INET):
    if 'broadcast' in address_group:
      yield DiscoverySource(
          interface_name=address_group['interface_name'],
          src_address=address_group['addr'],
          dst_address=address_group['broadcast'],
          dst_is_broadcast=True)
    elif 'peer' in address_group:
      yield DiscoverySource(
          interface_name=address_group['interface_name'],
          src_address=address_group['addr'],
          dst_address=address_group['peer'],
          dst_is_broadcast=False)


def receiver(sock: socket.socket, max_wait_time: float,
             response_queue: queue.Queue) -> None:
  sock.settimeout(max_wait_time)
  while True:
    try:
      data_bytes, addr = sock.recvfrom(1024)
    except socket.timeout:
      return
    response_queue.put((data_bytes, addr))


class Discoverer(object):
  """Performs Alpaca Discovery for a single DiscoverySource."""

  def __init__(self, source: DiscoverySource):
    self.source = source

  def perform_discovery(self,
                        response_queue: queue.Queue,
                        max_wait_time: float = 5.0) -> threading.Thread:
    """Returns a thread which writes DiscoveryResponses to response_queue."""

    def worker():
      for r in self.generate_responses(max_wait_time=max_wait_time):
        response_queue.put(r)

    t = threading.Thread(target=worker, name=self.source.get_name())
    t.start()
    return t

  def generate_responses(
      self,
      max_wait_time: float = 5.0) -> Generator[DiscoveryResponse, None, None]:
    """Yields DiscoveryResponses after sending from the source address."""
    sock = self.source.create_bound_udp_socket()
    q = queue.Queue(maxsize=1000)
    kw = dict(sock=sock, max_wait_time=max_wait_time, response_queue=q)
    t = threading.Thread(target=receiver, kwargs=kw)
    t.start()
    iota = max(0.001, min(0.05, max_wait_time / 100.0))
    time.sleep(iota)
    self.source.send_discovery_packet(sock)
    count = 0
    while t.is_alive():
      try:
        data_bytes, addr = q.get(block=True, timeout=iota)
      except queue.Empty:
        continue
      yield DiscoveryResponse(
          source=self.source, data_bytes=data_bytes, addr=addr)
      count += 1
    t.join()
    while not q.empty():
      data_bytes, addr = q.get(block=False)
      yield DiscoveryResponse(
          source=self.source, data_bytes=data_bytes, addr=addr)
    print(f'Collected {count} responses for source {self.source.get_name()}')


def find_broadcast_addresses():
  """Returns the broadcast IPv4 addresses of the computer's network interfaces."""
  addresses = []
  # netifaces.interfaces returns a list of interface names.
  for interface_name in netifaces.interfaces():
    # netifaces.ifaddresses(interface_name) returns a dictionary mapping an
    # address family (e.g. netifaces.AF_INET for IPv4) to details about that
    # address family on that interface.
    for interfacedata in netifaces.ifaddresses(interface_name):
      if netifaces.AF_INET == interfacedata:
        ifaddrs = netifaces.ifaddresses(interface_name)
        for ifaddrs in ifaddrs[netifaces.AF_INET]:
          if 'broadcast' in ifaddrs:
            addresses.append(ifaddrs['broadcast'])
  if not addresses:
    raise ValueError('Found no broadcast addresses')
  return addresses


def perform_discovery(sources: List[DiscoverySource],
                      max_wait_time: float = 5.0):
  """INCOMPLETE test method."""
  discoverers = [Discoverer(source) for source in sources]
  q = queue.Queue(maxsize=1000)
  threads = [
      d.perform_discovery(response_queue=q, max_wait_time=max_wait_time)
      for d in discoverers
  ]
  while threads:
    alive_threads = []
    for t in threads:
      if t.is_alive():
        alive_threads.append(t)
      else:
        print('Thread %r is done' % t.name)
        t.join()
    threads = alive_threads
    while not q.empty():
      dr = q.get(block=False)
      pprint.pprint(dr)
      print('Found Alpaca Server at %s:%d' % (dr.addr[0], dr.get_port()))



def main():
  sources = list(generate_discovery_sources())
  perform_discovery(sources)


if __name__ == '__main__':
  main()
