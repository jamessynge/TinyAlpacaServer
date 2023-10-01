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
import sys
import time
from typing import Iterable, List, Optional

import alpaca_discovery
import alpaca_http_client

MOVING_SLEEP_TIME = 1


def get_cover_state(
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
) -> int:
  resp = cover_calibrator.get_coverstate()
  return alpaca_http_client.get_ok_response_json_value_int(resp)


def is_present(
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
) -> bool:
  return get_cover_state(cover_calibrator) in (1, 2, 3, 4)


def is_closed(cover_calibrator: alpaca_http_client.HttpCoverCalibrator) -> bool:
  return get_cover_state(cover_calibrator) == 1


def is_moving(cover_calibrator: alpaca_http_client.HttpCoverCalibrator) -> bool:
  return get_cover_state(cover_calibrator) == 2


def is_open(cover_calibrator: alpaca_http_client.HttpCoverCalibrator) -> bool:
  return get_cover_state(cover_calibrator) == 3


def cover_state_to_name(
    cover_state: int, unavailable: Optional[int] = -1
) -> str:
  """Converts the cover state to a human readable string."""
  if cover_state == 1:
    return 'Closed'
  elif cover_state == 2:
    return 'Moving'
  elif cover_state == 3:
    return 'Open'
  elif cover_state == unavailable:
    return '(unavailable)'
  else:
    return f'Unknown ({cover_state})'


def cover_state_name(
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
) -> str:
  """Gets the cover state of the Cover Calibrator device."""
  cover_state = get_cover_state(cover_calibrator)
  return cover_state_to_name(cover_state)


def get_cover_state_after_moving(
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
) -> int:
  while True:
    state = get_cover_state(cover_calibrator)
    if state != 2:
      return state
    time.sleep(0.25)


def close_cover(
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
) -> None:
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
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
) -> None:
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


def max_switch_id(led_switches: alpaca_http_client.HttpSwitch) -> int:
  resp = led_switches.get_maxswitch()
  return alpaca_http_client.get_ok_response_json_value_int(resp)


def get_switch_ids(led_switches: alpaca_http_client.HttpSwitch) -> List[int]:
  return list(range(max_switch_id(led_switches)))


def get_writable_switches(
    led_switches: alpaca_http_client.HttpSwitch,
) -> List[int]:
  writable_switches = []
  for switch_id in get_switch_ids(led_switches):
    if led_switches.get_canwrite(switch_id):
      writable_switches.append(switch_id)
  return writable_switches


def get_switch(
    led_switches: alpaca_http_client.HttpSwitch, switch_id: int
) -> bool:
  resp = led_switches.get_getswitch(switch_id)
  return alpaca_http_client.get_ok_response_json_value_bool(resp)


def get_switches(led_switches: alpaca_http_client.HttpSwitch) -> List[bool]:
  return [get_switch(led_switches, switch_id) for switch_id in range(4)]


def bool_to_on_off(bool_value: bool) -> str:
  return 'on' if bool_value else 'off'


def get_switches_as_on_off(
    led_switches: alpaca_http_client.HttpSwitch,
) -> List[str]:
  return [bool_to_on_off(v) for v in get_switches(led_switches)]


def sweep_brightness(
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
    brightness_list: List[int],
) -> None:
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


def sweep_led_channel(
    led_switches: alpaca_http_client.HttpSwitch,
    led_channel: int,
    cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
    brightness_list: Iterable[int],
    all_led_channels: Optional[Iterable[int]] = None,
) -> None:
  """Sweep the brightness of a single led channel."""
  if all_led_channels is None:
    all_led_channels = range(4)
  for n in all_led_channels:
    if led_channel == n:
      print('Turning on led channel', n)
      led_switches.put_setswitch(n, True)
    else:
      print('Turning off led channel', n)
      led_switches.put_setswitch(n, False)
  sweep_brightness(cover_calibrator, list(brightness_list))


def main() -> None:
  parser = argparse.ArgumentParser(
      description="""Set Cover Calibrator brightness.

Specify at most one of --all-led-channels or --led-channels. If neither is
provided, then no sweeping of LEDs is performed.
      """,
      parents=[
          alpaca_discovery.make_discovery_parser(),
          alpaca_http_client.make_url_base_parser(),
          alpaca_http_client.make_device_number_parser(),
      ],
  )
  parser.add_argument(
      '--no-move',
      action='store_false',
      default=True,
      dest='move',
      help='Disable moving the cover.',
  )
  parser.add_argument(
      '--all-led-channels',
      action='store_true',
      default=False,
      dest='all_led_channels',
      help='Sweep all LED channels.',
  )
  parser.add_argument(
      '--led-channels',
      dest='led_channels',
      type=str,
      help=(
          'Comma separated list of LED channels to sweep, '
          'e.g. --led-channels 0  OR  --led-channels 1,3'
      ),
  )
  parser.add_argument(
      '--brightness-values',
      dest='brightness_values',
      type=int,
      help=(
          'Comma separated list of LED brightness values to sweep'
          ' through; if none are specified, default values are used. Values are'
          ' sorted from low to high, sweeps up from low to high, then back to'
          ' zero (off). For example: --'
      ),
  )
  cli_args = parser.parse_args()
  cli_kwargs = vars(cli_args)

  led_channels: list[int] = []
  if cli_args.all_led_channels and cli_args.led_channels:
    print(
        'Specify at most one of --all-led-channels and --led-channels',
        flush=True,
        file=sys.stderr,
    )
    sys.exit(1)
  elif cli_args.led_channels:
    led_channels = [int(x) for x in cli_args.led_channels.split(',')]

  if cli_args.brightness_values:
    brightness_list = sorted(list(cli_args.brightness_values))
  else:
    brightness_list = [0, 8000, 16000]
  decrease_list = list(reversed(brightness_list))[1:]
  brightness_list.extend(decrease_list)

  cover_calibrator: alpaca_http_client.HttpCoverCalibrator = (
      alpaca_http_client.HttpCoverCalibrator.find_sole_device(**cli_kwargs)
  )
  client = cover_calibrator.client
  print(
      f'Found Cover Calibrator #{cover_calibrator.device_number} @'
      f' {client.url_base}'
  )
  print('Cover state:', cover_state_name(cover_calibrator), flush=True)

  led_switches: alpaca_http_client.HttpSwitch = (
      alpaca_http_client.HttpSwitch.find_first_device(
          servers=[cover_calibrator.client],
          device_number=cover_calibrator.device_number,
      )
  )
  print(f'Found Switch #{led_switches.device_number}')
  print('Switch states:', get_switches_as_on_off(led_switches), flush=True)

  writable_switches = get_writable_switches(led_switches)

  if cli_args.all_led_channels:
    led_channels = writable_switches
  elif led_channels:
    for led_channel in led_channels:
      if led_channel not in writable_switches:
        print(
            f'Not a writable led channel: {led_channel}',
            flush=True,
            file=sys.stderr,
        )
        sys.exit(1)

  print('led_channels', led_channels)
  print('brightness_list', brightness_list)

  try:
    while True:
      for led_channel in led_channels:
        print(f'Raising and lowering brightness on channel {led_channel}')
        sweep_led_channel(
            led_switches,
            led_channel,
            cover_calibrator,
            brightness_list,
            all_led_channels=writable_switches,
        )

      if cli_args.move:
        open_cover(cover_calibrator)
        close_cover(cover_calibrator)
        open_cover(cover_calibrator)
  except KeyboardInterrupt:
    pass


if __name__ == '__main__':
  main()
