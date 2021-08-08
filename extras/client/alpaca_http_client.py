#!/usr/bin/env python3
"""Makes HTTP requests to Alpaca servers, returns HTTP responses."""

import argparse
import dataclasses
import enum
import random
import sys
import threading
import time
from typing import Callable, Dict, List, Optional, Union, Type, TypeVar

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


def to_url_base(arg: str) -> str:
  """Parse an address string that a browser might accept and return a URL."""
  orig = arg

  scheme = 'http'
  parts = arg.split('://', 1)
  if len(parts) == 2:
    scheme, arg = parts

  path = ''
  parts = arg.split('/', 1)
  if len(parts) == 2:
    arg, path = parts

  port = 80
  parts = arg.split(':')
  if len(parts) > 2:
    raise ValueError(f'Unable to parse {orig!r} to produce a URL')
  elif len(parts) == 2:
    port = int(parts[1])
    arg = parts[0]
  host = arg

  if '?' in path:
    raise ValueError(f'URL must not contain a query string: {orig!r}')
  path = '/' + path
  if '//' in path:
    raise ValueError(f'URL path must not contain empty (//) segments: {orig!r}')

  path = path + '/'
  if '/../' in path:
    raise ValueError(
        f'URL path must not contain reverse (/../) segments: {orig!r}')

  path = path.replace('//', '/').strip('/')
  return ''.join([scheme, '://', host, ':', str(port), '/', path]).strip('/')


@dataclasses.dataclass
class ConfiguredDevice:
  device_name: str
  device_type: EDeviceType
  device_number: int
  unique_id: str


ConfiguredDeviceFilterFunc = Callable[[ConfiguredDevice], bool]


def make_configured_device_filter(
    device_number: Optional[int] = None,
    device_type: Optional[EDeviceType] = None) -> ConfiguredDeviceFilterFunc:
  """Returns a ConfiguredDeviceFilterFunc based on the kwargs."""

  def the_filter(configured_device: ConfiguredDevice) -> bool:
    if (device_number is not None and
        configured_device.device_number != device_number):
      return False
    if (device_type is not None and
        configured_device.device_type != device_type):
      return False
    return True

  return the_filter


class AlpacaHttpClient(object):
  """Device independent Alpaca client, for communicating with one server."""

  client_id = random.randint(1, 65535)
  last_client_transaction_id = random.randint(1, 1000000)
  last_client_transaction_id_lock = threading.Lock()

  def __init__(self, url_base: str):
    self.url_base = to_url_base(url_base)
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
      device_filter: Optional[ConfiguredDeviceFilterFunc] = None
  ) -> List[ConfiguredDevice]:
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
    if device_filter is None:
      return self._configured_devices
    return [cd for cd in self._configured_devices if device_filter(cd)]

  def description(self) -> str:
    resp = self.get_description()
    resp_jv = resp.json()
    return resp_jv['Value']


ServerFilterFunc = Callable[[AlpacaHttpClient], bool]


def has_configured_device(
    client: AlpacaHttpClient,
    configured_device_filter: ConfiguredDeviceFilterFunc) -> bool:
  """Returns True if the server has a matching device."""
  return bool(client.configured_devices(configured_device_filter))


def make_composite_server_filter(
    server_filter: Optional[ServerFilterFunc] = None,
    configured_device_filter: Optional[ConfiguredDeviceFilterFunc] = None
) -> ServerFilterFunc:
  """Returns a ServerFilterFunc based on the kwargs."""

  def the_filter(client: AlpacaHttpClient):
    if server_filter and not server_filter(client):
      return False
    if (configured_device_filter and
        not has_configured_device(client, configured_device_filter)):
      return False
    return True

  return the_filter


def find_servers(cls: Type[AlpacaHttpClient] = AlpacaHttpClient,
                 url_base: Optional[str] = None,
                 device_number: Optional[int] = None,
                 device_type: Optional[Union[str, EDeviceType]] = None,
                 server_filter: Optional[ServerFilterFunc] = None,
                 min_required_devices=0,
                 verbose=False,
                 **kwargs) -> List[AlpacaHttpClient]:
  """Returns clients for the Alpaca servers discovered."""
  if isinstance(device_type, str):
    device_type: EDeviceType = EDeviceType(device_type)
  the_filter = make_composite_server_filter(
      server_filter=server_filter,
      configured_device_filter=make_configured_device_filter(
          device_number=device_number, device_type=device_type))
  if url_base:
    client = cls(url_base=url_base)
    if not the_filter(client):
      if verbose:
        print(f'Server at {url_base} not accepted by filter(s)')
      return []
    return [client]

  if verbose:
    print('Searching for Alpaca servers using the discovery protocol...')
  results: List[AlpacaHttpClient] = []

  def discovery_response_handler(
      dr: alpaca_discovery.DiscoveryResponse) -> None:
    url_base = f'http://{dr.get_alpaca_server_addr()}'
    if verbose:
      print(f'Found a server at {url_base}')
    client = cls(url_base=url_base)
    if not the_filter(client):
      if verbose:
        print('Not accepted by the filter(s)')
    else:
      results.append(client)

  alpaca_discovery.perform_discovery(
      discovery_response_handler, verbose=verbose, **kwargs)
  if len(results) < min_required_devices:
    sys.stdout.flush()
    print('Found no servers!', file=sys.stderr, flush=True)
    time.sleep(10)
    sys.exit(1)
  return results


