#!/usr/bin/env python3
"""Make HTTP requests to an AM WeatherBox Alpaca server, print values."""

import random
import sys
import time
from typing import List

import alpaca_http_client


def has_observing_conditions(client: alpaca_http_client.AlpacaHttpClient) -> bool:
  try:
    resp = client.get_configureddevices()
    print(f'response:', resp)
    print(f'headers:', resp.headers)
    print(f'content:', resp.content)
    resp_jv = resp.json()
    print(f'resp_jv:', resp_jv)

    for info in resp_jv['Value']:
      print(f'info:', info)
      if info['DeviceType'] == 'ObservingConditions':
        return True
  except:
    pass
  return False


def find_first_observing_conditions_server():
  clients = alpaca_http_client.AlpacaHttpClient.find_servers(
      client_id=random.randint(0, 10),
      initial_client_transaction_id=random.randint(10, 20),
      server_filter=has_observing_conditions)
  if clients:
    return clients[0]
  print('Found no servers!', file=sys.stderr)
  sys.exit(1)


def main(argv: List[str]) -> None:
  if not argv:
    client = find_first_observing_conditions_server()
  else:
    client = alpaca_http_client.AlpacaHttpClient(
        argv.pop(0),
        client_id=random.randint(0, 10),
        initial_client_transaction_id=random.randint(10, 20))

  if not argv:
    device_number = 1
  else:
    device_number = int(argv.pop(0))

  if argv:
    raise ValueError(
        'Expects at most two args, the base of the URL and the device number')

  try:
    print('get_apiversions', client.get_apiversions().text)
    print('get_description', client.get_description().text)
    print('get_configureddevices', client.get_configureddevices().text)

    observing_conditions = alpaca_http_client.HttpObservingConditions(
        client, device_number)

    while True:
      print('skytemperature',
            observing_conditions.get_skytemperature().json()['Value'])
      print('temperature',
            observing_conditions.get_temperature().json()['Value'])
      print('rainrate', observing_conditions.get_rainrate().json()['Value'])
      time.sleep(3)
  finally:
    client.session.close()


if __name__ == '__main__':
  main(sys.argv[1:])
