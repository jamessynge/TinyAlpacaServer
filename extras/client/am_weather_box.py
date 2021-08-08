#!/usr/bin/env python3
"""Make HTTP requests to an AM WeatherBox Alpaca server, print values."""

import argparse
import time

import alpaca_discovery
import alpaca_http_client


def main() -> None:
  parser = argparse.ArgumentParser(
      description='Get configured devices.',
      parents=[
          alpaca_discovery.make_discovery_parser(),
          alpaca_http_client.make_url_base_parser(),
          alpaca_http_client.make_device_number_parser(),
      ])
  cli_args = parser.parse_args()
  cli_kwargs = vars(cli_args)
  device = alpaca_http_client.HttpObservingConditions.find_sole_device(
      **cli_kwargs)

  while True:
    sky = device.get_skytemperature().json()['Value']
    temp = device.get_temperature().json()['Value']
    precipitation = ('RAINING'
                     if device.get_rainrate().json()['Value'] > 0 else 'dry')

    print(f'Sky: {sky:>+5.1f}    Ambient: {temp:>+5.1f}    {precipitation}')

    time.sleep(3)


if __name__ == '__main__':
  main()
