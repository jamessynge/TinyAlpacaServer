#!/usr/bin/env python3
"""Turns on and off the channels of an AstroMakers Cover Calibrator device.

The goal here is to test the independence of the different channels, with the
brightness value not being changed.

Usage: toggle_led_channels.py --brightness=value
"""

import argparse
import dataclasses

import alpaca_discovery
import alpaca_http_client


def get_canwrite(
    led_switches: alpaca_http_client.HttpSwitch, channel_number: int
) -> bool:
  resp = led_switches.get_canwrite(channel_number)
  return alpaca_http_client.get_ok_response_json_value_bool(resp)


def get_getswitch(
    led_switches: alpaca_http_client.HttpSwitch, channel_number: int
) -> bool:
  resp = led_switches.get_getswitch(channel_number)
  return alpaca_http_client.get_ok_response_json_value_bool(resp)


def get_getswitchname(
    led_switches: alpaca_http_client.HttpSwitch, channel_number: int
) -> str:
  resp = led_switches.get_getswitchname(channel_number)
  return alpaca_http_client.get_ok_response_json_value_str(resp)


@dataclasses.dataclass
class LedChannel:
  """Provides access to a single boolean switch."""

  led_switches: alpaca_http_client.HttpSwitch
  channel_number: int
  name: str = dataclasses.field(default_factory=str)
  _can_write: bool = dataclasses.field(init=False)
  _enabled: bool = dataclasses.field(init=False)

  def __post_init__(self):
    if not self.name:
      self.name = get_getswitchname(self.led_switches, self.channel_number)
    self._can_write = get_canwrite(self.led_switches, self.channel_number)
    self._enabled = get_getswitch(self.led_switches, self.channel_number)

  def sync_bools(self):
    """Syncs can_write and enabled.

    cover_calibrator.cc doesn't allow a channel to be enabled if the hardware
    jumper disables it, so we don't need to sync can_write if the channel is
    enabled because that implies the jumper enables the channel.
    Conversely, if can_write is false, we don't need to sync enabled.
    """
    if not self._can_write:
      self.sync_canwrite()
      if self._can_write:
        # The channel should not now be enabled, so we only sync enabled if for
        # some reason it is already considered enabled.
        if self._enabled:
          print(
              f'Channel {self.channel_number} ({self.name}) is unexpectedly'
              ' considered enabled; syncing.'
          )
          self.sync_enabled()
    else:
      self.sync_enabled()
      if self._enabled:
        self.sync_canwrite()

  @property
  def can_write(self) -> bool:
    return self._can_write

  def sync_canwrite(self) -> bool:
    """Syncs can_write, returns True if changed."""
    old = self._can_write
    self._can_write = get_canwrite(self.led_switches, self.channel_number)
    if self._can_write != old:
      print(
          f'Channel {self.channel_number} ({self.name}) canwrite has changed to'
          f' {self._can_write}'
      )
      return True
    return False

  @property
  def enabled(self) -> bool:
    return self._enabled

  @enabled.setter
  def enabled(self, value: bool) -> None:
    resp = self.led_switches.put_setswitch(self.channel_number, value)
    json_resp = alpaca_http_client.get_ok_response_json(resp)
    if json_resp['ErrorNumber'] == '0':
      # Successfully set the value.
      self._enabled = value
      print(
          ('Enabled' if self.enabled else 'Disabled'),
          f'channel {self.channel_number} ({self.name})',
      )
    else:
      self.sync_bools()

  def sync_enabled(self):
    old = self._enabled
    self._enabled = get_getswitch(self.led_switches, self.channel_number)
    if self._enabled != old:
      print(
          f'Channel {self.channel_number} ({self.name}) enabled has changed to'
          f' {self._enabled}'
      )
      return True
    return False


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
      'brightness', type=int, help='The LED brightness value.', default=1000
  )

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

  channels = [
      LedChannel(led_switches=led_switches, channel_number=0, name='Red'),
      LedChannel(led_switches=led_switches, channel_number=1, name='Green'),
      LedChannel(led_switches=led_switches, channel_number=2, name='Blue'),
      LedChannel(led_switches=led_switches, channel_number=3, name='White'),
  ]

  changed = True
  while True:
    for mask in range(1, 16):
      # Enable channels in mask that aren't already enabled.
      for channel in channels:
        if mask & (1 << channel.channel_number) and not channel.enabled:
          # Should be enabled.
          channel.enabled = True
          if channel.enabled:
            changed = True

      # Disable channels not in mask that are currently enabled.
      for channel in channels:
        if channel.enabled and not (mask & (1 << channel.channel_number)):
          # Should be disabled.
          channel.enabled = False
          if not channel.enabled:
            changed = True

      if changed:
        cover_calibrator.put_calibratoron(args.brightness)
        changed = False


if __name__ == '__main__':
  main()
