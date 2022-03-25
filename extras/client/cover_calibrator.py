#!/usr/bin/env python3
"""Demo program for an AstroMakers Cover Calibrator device.

Loops through the following operations:
*   Sweeps the brightness up and down for each of 4 channels
*   Opens, closes and opens the cover (i.e. ensures that it starts open each
    time, which after the first loop it is, so subsequent loops should close and
    open the cover).

Author: james.synge@gmail.com
"""

import argparse
import time
from typing import Iterable, List

import alpaca_discovery
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
    time.sleep(0.25)


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
  print('Closing cover...')
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
  print('Opening cover...')
  state = get_cover_state_after_moving(cover_calibrator)
  if state == 3:
    print('Successfully opened')
  else:
    print('Failed to open, cover state is', state)


def sweep_brightness(cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
                     brightness_list: List[int]) -> None:
  """Set the brightness to each brightness, then reverse order, then turn off.

  Generally assumed that the brightness_list rise from low to high, so this has
  the effect of gradually increasing the brightness, then lowering it back
  down before turning it off.

  Args:
    cover_calibrator: The Cover Calibrator device.
    brightness_list: The list of brightness values.
  """
  # start = datetime.datetime.now()
  for brightness in brightness_list:
    # print('brightness', brightness)
    cover_calibrator.put_calibratoron(brightness)
  for brightness in reversed(brightness_list):
    # print('brightness', brightness)
    cover_calibrator.put_calibratoron(brightness)
  cover_calibrator.put_calibratoroff()
  # end = datetime.datetime.now()
  # elapsed = end - start
  # print('Elapsed time:', elapsed)
  # print('Request duration', elapsed / (1 + 2 * len(brightness_list)))


def sweep_led_channel(led_switches: alpaca_http_client.HttpSwitch,
                      led_channel: int,
                      cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
                      brightness_list: Iterable[int]) -> None:
  for n in range(4):
    led_switches.put_setswitch(n, led_channel == n)
  sweep_brightness(cover_calibrator, list(brightness_list))


def main() -> None:
  parser = argparse.ArgumentParser(
      description='Set Cover Calibrator brightness.',
      parents=[
          alpaca_discovery.make_discovery_parser(),
          alpaca_http_client.make_url_base_parser(),
          alpaca_http_client.make_device_number_parser(),
      ])
  cli_args = parser.parse_args()
  cli_kwargs = vars(cli_args)

  cover_calibrator: alpaca_http_client.HttpCoverCalibrator = (
      alpaca_http_client.HttpCoverCalibrator.find_sole_device(**cli_kwargs))

  led_switches: alpaca_http_client.HttpSwitch = (
      alpaca_http_client.HttpSwitch.find_first_device(
          servers=[cover_calibrator.client],
          device_number=cover_calibrator.device_number))

  brightness_list = [min(1 << i, 65535) for i in range(17)]

  led_channels = list(range(4))
  # led_channels = []

  try:
    while True:
      for led_channel in led_channels:
        print(f'Raising and lowering brightness on channel {led_channel}')
        sweep_led_channel(led_switches, led_channel, cover_calibrator,
                          brightness_list)
      open_cover(cover_calibrator)
      close_cover(cover_calibrator)
      open_cover(cover_calibrator)
  except KeyboardInterrupt:
    pass


if __name__ == '__main__':
  main()
