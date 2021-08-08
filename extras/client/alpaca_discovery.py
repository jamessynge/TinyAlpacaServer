#!/usr/bin/env python3
"""Provides the ability to find IPv4 ASCOM Alpaca servers on the local networks.

Uses the netifaces library to find the broadcast IPs that can be used for
sending the UDP discovery message.

Note that I've chosen to omit support for IPv6 because I don't need it for
testing Tiny Alpaca Server.

TODO(jamessynge): Figure out if I can NOT use netifaces to get the network
interface information.
"""

import argparse
import dataclasses
import json
import pprint
import queue
import socket
import sys
import threading
import time
from typing import Callable, Dict, Generator, List, Optional

import install_advice

try:
  import netifaces  # pylint: disable=g-import-not-at-top
except ImportError:
  install_advice.install_advice('netifaces')
# build_cleaner doesn't find imports that aren't at the top level, so we repeat
# the import here.
import netifaces  # pylint: disable=g-import-not-at-top,g-bad-import-order

# Daniel VanNoord selected UDP port 32227 for the Alpaca Discovery protocol, but
# that port is not officially assigned to the protocol, so it may change some
# day. An Alpaca Server can confirm that the packet is intended for it by
# looking for the string 'alpacadiscovery1' as the entire body of the UDP packet
# it receives at that port, and an Alpaca Discovery client can confirm that a
# response is from an Alpaca Server by checking that the response body can be
# parsed as JSON and has a property 'alpacaport' whose value is an integer that
# can be a TCP port number (e.g. 1 to 65535).
ALPACA_DISCOVERY_PORT = 32227
DISCOVERY_REQUEST_BODY = 'alpacadiscovery1'
ALPACA_SERVER_PORT_PROPERTY = 'alpacaport'


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
      print(f'failure to bind {self}', file=sys.stderr, flush=True)
      sock.close()
      raise
    # sock.setblocking(0)
    return sock

  def send_discovery_packet(self, sock: socket.socket, verbose=False):
    """Writes an Alpaca Discovery UDP Packet to sock."""
    if verbose:
      action = 'Broadcasting' if self.dst_is_broadcast else 'Sending'
      # Appending a \n explicitly because multiple threads will output strings,
      # and I've found that the default end value is output as a separate
      # operation that can come after the "Collected..." string from another
      # thread.
      print(
          f'{action} from {self.src_address} to {self.dst_address}\n',
          flush=True,
          end='')
    sock.sendto(
        DISCOVERY_REQUEST_BODY.encode(encoding='ascii'),
        (self.dst_address, ALPACA_DISCOVERY_PORT))


@dataclasses.dataclass
class DiscoveryResponse:
  """Represents an Alpaca Discovery Protocol Response from an Alpaca Server."""

  source: DiscoverySource
  data_bytes: bytes
  recvfrom_addr: str
  recvfrom_port: int  # The discovery port.

  def get_alpaca_server_addr(self) -> str:
    return f'{self.recvfrom_addr}:{self.get_port()}'

  def get_port(self) -> int:
    data_str = str(self.data_bytes, 'ascii')
    jsondata = json.loads(data_str)
    return int(jsondata[ALPACA_SERVER_PORT_PROPERTY])


def generate_addresses(address_family) -> Generator[Dict[str, str], None, None]:
  """docstring."""
  # netifaces.interfaces returns a list of interface names.
  for name in netifaces.interfaces():
    # netifaces.ifaddresses(interface_name) returns a dictionary mapping an
    # address family (e.g. netifaces.AF_INET for IPv4) to a list of address
    # groups (dictionaries) provided by that interface. Note that a single
    # interface may have multiple addresses, even for a single address family.
    for addr_family, addr_groups in netifaces.ifaddresses(name).items():
      if address_family == addr_family:
        for address_group in addr_groups:
          if 'addr' not in address_group:
            # Note that I'm assuming
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


def receiver(sock: socket.socket, max_discovery_secs: float,
             response_queue: queue.Queue) -> None:
  sock.settimeout(max_discovery_secs)
  while True:
    try:
      data_bytes, addr = sock.recvfrom(1024)
    except socket.timeout:
      return
    # For AF_INET sockets, addr is a pair, (host, port).
    response_queue.put((data_bytes, addr[0], addr[1]))


