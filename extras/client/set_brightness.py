#!/usr/bin/env python3
"""Makes HTTP requests to Alpaca servers, returns HTTP responses.

Usage: get_api_versions.py [request_count [server_addr[:port]]]


"""

import random
import sys
from typing import Sequence

import alpaca_http_client


def main(argv: Sequence[str]) -> None:
  if len(argv) != 3:
    raise ValueError('Expects three args, the base of the URL, '
                     'the device number and the brightness')

  url_base = argv[0]
  device_number = int(argv[1])
  brightness = int(argv[2])

  client = alpaca_http_client.AlpacaHttpClient(
      url_base,
      client_id=random.randint(0, 10),
      initial_client_transaction_id=random.randint(10, 20))

  cover_calibrator = alpaca_http_client.HttpCoverCalibrator(
      client, device_number)
  response = cover_calibrator.put_calibratoron(brightness)
  print(response.content)
  client.session.close()


if __name__ == '__main__':
  main(sys.argv[1:])
