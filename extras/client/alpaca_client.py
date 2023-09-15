#!/usr/bin/env python3
"""API for accessing Alpaca Servers."""

# TODO(jamessynge): Consider caching selected responses from the Alpaca server
# for a short time, with invalidation of some of those responses when other
# operations are performed, or other operations fail. For example, the number of
# switches that a switch device supports is unlikely to change very often (but
# might when a jumper is changed or the device reboots); therefore, caching the
# number for a few seconds is likely fine, and could make quite a difference to
# the performance of an application, esp. when the server is relatively slow, as
# in the case of TinyAlpacaServer running on a 16MHz Arduino.
#
# https://www.dataleadsfuture.com/implement-a-cache-decorator-with-time-to-live-feature-in-python/

import argparse
import dataclasses
import functools
import typing
from typing import ClassVar, List, Sequence, Type, Union

import alpaca_discovery
import alpaca_http_client
import alpaca_model

EDeviceType = alpaca_model.EDeviceType
ConfiguredDevice = alpaca_model.ConfiguredDevice
HttpDeviceBase = alpaca_http_client.HttpDeviceBase
AlpacaHttpClient = alpaca_http_client.AlpacaHttpClient


@dataclasses.dataclass(frozen=True)
class AlpacaClient:
  """Provides access to an ASCOM Alpaca HTTP Server."""

  http_client: AlpacaHttpClient

  @classmethod
  def from_http_client(cls, http_client: AlpacaHttpClient) -> 'AlpacaClient':
    """Given an AlpacaHttpClient, returns an AlpacaClient.

    To avoid duplicate instances of AlpacaClient being created for a single
    AlpacaHttpClient, we stash an AlpacaClient in AlpacaHttpClient.

    Args:
      http_client: The AlpacaHttpClient for which an AlpacaClient is desired.

    Returns:
      The AlpacaClient.
    """
    stashed = http_client.stash.get_stashed('alpaca_client', default=None)
    if isinstance(stashed, AlpacaClient):
      return stashed
    else:
      alpaca_client = cls(http_client)
      # We use stash_weakly so that it is dropped (hence any caches are dropped)
      # if there are no strong references to the instance.
      http_client.stash.stash_weakly('alpaca_client', alpaca_client)
      return alpaca_client

  @functools.cached_property
  def description(self) -> alpaca_model.ServerDescription:
    """Returns the description of the server."""
    return self.http_client.server_description()

  @functools.cached_property
  def _configured_devices(self) -> Sequence[alpaca_model.ConfiguredDevice]:
    return self.http_client.configured_devices()

  def configured_devices(self) -> List[alpaca_model.ConfiguredDevice]:
    return list(self._configured_devices)

  @functools.cached_property
  def _devices(self) -> Sequence['AlpacaDevice']:
    return [create_alpaca_device(cd, self) for cd in self._configured_devices]

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

  http_device: alpaca_http_client.HttpDeviceBase
  _alpaca_client: AlpacaClient | None = None
  _configured_device: alpaca_model.ConfiguredDevice | None = None

  def __post_init__(self):
    if self._alpaca_client is None:
      self._alpaca_client = AlpacaClient.from_http_client(
          self.http_device.client
      )

  @property
  def client(self) -> AlpacaClient:
    return typing.cast(AlpacaClient, self._alpaca_client)

  @property
  def device_type(self) -> alpaca_model.EDeviceType:
    return self.http_device.device_type

  @property
  def device_number(self) -> int:
    return self.http_device.device_number

  def connected(self) -> bool:
    response = self.http_device.get_connected()
    return alpaca_http_client.get_ok_response_json_value_bool(response)

  def description(self) -> str:
    response = self.http_device.get_description()
    return alpaca_http_client.get_ok_response_json_value_str(response)

  def driver_info(self) -> str:
    response = self.http_device.get_driverinfo()
    return alpaca_http_client.get_ok_response_json_value_str(response)

  def driver_version(self) -> str:
    response = self.http_device.get_driverversion()
    return alpaca_http_client.get_ok_response_json_value_str(response)

  def interface_version(self) -> int:
    response = self.http_device.get_interfaceversion()
    return alpaca_http_client.get_ok_response_json_value_int(response)

  def name(self) -> str:
    response = self.http_device.get_description()
    return alpaca_http_client.get_ok_response_json_value_str(response)

  def supported_actions(self) -> List[str]:
    response = self.http_device.get_supportedactions()
    return alpaca_http_client.get_ok_response_json_value_array(response)


