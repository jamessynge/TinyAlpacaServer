#!/usr/bin/env python3
"""Make HTTP requests to an AM WeatherBox Alpaca server, print values."""

import argparse
import time

import alpaca_http_client


def main() -> None:
  parser = argparse.ArgumentParser(description='Get configured devices.')
  parser.add_argument(
      '--url_base',
      '--url',
      help=('Base of URL before /api/v1/. If not specified, Alpaca Discovery '
            'is used to find an Alpaca Server with an Observing Conditions '
            'device.'))

  args = parser.parse_args()

  print(args)

  servers = []
  if args.url_base:
    servers.append(alpaca_http_client.AlpacaHttpClient(args.url_base))

  device = alpaca_http_client.HttpObservingConditions.find_sole_device(
      servers=servers)

  while True:
    sky = device.get_skytemperature().json()['Value']
    temp = device.get_temperature().json()['Value']
    precipitation = ('RAINING'
                     if device.get_rainrate().json()['Value'] > 0 else 'dry')

    print(f'Sky: {sky:>+5.1}    Ambient: {temp:>+5.1}    {precipitation}')

    time.sleep(3)


if __name__ == '__main__':
  main()
