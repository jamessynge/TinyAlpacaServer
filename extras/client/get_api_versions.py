#!/usr/bin/env python3
"""Makes /management/apiversions requests, prints responses.

Usage: get_api_versions.py [request_count [server_addr[:port]]]
"""

import random
import sys
from typing import List

import alpaca_http_client


def main(argv: List[str]) -> None:
  if not argv:
    request_count = 1
  else:
    request_count = int(argv.pop(0))

  if not argv:
    client = alpaca_http_client.AlpacaHttpClient.find_first_server(
        client_id=random.randint(0, 10),
        initial_client_transaction_id=random.randint(10, 20))
    if not client:
      print('Found no servers!', file=sys.stderr)
      sys.exit(1)
  else:
    client = alpaca_http_client.AlpacaHttpClient(
        argv.pop(0),
        client_id=random.randint(0, 10),
        initial_client_transaction_id=random.randint(10, 20))

  try:
    for _ in range(request_count):
      client.get_apiversions()
  except KeyboardInterrupt:
    pass
  client.session.close()


if __name__ == '__main__':
  main(sys.argv[1:])