@dataclasses.dataclass(frozen=True)
class AlpacaSwitch(AlpacaDevice):
  """Client for accessing an Alpaca Switch device."""

  DEVICE_TYPE: ClassVar[EDeviceType] = EDeviceType.Switch

  http_device: alpaca_http_client.HttpSwitch
  _single_switches: dict[int, 'AlpacaSingleSwitch'] = dataclasses.field(
      init=False, default_factory=dict
  )

  def get_single_switch(
      self, switch_id: int
  ) -> Union['AlpacaSingleSwitch', None]:
    if switch_id not in self._single_switches:
      if switch_id >= self.max_switch():
        return None
      self._single_switches[switch_id] = AlpacaSingleSwitch(self, switch_id)
    return self._single_switches[switch_id]

  def max_switch(self) -> int:
    """Returns the number of switches."""
    response = self.http_device.get_maxswitch()
    return alpaca_http_client.get_ok_response_json_value_int(response)

  def can_write(self, switch_id: int) -> bool:
    response = self.http_device.get_canwrite(switch_id)
    return alpaca_http_client.get_ok_response_json_value_bool(response)

  def get_switch(self, switch_id: int) -> bool:
    """Returns the value of a boolean switch."""
    response = self.http_device.get_getswitch(switch_id)
    return alpaca_http_client.get_ok_response_json_value_bool(response)

  def get_switch_description(self, switch_id: int) -> str:
    """Returns the description of a switch."""
    response = self.http_device.get_getswitchdescription(switch_id)
    return alpaca_http_client.get_ok_response_json_value_str(response)

  def get_switch_name(self, switch_id: int) -> str:
    """Returns the name of a switch."""
    response = self.http_device.get_getswitchname(switch_id)
    return alpaca_http_client.get_ok_response_json_value_str(response)

  def get_switch_value(self, switch_id: int) -> float:
    """Returns the value of a switch as a floating point number."""
    response = self.http_device.get_getswitchvalue(switch_id)
    return alpaca_http_client.get_ok_response_json_value_float(response)

  def get_min_switch_value(self, switch_id: int) -> float:
    """Returns the minimum value of a switch as a floating point number."""
    response = self.http_device.get_minswitchvalue(switch_id)
    return alpaca_http_client.get_ok_response_json_value_float(response)

  def get_max_switch_value(self, switch_id: int) -> float:
    """Returns the maximum value of a switch as a floating point number."""
    response = self.http_device.get_maxswitchvalue(switch_id)
    return alpaca_http_client.get_ok_response_json_value_float(response)

  def get_switch_step(self, switch_id: int) -> float:
    """Returns the step size of a switch as a floating point number."""
    response = self.http_device.get_switchstep(switch_id)
    return alpaca_http_client.get_ok_response_json_value_float(response)


