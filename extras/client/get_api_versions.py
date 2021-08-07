#!/usr/bin/env python3
"""Makes /management/apiversions requests, prints responses.

Usage: get_api_versions.py [request_count [server_addr[:port]]]
"""

import argparse

import alpaca_http_client


def main() -> None:
  parser = argparse.ArgumentParser(
      description='Get supported Alpaca API versions.')
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
    print(f'API versions: {server.apiversions()!r}')


if __name__ == '__main__':
  main()
