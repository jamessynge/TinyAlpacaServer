#!/usr/bin/env python3
"""Makes HTTP requests to Alpaca servers, returns HTTP responses.

Usage: set_brightness.py brightness [request_count [server_addr[:port]]]
"""

import argparse
import sys
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
  parser.add_argument('--all', '-a', help='Enable all LED channels.')
  parser.add_argument('--red', '-r', help='Enable the red LED channel.')
  parser.add_argument('--green', '-g', help='Enable the green LED channel.')
  parser.add_argument('--blue', '-b', help='Enable blue LED channel.')
  parser.add_argument('--white', '-w', help='Enable white LED channel.')
  parser.add_argument('brightness', type=int, help=('The brightness value.'))

  args = parser.parse_args()
  cli_kwargs = dict(vars(args))  # Need a copy of the dictionary, else will
                                     # modify args itself.
  del cli_kwargs['brightness']
  cover_calibrator: alpaca_http_client.HttpCoverCalibrator = (
      alpaca_http_client.HttpCoverCalibrator.find_sole_device(**cli_kwargs))

  led_switches: alpaca_http_client.HttpSwitch = (
      alpaca_http_client.HttpSwitch.find_first_device(
          servers=[cover_calibrator.client],
          device_number=cover_calibrator.device_number))

  if args.all or args.red or args.green or args.blue or args.white:
    # We need to set or clear switches first.
    led_switches.put_setswitch(0, args.all or args.red)
    led_switches.put_setswitch(1, args.all or args.green)
    led_switches.put_setswitch(2, args.all or args.blue)
    led_switches.put_setswitch(3, args.all or args.white)

  if args.brightness > 0:
    cover_calibrator.put_calibratoron(args.brightness)
  else:
    cover_calibrator.put_calibratoroff()


if __name__ == '__main__':
  main()
