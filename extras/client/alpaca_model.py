"""Data classes and enums for representing Alpaca servers and devices."""

import dataclasses
import enum
from typing import Any, Callable, Dict, Optional, Union


class EDeviceType(enum.Enum):
  """The Alpaca device types."""

  # pylint: disable=invalid-name
  Camera = enum.auto()
  CoverCalibrator = enum.auto()
  Dome = enum.auto()
  FilterWheel = enum.auto()
  Focuser = enum.auto()
  ObservingConditions = enum.auto()
  Rotator = enum.auto()
  SafetyMonitor = enum.auto()
  Switch = enum.auto()
  Telescope = enum.auto()
  # pylint: enable=invalid-name

  @classmethod
  def _missing_(cls, value) -> Union['EDeviceType', None]:
    if isinstance(value, str):
      name = value.lower()
      for member in cls:
        if member.name.lower() == name:
          return member
    return None

  def api_name(self):
    return self.name.lower()


class ECoverStatus(enum.IntEnum):
  """Describes the state of a telescope cover."""
  NOT_PRESENT = 0
  CLOSED = 1
  MOVING = 2
  OPEN = 3
  UNKNOWN = 4
  ERROR = 5

  @classmethod
  def _missing_(cls, value) -> Union['ECoverStatus', None]:
    if isinstance(value, int):
      for member in cls:
        if member == value:
          return member
    return None


@dataclasses.dataclass
class ServerDescription:
  """The fields of an Alpaca Server Description value."""

  server_name: str
  manufacturer: str
  manufacturer_version: str
  location: str

  @staticmethod
  def from_response_value(sd_dict: Dict[str, Any]) -> 'ServerDescription':
    return ServerDescription(
        server_name=sd_dict['ServerName'],
        manufacturer=sd_dict['Manufacturer'],
        manufacturer_version=sd_dict['ManufacturerVersion'],
        location=sd_dict['Location'],
    )


@dataclasses.dataclass
class ConfiguredDevice:
  """The fields of an Alpaca Configured Device value."""

  device_name: str
  device_type: EDeviceType
  device_number: int
  unique_id: str

  @staticmethod
  def from_dict(cd_dict: Dict[str, Any]) -> 'ConfiguredDevice':
    return ConfiguredDevice(
        device_name=cd_dict['DeviceName'],
        device_type=EDeviceType[cd_dict['DeviceType']],
        device_number=cd_dict['DeviceNumber'],
        unique_id=cd_dict['UniqueID'],
    )


ConfiguredDeviceFilterFunc = Callable[[ConfiguredDevice], bool]


def make_configured_device_filter(
    device_number: Optional[int] = None,
    device_type: Optional[EDeviceType] = None,
) -> ConfiguredDeviceFilterFunc:
  """Returns a ConfiguredDeviceFilterFunc applying the restrictions."""

  def the_filter(configured_device: ConfiguredDevice) -> bool:
    if (
        device_number is not None
        and configured_device.device_number != device_number
    ):
      return False
    if device_type is not None and configured_device.device_type != device_type:
      return False
    return True

  return the_filter
