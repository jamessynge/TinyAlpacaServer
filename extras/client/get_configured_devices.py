#!/usr/bin/env python3
"""Makes HTTP requests to Alpaca servers, returns HTTP responses."""

import datetime
import random
import sys
from typing import List, Sequence

import alpaca_http_client

MOVING_SLEEP_TIME = 1


def main(argv: Sequence[str]) -> None:
  if len(argv) != 1:
    raise ValueError('Expects one arg, the base of the URL')

  url_base = argv[0]

  client = alpaca_http_client.AlpacaClient(
      url_base,
      client_id=random.randint(0, 10),
      initial_client_transaction_id=random.randint(10, 20))

  try:
    while True:
      print('get_configureddevices', client.get_configureddevices())
  except KeyboardInterrupt:
    pass
  client.session.close()


if __name__ == '__main__':
  main(sys.argv[1:])
