#!/usr/bin/env python3
"""Makes /management/apiversions requests, prints responses.

Usage: get_api_versions.py [request_count [server_addr[:port]]]
"""

import argparse

import alpaca_discovery
import alpaca_http_client


def main() -> None:
  parser = argparse.ArgumentParser(
      description='Get supported Alpaca API versions.',
      parents=[
          alpaca_discovery.make_discovery_parser(),
          alpaca_http_client.make_url_base_parser(),
          alpaca_http_client.make_device_number_parser(),
          alpaca_http_client.make_device_type_parser(),
      ],
  )
  cli_args = parser.parse_args()
  cli_kwargs = vars(cli_args)
  servers = alpaca_http_client.find_servers(**cli_kwargs)

  print(f'Found {len(servers)} Alpaca server{"" if len(servers) == 1 else "s"}')

  for server in servers:
    print()
    print(f'URL base: {server.url_base}')
    print(f'API versions: {server.apiversions()!r}')


if __name__ == '__main__':
  main()
