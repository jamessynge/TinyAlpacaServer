#!/usr/bin/env python3
"""Makes HTTP requests to Alpaca servers, returns HTTP responses."""

import enum
import random
import sys
from typing import Dict, Optional, Sequence, Union

import requests

GET = 'GET'
OPTIONS = 'OPTIONS'
POST = 'POST'
PUT = 'PUT'


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
  def _missing_(cls, name):
    for member in cls:
      if member.name.lower() == name.lower():
        return member

  def api_name(self):
    return self.name.lower()


def int_value_or_default(name: str,
                         value: Optional[int],
                         dflt: int,
                         minimum: int = 0,
                         maximum: int = 4294967295) -> int:
  """Returns value if provided and valid, or dflt if not provided."""
  if value is None:
    return dflt
  elif not isinstance(value, int) or value < minimum or value > maximum:
    raise ValueError(f'Unexpected {name}: {value!r}')
  else:
    return value


class AlpacaClient(object):
  """Device independent Alpaca client, for communicating with one server."""

  def __init__(self,
               url_base: str,
               client_id: Optional[int] = None,
               initial_client_transaction_id: Optional[int] = None):
    self.url_base = url_base
    self.mgmt_url_base = f'{url_base}/management'
    self.device_url_base = f'{url_base}/api/v1'
    self.client_id = int_value_or_default(
        'client_id',
        client_id,
        random.randint(1, 65535),
        minimum=100,
        maximum=1000000)
    self.next_client_transaction_id = int_value_or_default(
        'initial_client_transaction_id',
        initial_client_transaction_id,
        random.randint(1, 1000000),
        minimum=100,
        maximum=1000000)
    self.session = requests.session()

  def gen_standard_params(self, increment_transaction=True) -> Dict[str, str]:
    result = dict(
        ClientID=self.client_id,
        ClientTransactionID=self.next_client_transaction_id)
    if increment_transaction:
      self.next_client_transaction_id += 1
    return result

  def gen_management_request(self, path_extension: str) -> requests.Request:
    params = self.gen_standard_params()
    data = None
    url = f'{self.mgmt_url_base}/{path_extension}'
    return requests.Request(method=GET, url=url, data=data, params=params)

  def send(
      self, request: Union[requests.Request, requests.PreparedRequest]
  ) -> requests.Response:
    """Sends an HTTP request, returns the response."""
    if isinstance(request, requests.Request):
      request = request.prepare()
    request: requests.PreparedRequest
    # print('sending PreparedRequest')
    # print(request)
    # print('method', request.method)
    # print('url', request.url)
    r = self.session.send(request)
    # print('response', r)
    # print('content', r.content)
    return r

  def get_apiversions(self) -> requests.Response:
    req = self.gen_management_request('apiversions')
    return self.send(req)

  def get_configureddevices(self) -> requests.Response:
    req = self.gen_management_request('v1/configureddevices')
    return self.send(req)

  def get_description(self) -> requests.Response:
    req = self.gen_management_request('v1/description')
    return self.send(req)


class DeviceBase(object):
  """Base class of device type specific clients."""

  def __init__(self,
               client: AlpacaClient,
               device_type: str,
               device_number: int,
               name: str = '',
               unique_id: str = ''):
    self.client = client
    self.device_type = device_type
    self.device_number = device_number
    self.name = name
    self.unique_id = unique_id
    self.device_url_base = (
        f'{client.device_url_base}/{device_type}/{device_number}')

  def make_url_path(self, device_method: str) -> str:
    return f'{self.device_url_base}/{device_method}'

  def gen_request(self, http_method: str, alpaca_method: str,
                  **kwargs) -> requests.Request:
    params = self.client.gen_standard_params()
    params.update(kwargs)
    data = None
    if http_method in (PUT, POST):
      data, params = (params, data)
    url = self.make_url_path(alpaca_method)
    return requests.Request(
        method=http_method, url=url, data=data, params=params)

  def _put(self, device_method: str, **kwargs) -> requests.Response:
    req = self.gen_request(PUT, device_method, **kwargs)
    return self.client.send(req)

  def put_action(self, action_name: str, parameters: str) -> requests.Response:
    return self._put('action', Action=action_name, Parameters=parameters)

  def _put_command(self, device_method: str, command: str,
                   raw: bool) -> requests.Response:
    return self._put(device_method, Command=command, Raw=str(raw))

  def put_commandblind(self, command: str, raw: bool) -> requests.Response:
    return self._put_command('commandblind', command, raw)

  def put_commandbool(self, command: str, raw: bool) -> requests.Response:
    return self._put_command('commandbool', command, raw)

  def put_commandstring(self, command: str, raw: bool) -> requests.Response:
    return self._put_command('commandstring', command, raw)

  def put_connected(self, connected: bool) -> requests.Response:
    return self._put('connected', Connected=str(connected))

  def _get(self, device_method: str, **kwargs) -> requests.Response:
    req = self.gen_request(GET, device_method, **kwargs)
    return self.client.send(req)

  def get_connected(self) -> requests.Response:
    return self._get('connected')

  def get_description(self) -> requests.Response:
    return self._get('description')

  def get_driverinfo(self) -> requests.Response:
    return self._get('driverinfo')

  def get_driverversion(self) -> requests.Response:
    return self._get('driverversion')

  def get_interfaceversion(self) -> requests.Response:
    return self._get('interfaceversion')

  def get_name(self) -> requests.Response:
    return self._get('name')

  def get_supportedactions(self) -> requests.Response:
    return self._get('supportedactions')


