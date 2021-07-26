#!/usr/bin/env python3
"""Make HTTP requests to an AM WeatherBox Alpaca server, print values."""

import random
import sys
import time
from typing import Sequence

import alpaca_http_client


def main(argv: Sequence[str]) -> None:
  if not argv:
    client = alpaca_http_client.AlpacaHttpClient.find_first_server(
        client_id=random.randint(0, 10),
        initial_client_transaction_id=random.randint(10, 20))
    if not client:
      print('Found no servers!', file=sys.stderr)
      sys.exit(1)
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
