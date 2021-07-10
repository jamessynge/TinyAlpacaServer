#!/usr/bin/env python3
"""Makes HTTP requests to Alpaca servers, returns HTTP responses."""

import datetime
import random
import sys
from typing import List, Sequence

import alpaca_http_client

MOVING_SLEEP_TIME = 1


def sweep_brightness(cover_calibrator: alpaca_http_client.CoverCalibrator,
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


def sweep_led_channel(led_switches: alpaca_http_client.Switch, led_channel: int,
                      cover_calibrator: alpaca_http_client.CoverCalibrator,
                      brightnesses: List[int]) -> None:
  for n in range(4):
    led_switches.put_setswitch(n, led_channel == n)
  sweep_brightness(cover_calibrator, brightnesses)


def main(argv: Sequence[str]) -> None:
  if len(argv) != 2:
    raise ValueError(
        'Expects two args, the base of the URL and the device number')

  url_base = argv[0]
  device_number = int(argv[1])

  client = alpaca_http_client.AlpacaClient(
      url_base,
      client_id=random.randint(0, 10),
      initial_client_transaction_id=random.randint(10, 20))

  cover_calibrator = alpaca_http_client.CoverCalibrator(client, device_number)
  response = cover_calibrator.put_calibratoron(65536)
  print(response.content)
  client.session.close()


if __name__ == '__main__':
  main(sys.argv[1:])


# 169.254.239.130
