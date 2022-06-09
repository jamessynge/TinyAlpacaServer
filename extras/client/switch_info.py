#!/usr/bin/env python3
"""Prints the info provided by an Alpaca Switch device.

Usage: switch_info.py [--url_base=1.2.3.4:8080]
"""

import argparse
from typing import List

import alpaca_discovery
import alpaca_http_client


def main() -> None:
  parser = argparse.ArgumentParser(
      description='Switch Info.',
      parents=[
          alpaca_discovery.make_discovery_parser(),
          alpaca_http_client.make_url_base_parser(),
          alpaca_http_client.make_device_number_parser(),
          alpaca_http_client.make_device_limits_parser(),
      ])

  args = parser.parse_args()
  switch_devices: List[alpaca_http_client.HttpSwitch] = (
      alpaca_http_client.HttpSwitch.find_devices(
        **vars(args))

  for switch_device in switch_devices:
    # Print common device info.
    # Print number of switches.
    num_switches = switch_device.get_maxswitch().json()['Value']
    # For each switch in the switch_device:
    for switch in range(switch_device

if __name__ == '__main__':
  main()
