#!/usr/bin/env python3
"""Makes /management/apiversions requests, prints responses."""

import random
import sys
from typing import List

import alpaca_http_client


def main(argv: List[str]) -> None:
  if len(argv) not in [1, 2]:
    raise ValueError('Expects one arg, the base of the URL, '
                     'and optionally the number of requests')

  url_base = argv[0]
  request_count = int(argv[1]) if len(argv) > 1 else 100

  client = alpaca_http_client.AlpacaClient(
      url_base,
      client_id=random.randint(0, 10),
      initial_client_transaction_id=1)

  try:
    for _ in range(request_count):
      client.get_apiversions()
  except KeyboardInterrupt:
    pass
  client.session.close()


if __name__ == '__main__':
  main(sys.argv[1:])