class CoverCalibrator(DeviceBase):
  """Makes HTTP requests to a CoverCalibrator device."""

  def __init__(self, client: AlpacaClient, device_number: int, **kwargs):
    super().__init__(client, 'covercalibrator', device_number, **kwargs)

  def get_brightness(self) -> requests.Response:
    return self._get('brightness')

  def get_calibratorstate(self) -> requests.Response:
    return self._get('calibratorstate')

  def get_coverstate(self) -> requests.Response:
    return self._get('coverstate')

  def get_maxbrightness(self) -> requests.Response:
    return self._get('maxbrightness')

  def put_calibratoroff(self) -> requests.Response:
    return self._put('calibratoroff')

  def put_calibratoron(self, brightness: int) -> requests.Response:
    return self._put('calibratoron', Brightness=brightness)

  def put_closecover(self) -> requests.Response:
    return self._put('closecover')

  def put_haltcover(self) -> requests.Response:
    return self._put('haltcover')

  def put_opencover(self) -> requests.Response:
    return self._put('opencover')


class ObservingConditions(DeviceBase):
  """Makes HTTP requests to an ObservingConditions device."""

  def get_averageperiod(self) -> requests.Response:
    return self._get('averageperiod')

  def get_cloudcover(self) -> requests.Response:
    return self._get('cloudcover')

  def get_dewpoint(self) -> requests.Response:
    return self._get('dewpoint')

  def get_humidity(self) -> requests.Response:
    return self._get('humidity')

  def get_pressure(self) -> requests.Response:
    return self._get('pressure')

  def get_rainrate(self) -> requests.Response:
    return self._get('rainrate')

  def get_skybrightness(self) -> requests.Response:
    return self._get('skybrightness')

  def get_skyquality(self) -> requests.Response:
    return self._get('skyquality')

  def get_skytemperature(self) -> requests.Response:
    return self._get('skytemperature')

  def get_starfwhm(self) -> requests.Response:
    return self._get('starfwhm')

  def get_temperature(self) -> requests.Response:
    return self._get('temperature')

  def get_winddirection(self) -> requests.Response:
    return self._get('winddirection')

  def get_windgust(self) -> requests.Response:
    return self._get('windgust')

  def get_windspeed(self) -> requests.Response:
    return self._get('windspeed')

  def get_sensordescription(self) -> requests.Response:
    return self._get('sensordescription')

  def get_timesincelastupdate(self) -> requests.Response:
    return self._get('timesincelastupdate')

  def put_averageperiod(self, average_period: float) -> requests.Response:
    return self._put('averageperiod', AveragePeriod=average_period)

  def put_refresh(self) -> requests.Response:
    return self._put('refresh')


class SafetyMonitor(DeviceBase):
  """Makes HTTP requests to an SafetyMonitor device."""

  def get_issafe(self) -> requests.Response:
    return self._get('issafe')


def main(argv: Sequence[str]) -> None:
  # mgmt_server = 'https://private-d6fe6-ascom.apiary-mock.com/ascom'
  # device_api_server = 'https://virtserver.swaggerhub.com/ASCOMInitiative'
  if len(argv) != 1:
    raise ValueError('Expects one arg, the base of the URL')

  url_base = argv[0]

  client = AlpacaClient(url_base)
  print('get_apiversions', client.get_apiversions())
  print('get_description', client.get_description())
  print('get_configureddevices', client.get_configureddevices())


if __name__ == '__main__':
  main(sys.argv[1:])
