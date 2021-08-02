#!/usr/bin/env python3
"""Makes HTTP requests to Alpaca servers, returns HTTP responses."""

import datetime
import random
import sys
from typing import Iterable, List

import alpaca_http_client

MOVING_SLEEP_TIME = 1


def get_cover_state(
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator) -> int:
  resp = cover_calibrator.get_coverstate()
  return resp.json()['Value']


def is_present(
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator) -> bool:
  return get_cover_state(cover_calibrator) in (1, 2, 3, 4)


def is_closed(cover_calibrator: alpaca_http_client.HttpCoverCalibrator) -> bool:
  return get_cover_state(cover_calibrator) == 1


def is_moving(cover_calibrator: alpaca_http_client.HttpCoverCalibrator) -> bool:
  return get_cover_state(cover_calibrator) == 2


def is_open(cover_calibrator: alpaca_http_client.HttpCoverCalibrator) -> bool:
  return get_cover_state(cover_calibrator) == 3


def get_cover_state_after_moving(
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator) -> int:
  while True:
    state = get_cover_state(cover_calibrator)
    if state != 2:
      return state


def close_cover(
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator) -> None:
  """Close the cover of the Cover Calibrator device."""
  if not is_present(cover_calibrator):
    raise UserWarning('No cover')
  if is_closed(cover_calibrator):
    return
  resp = cover_calibrator.put_closecover()
  if resp.status_code != 200:
    resp.raise_for_status()
  print('Started to close')
  state = get_cover_state_after_moving(cover_calibrator)
  if state == 1:
    print('Successfully closed')
  else:
    print('Failed to close, cover state is', state)


def open_cover(
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator) -> None:
  """Open the cover of the Cover Calibrator device."""
  if not is_present(cover_calibrator):
    raise UserWarning('No cover')
  if is_open(cover_calibrator):
    return
  resp = cover_calibrator.put_opencover()
  if resp.status_code != 200:
    resp.raise_for_status()
  print('Started to open')
  state = get_cover_state_after_moving(cover_calibrator)
  if state == 3:
    print('Successfully opened')
  else:
    print('Failed to open, cover state is', state)


def sweep_brightness(cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
                     brightnesses: List[int]) -> None:
  """Set the brightness to each brightness, then reverse order, then turn off.

  Generally assumed that the brightnesses rise from low to high, so this has
  the effect of gradually increasing the brightness, then lowering it back
  down before turning it off.

  Args:
    cover_calibrator: The Cover Calibrator device.
    brightnesses: The list of brightness values.
  """
  start = datetime.datetime.now()
  for brightness in brightnesses:
    print('brightness', brightness)
    cover_calibrator.put_calibratoron(brightness)
  for brightness in reversed(brightnesses):
    print('brightness', brightness)
    cover_calibrator.put_calibratoron(brightness)
  cover_calibrator.put_calibratoroff()
  end = datetime.datetime.now()
  elapsed = end - start
  print('Elapsed time:', elapsed)
  print('Request duration', elapsed / (1 + 2 * len(brightnesses)))


def sweep_led_channel(led_switches: alpaca_http_client.HttpSwitch,
                      led_channel: int,
                      cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
                      brightnesses: Iterable[int]) -> None:
  for n in range(4):
    led_switches.put_setswitch(n, led_channel == n)
  sweep_brightness(cover_calibrator, list(brightnesses))


def find_first_cover_calibrator_server():
  for client in  alpaca_http_client.AlpacaHttpClient.find_servers(
      client_id=random.randint(0, 10),
      initial_client_transaction_id=random.randint(10, 20)):
    client.get_configureddevices()
  if not client:
    print('Found no servers!', file=sys.stderr)
    sys.exit(1)


def main(argv: List[str]) -> None:
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

  if not argv:
    calibrator_device_number = 1
  else:
    calibrator_device_number = int(argv.pop(0))

  if not argv:
    switch_device_number = calibrator_device_number
  else:
    switch_device_number = int(argv.pop(0))

  try:
    cover_calibrator = alpaca_http_client.HttpCoverCalibrator(
        client, calibrator_device_number)
    led_switches = alpaca_http_client.HttpSwitch(client, switch_device_number)

    while True:
      print('get_apiversions', client.get_apiversions())
      print('get_description', client.get_description())
      print('get_configureddevices', client.get_configureddevices())
      for led_channel in range(4):
        sweep_led_channel(led_switches, led_channel, cover_calibrator,
                          range(1, 10))
      open_cover(cover_calibrator)
      close_cover(cover_calibrator)
      open_cover(cover_calibrator)
  except KeyboardInterrupt:
    pass
  client.session.close()


if __name__ == '__main__':
  main(sys.argv[1:])
