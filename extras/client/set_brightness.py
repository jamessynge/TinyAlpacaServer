#!/usr/bin/env python3
"""Makes HTTP requests to Alpaca servers, returns HTTP responses.

Usage: set_brightness.py brightness [request_count [server_addr[:port]]]
"""

import argparse

import alpaca_http_client


def main() -> None:
  parser = argparse.ArgumentParser(
      description='Set Cover Calibrator brightness.')
  parser.add_argument(
      '--url_base',
      '--url',
      help=('Base of URL before /api/v1. If not specified, Alpaca Discovery is '
            'used to find an Alpaca Server with a Cover Calibrator device.'))
  parser.add_argument(
      '--device_number',
      type=int,
      help=('Device number to affect. Defaults to the sole '
            'Cover Calibrator device (there must not be multiple).'))
  parser.add_argument('brightness', type=int, help=('The brightness value.'))

  args = parser.parse_args()

  print(args)

  servers = ([alpaca_http_client.AlpacaHttpClient(args.url_base)]
             if args.url_base else [])

  device = alpaca_http_client.HttpCoverCalibrator.find_first_device(
      servers=servers, device_number=args.device_number)

  response = device.put_calibratoron(args.brightness)
  print(response.content)


if __name__ == '__main__':
  main()
