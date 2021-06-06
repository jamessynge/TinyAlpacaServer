#!/usr/bin/env python3
"""Makes HTTP requests to Alpaca servers, returns HTTP responses."""

import enum
import random
import sys
from typing import Dict, Optional, Sequence, Union

import requests

import alpaca_http_client

def main(argv: Sequence[str]) -> None:
  if len(argv) != 2:
    raise ValueError('Expects one arg, the base of the URL')

  url_base = argv[0]
  device_number = int(argv[1])

  client = alpaca_http_client.AlpacaClient(url_base)

  print('get_apiversions', client.get_apiversions())
  print('get_description', client.get_description())
  print('get_configureddevices', client.get_configureddevices())

  cover_calibrator = alpaca_http_client.CoverCalibrator(client, device_number)

  print('Getting CoverCalibrator cover state')
  resp = cover_calibrator.get_coverstate()
  print(resp)
  print(resp.content)

if __name__ == '__main__':
  main(sys.argv[1:])
