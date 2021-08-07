#!/usr/bin/env python3
"""Makes HTTP requests to Alpaca servers, returns HTTP responses."""

import dataclasses
import enum
import random
import sys
import threading
import time
from typing import Callable, Dict, List, Optional, Sequence, Union, Type, TypeVar

import alpaca_discovery
import install_advice

try:
  import requests  # pylint: disable=g-import-not-at-top
except ImportError:
  install_advice.install_advice('requests')
# build_cleaner doesn't find imports that aren't at the top level, so we repeat
# the import here.
import requests  # pylint: disable=g-import-not-at-top,g-bad-import-order

GET = 'GET'
OPTIONS = 'OPTIONS'
POST = 'POST'
PUT = 'PUT'

_T = TypeVar('_T')


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


@dataclasses.dataclass
class ConfiguredDevice:
  device_name: str
  device_type: EDeviceType
  device_number: int
  unique_id: str


class AlpacaHttpClient(object):
  """Device independent Alpaca client, for communicating with one server."""

  client_id = random.randint(1, 65535)
  last_client_transaction_id = random.randint(1, 1000000)
  last_client_transaction_id_lock = threading.Lock()

  def __init__(self, url_base: str):
    self.url_base = url_base
    self.mgmt_url_base = f'{url_base}/management'
    self.device_url_base = f'{url_base}/api/v1'
    self.session = requests.session()
    self._configured_devices: Optional[List[ConfiguredDevice]] = None

  def gen_next_client_transaction_id(self):
    with self.last_client_transaction_id_lock:
      self.last_client_transaction_id += 1
      return self.last_client_transaction_id

  def gen_standard_params(self) -> Dict[str, str]:
    result = dict(
        ClientID=self.client_id,
        ClientTransactionID=self.gen_next_client_transaction_id())
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
    # print(request.method, request.url)
    # print('Headers:', request.headers)
    # if request.body:
    #   print('body', f'{request.body!r}')
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

  def apiversions(self) -> List[int]:
    resp = self.get_apiversions()
    resp_jv = resp.json()
    return resp_jv['Value']

  def configured_devices(
      self,
      device_type: Optional[EDeviceType] = None) -> List[ConfiguredDevice]:
    """Returns the configured devices of device_type provided by the server."""
    if self._configured_devices is None:
      resp = self.get_configureddevices()
      resp_jv = resp.json()
      cds: List[ConfiguredDevice] = []
      for info in resp_jv['Value']:
        cds.append(
            ConfiguredDevice(
                device_name=info['DeviceName'],
                device_type=EDeviceType[info['DeviceType']],
                device_number=info['DeviceNumber'],
                unique_id=info['UniqueID']))
      self._configured_devices = cds
    if device_type is None:
      return self._configured_devices
    return [
        cd for cd in self._configured_devices if cd.device_type == device_type
    ]


ServerFilterFunc = Callable[[AlpacaHttpClient], bool]


def has_device_type(client: AlpacaHttpClient, device_type: EDeviceType) -> bool:
  """Returns True if the server has a device of the specified type."""
  return bool(client.configured_devices(device_type))


def make_has_device_type_filter(device_type: EDeviceType) -> ServerFilterFunc:

  def server_filter(client: AlpacaHttpClient) -> bool:
    return has_device_type(client, device_type)

  return server_filter


