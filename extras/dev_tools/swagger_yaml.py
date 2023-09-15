#!/usr/bin/env python3
"""Simple tool to read Swagger YAML files (i.e. API definitions)."""

import dataclasses
import os
import sys
import typing
from typing import Any, Dict, List, Sequence

import requests

# Spec = Dict[str, Union['Spec', str, int, List['Spec']]]

Spec = typing.TypeVar('Spec')


def get_spec(
    file_name: str = 'AlpacaDeviceAPI_v1.yaml', refresh: bool = False
) -> str:
  """Reach Alpaca Device spec from local cache with fallback to URL fetch."""
  local_copy = os.path.join(os.path.expanduser('~'), file_name)
  if os.path.exists(local_copy) and not refresh:
    with open(local_copy, 'r') as f:
      text = f.read()
    print(f'Read spec from cache file {local_copy}')
  else:
    response = requests.get('https://www.ascom-standards.org/api/' + file_name)
    text = response.text
    with open(local_copy, 'w') as f:
      f.write(response.text)
    print(f'Cached spec in file {local_copy}')
  return text


def make_components_map(spec: Any) -> Dict[str, Any]:
  result: Dict[str, Any] = {}
  all_components = spec['components']
  for component_type, components_of_type in all_components.items():
    for component_name, component_def in components_of_type.items():
      ref = f'#/components/{component_type}/{component_name}'
      result[ref] = component_def
  return result


@dataclasses.dataclass()
class Schema:
  type_: str


@dataclasses.dataclass()
class Parameter:
  """Represents a named value in an HTTP request path or query string.

  Parameters in a request are positional, with that position specified by name
  in a path template; for example '{device_number}' in
  '/camera/{device_number}/action' indicates that the parameter named
  'device_number' appears at that position in the path.

  Parameters in the query string are identified by name and can appear in any
  order, with unknown parameters ignored by the server. Their format is
  name=value, with '&' as a separator between parameters.
  """

  name: str


class Property:
  """Represents a named value provided in an HTTP request or response body."""


class AlpacaSpec:
  """An Alpaca API spec."""

  def __init__(self, spec: Dict[str, Any]):
    self.spec = spec
    print('spec keys:', self.spec.keys())
    self.paths = spec['paths']
    self.components: Dict[str, Any] = make_components_map(spec)
    print('components keys:\n\t', end='')
    print('\n\t'.join(sorted(self.components.keys())))
    self.resolve_re = None

  def _resolve_value_refs(self, obj: object):
    pass

  def _resolve_elem_refs(self, lst: List[Any]):
    pass


def main(argv: Sequence[str]) -> None:
  if len(argv) > 1:
    raise UserWarning('Too many command-line arguments.')
  get_spec()
  get_spec(file_name='AlpacaManagementAPI_v1.yaml')


if __name__ == '__main__':
  main(sys.argv[1:])
