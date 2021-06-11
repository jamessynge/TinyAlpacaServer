#!/usr/bin/env python3
"""Makes HTTP requests to Alpaca servers, returns HTTP responses."""

import enum
import random
import sys
import time
from typing import Dict, Optional, Sequence, Union

import requests

import alpaca_http_client

MOVING_SLEEP_TIME = 1


def get_cover_state(
    cover_calibrator: alpaca_http_client.CoverCalibrator) -> int:
  resp = cover_calibrator.get_coverstate()
  return resp.json()['Value']


def is_present(cover_calibrator: alpaca_http_client.CoverCalibrator) -> bool:
  return get_cover_state(cover_calibrator) in (1, 2, 3, 4)


def is_closed(cover_calibrator: alpaca_http_client.CoverCalibrator) -> bool:
  return get_cover_state(cover_calibrator) == 1


def is_moving(cover_calibrator: alpaca_http_client.CoverCalibrator) -> bool:
  return get_cover_state(cover_calibrator) == 2


def is_open(cover_calibrator: alpaca_http_client.CoverCalibrator) -> bool:
  return get_cover_state(cover_calibrator) == 3


def close_cover(cover_calibrator: alpaca_http_client.CoverCalibrator):
  if not is_present(cover_calibrator):
    raise UserWarning('No cover')
  if is_closed(cover_calibrator):
    return
  resp = cover_calibrator.put_closecover()
  if resp.status_code != 200:
    resp.raise_for_status()
  print('Started to close')
  while is_moving(cover_calibrator):
    print('Still moving')
    time.sleep(MOVING_SLEEP_TIME)
  state = get_cover_state(cover_calibrator)
  if state == 1:
    print('Successfully closed')
  else:
    print('Failed to close, cover state is', state)


def open_cover(cover_calibrator: alpaca_http_client.CoverCalibrator):
  if not is_present(cover_calibrator):
    raise UserWarning('No cover')
  if is_open(cover_calibrator):
    return
  resp = cover_calibrator.put_opencover()
  if resp.status_code != 200:
    resp.raise_for_status()
  print('Started to open')
  while is_moving(cover_calibrator):
    print('Still moving')
    time.sleep(MOVING_SLEEP_TIME)
  state = get_cover_state(cover_calibrator)
  if state == 3:
    print('Successfully opened')
  else:
    print('Failed to open, cover state is', state)


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

  open_cover(cover_calibrator)
  close_cover(cover_calibrator)
  open_cover(cover_calibrator)


if __name__ == '__main__':
  main(sys.argv[1:])