class HttpDeviceBase(object):
  """Base class of device type specific clients."""

  @classmethod
  def find_devices(cls: Type[_T],
                   servers: Optional[List[AlpacaHttpClient]] = None,
                   max_discovery_time: float = 5.0,
                   device_number: Optional[int] = None,
                   server_cls: Type[AlpacaHttpClient] = AlpacaHttpClient,
                   min_required=Optional[int],
                   max_allowed=Optional[int]) -> List[_T]:
    """Returns devices of type cls.DEVICE_TYPE."""
    if cls == HttpDeviceBase:
      raise ValueError('Call find_devices as a class method of a sub-class '
                       'of HttpDeviceBase, e.g. HttpSwitch.')
    device_type: EDeviceType = cls.DEVICE_TYPE
    if servers is None:
      servers = find_servers(
          max_discovery_time=max_discovery_time,
          server_filter=make_has_device_type_filter(device_type),
          cls=server_cls)
    devices: List[HttpDeviceBase] = []
    for server in servers:
      for configured_device in server.configured_devices(device_type):
        if (device_number is not None and
            device_number != configured_device.device_number):
          continue
        devices.append(
            cls(server, device_type, configured_device.device_number))
    if min_required is not None and len(devices) < min_required:
      sys.stdout.flush()
      print(
          f'Did not find at least {min_required} servers with a device '
          f'of type: {device_type.name}',
          file=sys.stderr,
          flush=True)
      time.sleep(10)
      sys.exit(1)
    if max_allowed is not None and len(devices) > max_allowed:
      sys.stdout.flush()
      print(
          f'Found too many ({len(devices)}) devices of type: {device_type.name}',
          file=sys.stderr,
          flush=True)
      time.sleep(10)
      sys.exit(1)
    return devices

  @classmethod
  def find_first_device(
      cls: Type[_T],
      servers: Optional[List[AlpacaHttpClient]] = None,
      max_discovery_time: float = 5.0,
      device_number: Optional[int] = None,
      server_cls: Type[AlpacaHttpClient] = AlpacaHttpClient) -> _T:
    return cls.find_devices(
        servers=servers,
        max_discovery_time=max_discovery_time,
        device_number=device_number,
        server_cls=server_cls,
        min_required=1)[0]

  @classmethod
  def find_sole_device(
      cls: Type[_T],
      servers: Optional[List[AlpacaHttpClient]] = None,
      max_discovery_time: float = 5.0,
      device_number: Optional[int] = None,
      server_cls: Type[AlpacaHttpClient] = AlpacaHttpClient) -> _T:
    return cls.find_devices(
        servers=servers,
        max_discovery_time=max_discovery_time,
        device_number=device_number,
        server_cls=server_cls,
        min_required=1,
        max_allowed=1)[0]

  @classmethod
  def create_from_configured_device(cls: Type[_T], client: AlpacaHttpClient,
                                    configured_device: ConfiguredDevice) -> _T:
    if cls.DEVICE_TYPE != configured_device.device_type:
      raise ValueError(f'Expected {cls.DEVICE_TYPE} in: {configured_device}')
    return cls(client, cls.DEVICE_TYPE, configured_device.device_number)

  def __init__(self, client: AlpacaHttpClient, device_type: EDeviceType,
               device_number: int):
    self.client = client
    self.device_type = device_type
    self.device_number = device_number
    self.device_url_base = (
        f'{client.device_url_base}/{device_type.api_name()}/{device_number}')

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


class HttpCoverCalibrator(HttpDeviceBase):
  """Makes CoverCalibrator HTTP requests, returns HTTP responses."""

  DEVICE_TYPE = EDeviceType.CoverCalibrator

  # def __init__(self, client: AlpacaHttpClient, device_number: int, **kwargs):
  #   super().__init__(client, EDeviceType.CoverCalibrator, device_number,
  #                    **kwargs)

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


class HttpObservingConditions(HttpDeviceBase):
  """Makes ObservingConditions HTTP requests, returns HTTP responses."""

  DEVICE_TYPE = EDeviceType.ObservingConditions

  # def __init__(self, client: AlpacaHttpClient, device_number: int, **kwargs):
  #   super().__init__(client, EDeviceType.ObservingConditions, device_number,
  #                    **kwargs)

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


class HttpSafetyMonitor(HttpDeviceBase):
  """Makes SafetyMonitor HTTP requests, returns HTTP responses."""

  DEVICE_TYPE = EDeviceType.SafetyMonitor

  #   def __init__(self, client: AlpacaHttpClient, device_number: int, **kwargs):
  #     super().__init__(client, EDeviceType.SafetyMonitor, device_number, **kwargs)

  def get_issafe(self) -> requests.Response:
    return self._get('issafe')