class HttpDeviceBase(object):
  """Base class of device type specific clients."""

  @classmethod
  def find_devices(cls: Type[_T],
                   servers: Optional[List[AlpacaHttpClient]] = None,
                   server_cls: Type[AlpacaHttpClient] = AlpacaHttpClient,
                   device_number: Optional[int] = None,
                   min_required_devices=Optional[int],
                   max_allowed_devices=Optional[int],
                   **kwargs) -> List[_T]:
    """Returns devices of type cls.DEVICE_TYPE."""
    if cls == HttpDeviceBase:
      raise ValueError('Call find_devices as a class method of a sub-class '
                       'of HttpDeviceBase, e.g. HttpSwitch.')
    device_type: EDeviceType = cls.DEVICE_TYPE
    if not servers:
      servers = find_servers(
          cls=server_cls,
          device_type=device_type,
          device_number=device_number,
          **kwargs)
    cd_filter = make_configured_device_filter(
        device_number=device_number, device_type=device_type)
    devices: List[HttpDeviceBase] = []
    for server in servers:
      for configured_device in server.configured_devices(cd_filter):
        devices.append(
            cls(server, device_type, configured_device.device_number))
    if min_required_devices is not None and len(devices) < min_required_devices:
      sys.stdout.flush()
      print(
          f'Did not find at least {min_required_devices} servers with a device '
          f'of type: {device_type.name}',
          file=sys.stderr,
          flush=True)
      time.sleep(10)
      sys.exit(1)
    if max_allowed_devices is not None and len(devices) > max_allowed_devices:
      sys.stdout.flush()
      print(
          f'Found too many ({len(devices)}) devices of type: {device_type.name}',
          file=sys.stderr,
          flush=True)
      time.sleep(10)
      sys.exit(1)
    return devices

  @classmethod
  def find_first_device(cls: Type[_T], **kwargs) -> _T:
    return cls.find_devices(min_required_devices=1, **kwargs)[0]

  @classmethod
  def find_sole_device(cls: Type[_T], **kwargs) -> _T:
    return cls.find_devices(
        min_required_devices=1, max_allowed_devices=1, **kwargs)[0]

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


def make_url_base_parser(
    required: bool = False,
    required_device_type: Optional[EDeviceType] = None
) -> argparse.ArgumentParser:
  """Returns a parser for a --url_base CLI argument."""
  parts = [
      'Base of URL before /api/v1/. If not specified, Alpaca Discovery ',
      'is used to find an Alpaca Server'
  ]
  if required_device_type is not None:
    parts.append(' with a device of type ' + required_device_type.name)
  parts.append('.')
  parser = argparse.ArgumentParser(add_help=False)
  parser.add_argument(
      '--url_base',
      '--url',
      metavar='URL_OR_ADDRESS',
      required=required,
      type=to_url_base,
      help=''.join(parts))
  return parser


def make_device_type_parser(required: bool = False) -> argparse.ArgumentParser:
  """Returns a parser for a --device_type CLI argument."""
  parser = argparse.ArgumentParser(add_help=False)
  parser.add_argument(
      '--device_type',
      choices=sorted(map(lambda x: x.lower(), EDeviceType.__members__.keys())),
      required=required,
      help='Type of device to find or use.')
  return parser


def make_device_number_parser(
    required: bool = False,
    required_device_type: Optional[EDeviceType] = None
) -> argparse.ArgumentParser:
  """Returns a parser for a --device_number CLI argument."""
  parts = []
  if required_device_type:
    parts.append(required_device_type.name)
    parts.append(' device')
  else:
    parts.append('Device ')
  parts.append('number to find or use.')
  parser = argparse.ArgumentParser(add_help=False)
  parser.add_argument(
      '--device_number', type=int, required=required, help=''.join(parts))
  return parser


def make_device_limits_parser(
    min_required_devices=None,
    max_allowed_devices=None) -> argparse.ArgumentParser:
  """Returns an arg parser for the number devices."""
  parser = argparse.ArgumentParser(add_help=False)
  parser.add_argument(
      '--min_required_devices',
      metavar='MIN',
      type=int,
      default=min_required_devices,
      help='Minimum number of devices that must be found')
  parser.add_argument(
      '--max_allowed_devices',
      metavar='MAX',
      type=int,
      default=max_allowed_devices,
      help='Maximum number of devices allowed to be found.')
  return parser


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


def main() -> None:
  parser = argparse.ArgumentParser(
      description='Describe Alpaca Servers.',
      parents=[
          alpaca_discovery.make_discovery_parser(),
          make_url_base_parser(),
          make_device_number_parser(),
          make_device_type_parser(),
      ])
  cli_args = parser.parse_args()
  cli_kwargs = vars(cli_args)
  clients: List[AlpacaHttpClient] = find_servers(**cli_kwargs)

  print(f'Found {len(clients)} Alpaca server{"" if len(clients) == 1 else "s"}')

  for client in clients:
    print()
    print(f'Alpaca Server {client.url_base} supports API versions: '
          f'{client.apiversions()!r}')
    desc = client.description()
    print('Using API v1, server description:')
    print(f'        Name: {desc["ServerName"]}')
    print(f'Manufacturer: {desc["Manufacturer"]}')
    print(f'     Version: {desc["ManufacturerVersion"]}')
    print(f'    Location: {desc["Location"]}')
    print()

    configured_devices = client.configured_devices()
    print(f'Server has {len(configured_devices)} configured '
          f'devices{"" if len(configured_devices) == 1 else "s"}')
    for cd in configured_devices:
      print()
      print(f'  Device #: {cd.device_number}')
      print(f'      Type: {cd.device_type.name}')
      print(f'      Name: {cd.device_name}')
      print(f'  UniqueID: {cd.unique_id}', flush=True)


if __name__ == '__main__':
  main()
