#!/usr/bin/env python3
"""Make HTTP requests to an AM WeatherBox Alpaca server, print values."""

import random
import sys
import time
from typing import Sequence

import alpaca_http_client


def main(argv: Sequence[str]) -> None:
  if len(argv) != 2:
    raise ValueError(
        'Expects two args, the base of the URL and the device number')

  url_base = argv[0]
  device_number = int(argv[1])

  client = alpaca_http_client.AlpacaClient(
      url_base,
      client_id=random.randint(0, 10),
      initial_client_transaction_id=random.randint(10, 20))

  try:
    print('get_apiversions', client.get_apiversions().text)
    print('get_description', client.get_description().text)
    print('get_configureddevices', client.get_configureddevices().text)

    observing_conditions = alpaca_http_client.ObservingConditions(
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
