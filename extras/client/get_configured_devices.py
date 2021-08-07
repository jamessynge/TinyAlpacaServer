#!/usr/bin/env python3
"""Makes /management/v1/configureddevices requests, prints responses.

Usage: get_configured_devices.py [request_count [server_addr[:port]]]
"""

import argparse

import alpaca_http_client


def main() -> None:
  parser = argparse.ArgumentParser(description='Get configured devices.')
  parser.add_argument(
      '--url_base',
      '--url',
      help=('Base of URL before /management/. If not specified, '
            'Alpaca Discovery is used to find Alpaca Servers.'))

  args = parser.parse_args()

  print(args)

  if args.url_base:
    servers = [alpaca_http_client.AlpacaHttpClient(args.url_base)]
  else:
    servers = alpaca_http_client.find_servers()

  print(f'Found {len(servers)} Alpaca servers')

  for server in servers:
    print()
    print(f'URL base: {server.url_base}')
    for cd in server.configured_devices():
      print(f'      Type: {cd.device_type}')
      print(f'         #: {cd.device_number}')
      print(f'      Name: {cd.device_name}')
      print(f'  UniqueID: {cd.device_name}')
      print(flush=True)


if __name__ == '__main__':
  main()
