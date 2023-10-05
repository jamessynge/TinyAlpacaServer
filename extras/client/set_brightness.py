#!/usr/bin/env python3
"""Sets the brightness of an AstroMakers Cover Calibrator device.

Usage: set_brightness.py [options] brightness
Options:
  Colors: If any colors are specified, then only the specified channels will be
          left on after this runs. If no colors are specified, then the colors
          aren't changed, just the brightness.
    --all or -a: Turn on all 4 channels.
    --red or -r: Turn on the red channel.
    --green or -g: Turn on the green channel.
    --blue or -b: Turn on the blue channel.
    --white or -w: Turn on the white channel.
    
  --help provides more details, such as the discovery API options and device
  selection.
"""

import argparse

import alpaca_discovery
import alpaca_http_client


def main() -> None:
  parser = argparse.ArgumentParser(
      description='Set Cover Calibrator brightness.',
      parents=[
          alpaca_discovery.make_discovery_parser(),
          alpaca_http_client.make_url_base_parser(),
          alpaca_http_client.make_device_number_parser(),
          alpaca_http_client.make_device_limits_parser(),
      ],
  )

  parser.add_argument(
      '--all', '-a', action='store_true', help='Enable all LED channels.'
  )
  parser.add_argument(
      '--red', '-r', action='store_true', help='Enable the red LED.'
  )
  parser.add_argument(
      '--green', '-g', action='store_true', help='Enable the green LED.'
  )
  parser.add_argument(
      '--blue', '-b', action='store_true', help='Enable blue LED.'
  )
  parser.add_argument(
      '--white', '-w', action='store_true', help='Enable white LED.'
  )

  parser.add_argument('brightness', type=int, help='The LED brightness value.')

  args = parser.parse_args()
  # Need a copy of the dictionary, else will modify args itself.
  cli_kwargs = dict(vars(args))
  cli_kwargs.pop('brightness', None)
  cover_calibrator: alpaca_http_client.HttpCoverCalibrator = (
      alpaca_http_client.HttpCoverCalibrator.find_sole_device(**cli_kwargs)
  )

  led_switches: alpaca_http_client.HttpSwitch = (
      alpaca_http_client.HttpSwitch.find_first_device(
          servers=[cover_calibrator.client],
          device_number=cover_calibrator.device_number,
      )
  )

  if args.all or args.red or args.green or args.blue or args.white:
    # We need to set or clear switches first to enable or disabled the desired
    # LED channels. We start by turning on channels so that the calibrator
    # doesn't blink off and then on again if just one channel is on at a time.
    # This is strictly an esthetic choice.

    if args.all or args.red:
      led_switches.put_setswitch(0, True)
    if args.all or args.green:
      led_switches.put_setswitch(1, True)
    if args.all or args.blue:
      led_switches.put_setswitch(2, True)
    if args.all or args.white:
      led_switches.put_setswitch(3, True)

    if not (args.all or args.red):
      led_switches.put_setswitch(0, False)
    if not (args.all or args.green):
      led_switches.put_setswitch(1, False)
    if not (args.all or args.blue):
      led_switches.put_setswitch(2, False)
    if not (args.all or args.white):
      led_switches.put_setswitch(3, False)

  # Note that the ASCOM Alpaca API allows brightness zero, without actually
  # turning off the calibrator, but I'm not exposing that here.
  if args.brightness > 0:
    cover_calibrator.put_calibratoron(args.brightness)
  else:
    cover_calibrator.put_calibratoroff()


if __name__ == '__main__':
  main()
