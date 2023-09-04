#!/usr/bin/env python3
"""API for accessing Alpaca Servers.
"""

import abc
import argparse
import dataclasses
import functools
from typing import List, Sequence

import alpaca_discovery
import alpaca_model
import alpaca_http_client


@dataclasses.dataclass(frozen=True)
class AlpacaClient:
  """Provides access to an Alpaca Server."""

  client: alpaca_http_client.AlpacaHttpClient

  @functools.cached_property
  def description(self) -> alpaca_model.ServerDescription:
    """Returns the description of the server."""
    return self.client.server_description()

  @functools.cached_property
  def _configured_devices(self) -> Sequence[alpaca_model.ConfiguredDevice]:
    return self.client.configured_devices()

  def configured_devices(self) -> List[alpaca_model.ConfiguredDevice]:
    return list(self._configured_devices)

  @functools.cached_property
  def _devices(self) -> Sequence['AlpacaDevice']:
    raise NotImplementedError()

  def devices(self) -> List['AlpacaDevice']:
    """Returns the list of devices provided by the server.

    We assume the list is immutable (can be cached), though this might
    not be true in the case of a long running client, accessing a server
    which is restarted with changes to the set of devices.

    TODO(jamessynge): Think about how to deal with this. Clearly we'll need to
    refetch the list of devices periodically, and compare the list with the info
    we previously fetched; in particular, has any device been added or removed,
    had it's (apparently) cacheable properties changed (e.g. UniqueId,
    Name, or Driver Version).
    """
    return list(self._devices)  # Make a mutable copy of the list.


@dataclasses.dataclass(frozen=True)
class AlpacaDevice:
  """Base class for providing access to an Alpaca device.

  Derived class must provide a property `client` of type
  alpaca_http_client.HttpDeviceBase.
  TODO(jamessynge): Make this an absract base class, with a `client` property
  as described above.
  """

  server: AlpacaClient
  client: alpaca_http_client.HttpDeviceBase

  @property
  def device_type(self) -> alpaca_model.EDeviceType:
    return self.client.device_type

  @property
  def device_number(self) -> int:
    return self.client.device_number

  def connected(self) -> bool:
    response = self.client.get_connected()
    return alpaca_http_client.get_ok_response_json_value_bool(response)

  def description(self) -> str:
    response = self.client.get_description()
    return alpaca_http_client.get_ok_response_json_value_str(response)

  def driver_info(self) -> str:
    response = self.client.get_driverinfo()
    return alpaca_http_client.get_ok_response_json_value_str(response)

  def driver_version(self) -> str:
    response = self.client.get_driverversion()
    return alpaca_http_client.get_ok_response_json_value_str(response)

  def interface_version(self) -> int:
    response = self.client.get_interfaceversion()
    return alpaca_http_client.get_ok_response_json_value_int(response)

  def name(self) -> str:
    response = self.client.get_description()
    return alpaca_http_client.get_ok_response_json_value_str(response)

  def supported_actions(self) -> List[str]:
    response = self.client.get_supportedactions()
    return alpaca_http_client.get_ok_response_json_value_array(response)


@dataclasses.dataclass(frozen=True)
class AlpacaSwitch(AlpacaDevice):
  """Client for accessing an Alpaca Switch device."""

  client: alpaca_http_client.HttpSwitch

  def max_switch(self) -> int:
    """Returns the number of switches."""
    response = self.client.get_maxswitch()
    return alpaca_http_client.get_ok_response_json_value_int(response)

  def can_write(self, switch_id: int) -> bool:
    response = self.client.get_canwrite(switch_id)
    return alpaca_http_client.get_ok_response_json_value_bool(response)

  def get_switch(self, switch_id: int) -> bool:
    """Returns the value of a boolean switch."""
    response = self.client.get_getswitch(switch_id)
    return alpaca_http_client.get_ok_response_json_value_bool(response)

  def get_switch_description(self, switch_id: int) -> str:
    """Returns the description of a switch."""
    response = self.client.get_getswitchdescription(switch_id)
    return alpaca_http_client.get_ok_response_json_value_str(response)

  def get_switch_name(self, switch_id: int) -> str:
    """Returns the name of a switch."""
    response = self.client.get_getswitchname(switch_id)
    return alpaca_http_client.get_ok_response_json_value_str(response)

  def get_switch_value(self, switch_id: int) -> float:
    """Returns the value of a switch as a floating point number."""
    response = self.client.get_getswitchvalue(switch_id)
    return alpaca_http_client.get_ok_response_json_value_float(response)

  def get_min_switch_value(self, switch_id: int) -> float:
    """Returns the minimum value of a switch as a floating point number."""
    response = self.client.get_minswitchvalue(switch_id)
    return alpaca_http_client.get_ok_response_json_value_float(response)

  def get_max_switch_value(self, switch_id: int) -> float:
    """Returns the maximum value of a switch as a floating point number."""
    response = self.client.get_maxswitchvalue(switch_id)
    return alpaca_http_client.get_ok_response_json_value_float(response)

  def get_switch_step(self, switch_id: int) -> float:
    """Returns the step size of a  switch as a floating point number."""
    response = self.client.get_switchstep(switch_id)
    return alpaca_http_client.get_ok_response_json_value_float(response)


def main() -> None:
  parser = argparse.ArgumentParser(
      description='Describe Alpaca Servers.',
      parents=[
          alpaca_discovery.make_discovery_parser(),
          alpaca_http_client.make_url_base_parser(),
          alpaca_http_client.make_device_number_parser(),
          alpaca_http_client.make_device_type_parser(),
      ],
  )
  cli_args = parser.parse_args()
  cli_kwargs = vars(cli_args)
  clients: List[
      alpaca_http_client.AlpacaHttpClient
  ] = alpaca_http_client.find_servers(**cli_kwargs)


if __name__ == '__main__':
  main()
