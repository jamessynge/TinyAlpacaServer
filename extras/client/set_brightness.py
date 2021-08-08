#!/usr/bin/env python3
"""Makes HTTP requests to Alpaca servers, returns HTTP responses.

Usage: set_brightness.py brightness [request_count [server_addr[:port]]]
"""

import argparse
from typing import List

import alpaca_discovery
import alpaca_http_client


def main() -> None:
  parser = argparse.ArgumentParser(
      description='Set Cover Calibrator brightness.',
      parents=[
          alpaca_discovery.make_discovery_parser(),
          alpaca_http_client.make_url_base_parser(),
          alpaca_http_client.make_device_number_parser(),
          alpaca_http_client.make_device_limits_parser(),
      ])
  parser.add_argument('brightness', type=int, help=('The brightness value.'))

  cli_args = parser.parse_args()
  cli_kwargs = vars(cli_args)
  del cli_kwargs['brightness']
  devices: List[alpaca_http_client.HttpCoverCalibrator] = (
      alpaca_http_client.HttpCoverCalibrator.find_devices(**cli_kwargs))

  for device in devices:
    print(f'Setting brightness of server {device.client.url_base} cover '
          f'calibrator device {device.device_number}')
    response = device.put_calibratoron(cli_args.brightness)
    print(response.content)


if __name__ == '__main__':
  main()
