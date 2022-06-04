#!/usr/bin/env python3
"""Demo program showing auto-closing when weather is bad.

If weather is "bad" (sky temp is high or rain detected), then closes
the cover (simulating closing a roof) and repeatedly sweeps LED brightness
up and down. When weather is no longer bad, turns off the LEDs and opens
the cover.

Author: james.synge@gmail.com
"""

import argparse
import time
from typing import Any, Dict, Iterable, List

import alpaca_discovery
import alpaca_http_client

MOVING_SLEEP_TIME = 1
last_weather_msg = None



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


def enable_only_led_channel(led_switches: alpaca_http_client.HttpSwitch,
                      led_channel: int) -> None:
  for n in range(4):
    led_switches.put_setswitch(n, led_channel == n)


def sweep_led_channel(led_switches: alpaca_http_client.HttpSwitch,
                      led_channel: int,
                      cover_calibrator: alpaca_http_client.HttpCoverCalibrator,
                      brightness_list: Iterable[int]) -> None:
  for n in range(4):
    led_switches.put_setswitch(n, led_channel == n)
  sweep_brightness(cover_calibrator, list(brightness_list))


def get_weather_report(device: alpaca_http_client.HttpObservingConditions) -> Dict[str, Any]:
  """Returns True if the weather is OK."""
  sky = device.get_skytemperature().json()['Value']
  ambient = device.get_temperature().json()['Value']
  rainrate = device.get_rainrate().json()['Value']
  if rainrate > 0:
    condition = 'RAINING'
  elif sky > ambient:
    # Normally this would be something like (ambient - sky) < 20.
    condition = 'Cloudy'
  else:
    condition = 'dry'
  return dict(sky=sky, ambient=ambient, rainrate=rainrate, condition=condition)


def print_weather_if_changed(report: Dict[str, Any]) -> None:
  def round_to_half(v: float) -> float:
    return round(v * 2) / 2
  sky = round_to_half(report['sky'])
  ambient = round_to_half(report['ambient'])
  rainrate = report['rainrate']
  condition = report['condition']
  msg = (f'Sky: {sky:>+5.1f}    Ambient: {ambient:>+5.1f}    '
         f'Rainrate: {rainrate}    Condition: {condition}')
  global last_weather_msg
  if last_weather_msg == msg:
    return
  last_weather_msg = msg
  print(msg)


def main() -> None:
  parser = argparse.ArgumentParser(
      description='Set Cover Calibrator brightness.',
      parents=[
          alpaca_discovery.make_discovery_parser(),
          alpaca_http_client.make_url_base_parser(),
          alpaca_http_client.make_device_number_parser(),
      ])
  parser.add_argument(
      '--red', '-r', action='store_true', help='Enable the red LED.')
  parser.add_argument(
      '--green', '-g', action='store_true', help='Enable the green LED.')
  parser.add_argument(
      '--blue', '-b', action='store_true', help='Enable blue LED.')
  parser.add_argument(
      '--white', '-w', action='store_true', help='Enable white LED.')
  parser.add_argument('brightness', metavar='N', type=int, nargs='*',
                    help='Brightness values for sweep')
  cli_args = parser.parse_args()
  cli_kwargs = vars(cli_args)

  if cli_args.brightness:
    brightness_list = sorted(list(cli_args.brightness))
  else:
    brightness_list = [min(1 << i, 65535) for i in range(17)]
  print('brightness_list', brightness_list)
  decrease_list = list(reversed(brightness_list))[1:]
  brightness_list.extend(decrease_list)
  print('brightness_list', brightness_list)

  green_channel = 0
  red_channel = 1
  blue_channel = 2
  white_channel = 3

  if (cli_args.red or cli_args.green or
      cli_args.blue or cli_args.white):
    led_channels = (
      ([red_channel] if cli_args.red else []) +
      ([green_channel] if cli_args.green else []) +
      ([blue_channel] if cli_args.blue else []) +
      ([white_channel] if cli_args.white else [])
    )
  else:
    led_channels = [red_channel, green_channel, blue_channel, white_channel]
  led_channels.sort()
  print('led_channels', led_channels)

  cover_calibrator: alpaca_http_client.HttpCoverCalibrator = (
      alpaca_http_client.HttpCoverCalibrator.find_sole_device(**cli_kwargs))
  if not is_present(cover_calibrator):
    raise UserWarning('No cover')

  led_switches: alpaca_http_client.HttpSwitch = (
      alpaca_http_client.HttpSwitch.find_first_device(
          servers=[cover_calibrator.client],
          device_number=cover_calibrator.device_number))

  obs_conditions: alpaca_http_client.HttpObservingConditions = (
      alpaca_http_client.HttpObservingConditions.find_sole_device(**cli_kwargs))

  cover_calibrator.put_calibratoroff()

  try:
    weather_ok = False
    led_channel_index = 99999
    brightness_list_index = 99999
    while True:
      report = get_weather_report(obs_conditions)
      print_weather_if_changed(report)
      if report['condition'] == 'dry':
        if not weather_ok:
          cover_calibrator.put_calibratoroff()
          cover_calibrator.put_haltcover()
          cover_calibrator.put_opencover()
          weather_ok = True
          led_channel_index = 99999
          brightness_list_index = 99999
        continue

      # Weather is not OK.
      weather_ok = False
      if not is_closed(cover_calibrator):
        cover_calibrator.put_closecover()
        continue

      if brightness_list_index < len(brightness_list):
        # Set brightness.
        brightness = brightness_list[brightness_list_index]
        cover_calibrator.put_calibratoron(brightness)
        print('brightness', brightness)
        brightness_list_index += 1
        continue

      # Done with the previous channel.
      cover_calibrator.put_calibratoroff()

      # Move on to the next channel.
      led_channel_index += 1
      if led_channel_index >= len(led_channels):
        led_channel_index = 0
      led_channel = led_channels[led_channel_index]
      print('led_channel', led_channel)
      enable_only_led_channel(led_switches, led_channel)
      brightness_list_index = 0      
      continue 
  except KeyboardInterrupt:
    pass


if __name__ == '__main__':
  main()