@dataclasses.dataclass()
class AlpacaSingleSwitch:
  """Client for accessing a single switch of an Alpaca Switch device."""

  switch_device: AlpacaSwitch
  switch_id: int
  name: str = dataclasses.field(init=False)
  description: str = dataclasses.field(init=False)
  min_value: float = dataclasses.field(init=False)
  max_value: float = dataclasses.field(init=False)
  step: float = dataclasses.field(init=False)

  def __post_init__(self):
    self.name = self.switch_device.get_switch_name(self.switch_id)
    self.description = self.switch_device.get_switch_description(self.switch_id)
    self.min_value = self.switch_device.get_min_switch_value(self.switch_id)
    self.max_value = self.switch_device.get_max_switch_value(self.switch_id)
    self.step = self.switch_device.get_switch_step(self.switch_id)

  def can_write(self, switch_id: int) -> bool:
    return self.switch_device.can_write(switch_id)

  def as_bool(self, switch_id: int) -> bool:
    return self.switch_device.get_switch(switch_id)

  def as_float(self, switch_id: int) -> float:
    return self.switch_device.get_switch_value(switch_id)

  def set(self, value: bool | float) -> None:
    if isinstance(value, bool):
      response = self.switch_device.http_device.put_setswitch(
          self.switch_id, value
      )
    else:
      response = self.switch_device.http_device.put_setswitchvalue(
          self.switch_id, value
      )
    alpaca_http_client.verify_ok_response(response)


def device_type_to_class(device_type: EDeviceType) -> Type[AlpacaDevice]:
  """Returns an HttpDeviceBase instance appropriate for the device."""
  # if device_type == EDeviceType.Camera:
  #   return HttpCamera
  # if device_type == EDeviceType.CoverCalibrator:
  #   return HttpCoverCalibrator
  # if device_type == EDeviceType.Dome:
  #   return HttpDome
  # if device_type == EDeviceType.FilterWheel:
  #   return HttpFilterWheel
  # if device_type == EDeviceType.Focuser:
  #   return HttpFocuser
  # if device_type == EDeviceType.ObservingConditions:
  #   return HttpObservingConditions
  # if device_type == EDeviceType.Rotator:
  #   return HttpRotator
  # if device_type == EDeviceType.SafetyMonitor:
  #   return HttpSafetyMonitor
  if device_type == EDeviceType.Switch:
    return AlpacaSwitch
  # if device_type == EDeviceType.Telescope:
  #   return HttpTelescope
  raise UserWarning(f'Unsupported device type: {device_type}')


@typing.overload
def create_alpaca_device(
    device: ConfiguredDevice,
    client: AlpacaClient | AlpacaHttpClient,
) -> AlpacaDevice:
  ...


@typing.overload
def create_alpaca_device(
    device: HttpDeviceBase,
    client: AlpacaClient | AlpacaHttpClient | None = None,
) -> AlpacaDevice:
  ...


def create_alpaca_device(
    device: ConfiguredDevice | HttpDeviceBase,
    client: AlpacaClient | AlpacaHttpClient | None,
) -> AlpacaDevice:
  """Returns an AlpacaDevice instance appropriate for the device.

  Args:
    device: Specifies the underlying device for which an AlpacaDevice is
      required.
    client: Specifies the client of the Alpaca server that has the device. If
      None, then the device must be derived from HttpDeviceBase. Ignored if the
      device is an HttpDeviceBase instance.
  """
  if isinstance(device, HttpDeviceBase):
    # we override the client in this case, taking it from the device. While this
    # might obscure some programming errors, it makes this function relatively
    # simple to use.
    client = device.client

  if isinstance(client, AlpacaHttpClient):
    http_client = client
    alpaca_client = AlpacaClient.from_http_client(client)
  elif isinstance(client, AlpacaClient):
    alpaca_client = client
    http_client = client.http_client
  else:
    raise ValueError('A client is required for a ConfiguredDevice.')

  if isinstance(device, ConfiguredDevice):
    http_device = alpaca_http_client.create_http_device(
        client=http_client, configured_device=device
    )
  else:
    assert isinstance(device, HttpDeviceBase)
    http_device = device

  cls = device_type_to_class(http_device.device_type)
  return cls(http_device=http_device, alpaca_client=alpaca_client)


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
  clients: List[AlpacaHttpClient] = alpaca_http_client.find_servers(
      **cli_kwargs
  )


if __name__ == '__main__':
  main()
