#!/usr/bin/env python3
"""Makes /management/v1/configureddevices requests, prints responses.

Usage: get_configured_devices.py [--url_base=1.2.3.4:8080]
"""

import argparse

import alpaca_discovery
import alpaca_http_client


def main() -> None:
  parser = argparse.ArgumentParser(
      description='Get configured devices.',
      parents=[
          alpaca_discovery.make_discovery_parser(),
          alpaca_http_client.make_url_base_parser(),
          alpaca_http_client.make_device_number_parser(),
          alpaca_http_client.make_device_type_parser(),
      ])
  cli_args = parser.parse_args()
  cli_kwargs = vars(cli_args)
  servers = alpaca_http_client.find_servers(**cli_kwargs)

  print(f'Found {len(servers)} Alpaca server{"" if len(servers) == 1 else "s"}')

  for server in servers:
    print()
    print(f'URL base: {server.url_base}')
    for cd in server.configured_devices():
      print()
      print(f'  Device #: {cd.device_number}')
      print(f'      Type: {cd.device_type.name}')
      print(f'      Name: {cd.device_name}')
      print(f'  UniqueID: {cd.unique_id}', flush=True)

  print()


if __name__ == '__main__':
  main()