class HttpSwitch(HttpDeviceBase):
  """Makes Switch HTTP requests, returns HTTP responses."""

  DEVICE_TYPE = EDeviceType.Switch

  def get_maxswitch(self) -> requests.Response:
    return self._get('maxswitch')

  def get_canwrite(self, switch_id: int) -> requests.Response:
    return self._get('canwrite', Id=switch_id)

  def get_getswitch(self, switch_id: int) -> requests.Response:
    return self._get('getswitch', Id=switch_id)

  def get_getswitchdescription(self, switch_id: int) -> requests.Response:
    return self._get('getswitchdescription', Id=switch_id)

  def get_getswitchname(self, switch_id: int) -> requests.Response:
    return self._get('getswitchname', Id=switch_id)

  def get_getswitchvalue(self, switch_id: int) -> requests.Response:
    return self._get('getswitchvalue', Id=switch_id)

  def get_minswitchvalue(self, switch_id: int) -> requests.Response:
    return self._get('minswitchvalue', Id=switch_id)

  def get_maxswitchvalue(self, switch_id: int) -> requests.Response:
    return self._get('maxswitchvalue', Id=switch_id)

  def get_switchstep(self, switch_id: int) -> requests.Response:
    return self._get('switchstep', Id=switch_id)

  def put_setswitch(self, switch_id: int, state: bool) -> requests.Response:
    return self._put('setswitch', Id=switch_id, State=state)

  def put_setswitchname(self, switch_id: int, name: str) -> requests.Response:
    return self._put('setswitchname', Id=switch_id, Name=name)

  def put_setswitchvalue(self, switch_id: int,
                         value: float) -> requests.Response:
    return self._put('setswitchvalue', Id=switch_id, Value=value)


def device_type_to_class(device_type: EDeviceType) -> Type[HttpDeviceBase]:
  """Returns an HttpDeviceBase instance appropriate for the device."""
  # if device_type == EDeviceType.Camera:
  #   return HttpCamera
  if device_type == EDeviceType.CoverCalibrator:
    return HttpCoverCalibrator
  # if device_type == EDeviceType.Dome:
  #   return HttpDome
  # if device_type == EDeviceType.FilterWheel:
  #   return HttpFilterWheel
  # if device_type == EDeviceType.Focuser:
  #   return HttpFocuser
  if device_type == EDeviceType.ObservingConditions:
    return HttpObservingConditions
  # if device_type == EDeviceType.Rotator:
  #   return HttpRotator
  if device_type == EDeviceType.SafetyMonitor:
    return HttpSafetyMonitor
  if device_type == EDeviceType.Switch:
    return HttpSwitch
  # if device_type == EDeviceType.Telescope:
  #   return HttpTelescope
  raise UserWarning(f'Unsupported device type: {device_type}')


def create_http_device(client: AlpacaHttpClient,
                       configured_device: ConfiguredDevice) -> HttpDeviceBase:
  """Returns an HttpDeviceBase instance appropriate for the device."""
  if configured_device.device_type == EDeviceType.CoverCalibrator:
    cls = HttpCoverCalibrator
  elif configured_device.device_type == EDeviceType.ObservingConditions:
    cls = HttpObservingConditions
  elif configured_device.device_type == EDeviceType.SafetyMonitor:
    cls = HttpSafetyMonitor
  elif configured_device.device_type == EDeviceType.Switch:
    cls = HttpSwitch
  else:
    raise UserWarning(
        f'Unsupported device type: {configured_device.device_type}')
  return cls.create_from_configured_device(client, configured_device)


def find_servers(
    max_discovery_time: float = 5.0,
    server_filter: Optional[ServerFilterFunc] = None,
    min_required=0,
    verbose=False,
    cls: Type[AlpacaHttpClient] = AlpacaHttpClient) -> List[AlpacaHttpClient]:
  """Return a client for the first Alpaca server discovered, or None."""
  if verbose:
    print('Searching for Alpaca servers using the discovery protocol...')
  results: List[AlpacaHttpClient] = []

  def discovery_response_handler(
      dr: alpaca_discovery.DiscoveryResponse) -> None:
    url_base = f'http://{dr.get_alpaca_server_addr()}'
    if verbose:
      print(f'Found a server at {url_base}')
    client = cls(url_base=url_base)
    if server_filter and not server_filter(client):
      if verbose:
        print('Not accepted by server_filter')
    else:
      results.append(client)

  alpaca_discovery.perform_discovery(
      discovery_response_handler, max_wait_time=max_discovery_time)
  if len(results) < min_required:
    sys.stdout.flush()
    print('Found no servers!', file=sys.stderr, flush=True)
    time.sleep(10)
    sys.exit(1)
  return results


def main(argv: Sequence[str]) -> None:
  if not argv:
    client = find_servers(min_required=1)[0]
  else:
    if len(argv) != 1:
      raise ValueError('Expects one arg, the base of the URL')
    url_base = argv[0]
    client = AlpacaHttpClient(url_base)

  print('get_apiversions', client.get_apiversions())
  print('get_description', client.get_description())
  print('get_configureddevices', client.get_configureddevices())


if __name__ == '__main__':
  main(sys.argv[1:])