class Discoverer(object):
  """Performs Alpaca Discovery for a single DiscoverySource."""

  def __init__(self, source: DiscoverySource):
    self.source = source

  def perform_discovery(self,
                        response_queue: queue.Queue,
                        max_discovery_secs: float = 5.0,
                        verbose=False) -> threading.Thread:
    """Returns a thread which writes DiscoveryResponses to response_queue."""

    def worker():
      for r in self.generate_responses(
          max_discovery_secs=max_discovery_secs, verbose=verbose):
        response_queue.put(r)

    t = threading.Thread(target=worker, name=self.source.get_name())
    t.start()
    return t

  def generate_responses(
      self,
      max_discovery_secs: float = 5.0,
      verbose=False) -> Generator[DiscoveryResponse, None, None]:
    """Yields DiscoveryResponses after sending from the source address."""
    sock = self.source.create_bound_udp_socket()
    q = queue.Queue(maxsize=1000)
    t = threading.Thread(target=receiver, args=(sock, max_discovery_secs, q))
    t.start()
    iota = max(0.001, min(0.05, max_discovery_secs / 100.0))
    time.sleep(iota)
    self.source.send_discovery_packet(sock, verbose=verbose)
    count = 0
    while t.is_alive():
      try:
        data_bytes, addr, port = q.get(block=True, timeout=iota)
      except queue.Empty:
        continue
      yield DiscoveryResponse(
          source=self.source,
          data_bytes=data_bytes,
          recvfrom_addr=addr,
          recvfrom_port=port)
      count += 1
    t.join()
    while not q.empty():
      data_bytes, addr, port = q.get(block=False)
      yield DiscoveryResponse(
          source=self.source,
          data_bytes=data_bytes,
          recvfrom_addr=addr,
          recvfrom_port=port)
    if verbose:
      # Appending a \n explicitly because multiple threads will output strings,
      # and I've found that the default end value is output as a separate
      # operation that can come after the "Collected..." string from another
      # thread.
      print(
          f'Collected {count} responses for source {self.source.get_name()}\n',
          flush=True,
          end='')


def perform_discovery(discovery_response_handler: Callable[[DiscoveryResponse],
                                                           None],
                      sources: Optional[List[DiscoverySource]] = None,
                      max_discovery_secs: float = 5.0,
                      verbose=False) -> None:
  """Sends a discovery packet from all sources, passes results to handler."""
  if sources is None:
    sources = list(generate_discovery_sources())
  discoverers = [Discoverer(source) for source in sources]
  q = queue.Queue(maxsize=1000)
  threads = [
      d.perform_discovery(
          response_queue=q,
          max_discovery_secs=max_discovery_secs,
          verbose=verbose) for d in discoverers
  ]
  start_secs = time.time()
  while threads:
    if not threads[0].is_alive():
      t = threads.pop(0)
      if verbose:
        print('Thread %r is done' % t.name, flush=True)
      t.join()
    while not q.empty():
      dr = q.get(block=False)
      discovery_response_handler(dr)
    time.sleep(0.01)
  end_secs = time.time()
  if verbose:
    elapsed_secs = end_secs - start_secs
    print(f'perform_discovery: elapsed_secs={elapsed_secs}')
    if elapsed_secs < max_discovery_secs:
      print(
          f'perform_discovery: ended {max_discovery_secs - elapsed_secs}s early'
      )


def find_first_server(max_discovery_secs: float = 5.0,
                      verbose=False) -> Optional[DiscoveryResponse]:
  """Return the first server to respond within max_discovery_secs, else None."""
  result = None

  def discovery_response_handler(dr: DiscoveryResponse) -> None:
    nonlocal result
    if result is not None:
      return
    result = dr

  perform_discovery(
      discovery_response_handler,
      max_discovery_secs=max_discovery_secs,
      verbose=verbose)
  return result


def make_discovery_parser() -> argparse.ArgumentParser:
  """Returns a parser for discovery operations."""
  parser = argparse.ArgumentParser(add_help=False)
  parser.add_argument(
      '--max_discovery_secs',
      metavar='SECONDS',
      type=float,
      default=10.0,
      help='Time to wait (seconds) for Alpaca Discovery responses.')
  parser.add_argument(
      '--verbose',
      '-v',
      action='store_true',
      help='Print more messages about what the program is doing.')
  return parser


def main():
  parser = argparse.ArgumentParser(
      description='Find Alpaca servers.', parents=[make_discovery_parser()])
  cli_args = parser.parse_args()
  cli_kwargs = vars(cli_args)

  def discovery_response_handler(dr: DiscoveryResponse) -> None:
    pprint.pprint(dr)

  perform_discovery(discovery_response_handler, **cli_kwargs)


if __name__ == '__main__':
  main()
