#!/usr/bin/env python3
"""Extract the enumeration definitions from a CppSource instance."""

import dataclasses
import re
import sys
import typing
from typing import Callable, List, Optional, Tuple

import tokenize_cpp

EToken = tokenize_cpp.EToken
Group = tokenize_cpp.Group
Token = tokenize_cpp.Token
TokenOrGroup = tokenize_cpp.TokenOrGroup

TokenOrGroupMatcher = Callable[[TokenOrGroup], bool]


def items_match(items: List[TokenOrGroup],
                matchers: List[TokenOrGroupMatcher],
                offset=0) -> bool:
  if len(matchers) + offset > len(items):
    return False
  for item, matcher in zip(items[offset:], matchers):
    if not matcher(item):
      return False
  return True


def is_token_kind(item: TokenOrGroup, kind: EToken) -> bool:
  return isinstance(item, Token) and typing.cast(Token, item).kind == kind


def is_comma(item: TokenOrGroup) -> bool:
  return is_token_kind(item, EToken.OP_COMMA)


def is_string_literal(item: TokenOrGroup) -> bool:
  return is_token_kind(item, EToken.STRING)


def is_any_identifier(item: TokenOrGroup) -> bool:
  return is_token_kind(item, EToken.IDENTIFIER)


def is_group_kind(item: TokenOrGroup, start_kind: EToken) -> bool:
  if not isinstance(item, Group):
    return False
  return typing.cast(Group, item).start_token.kind == start_kind


def is_brace_group(item: TokenOrGroup) -> bool:
  return is_group_kind(item, EToken.LEFT_BRACE)


def is_parens_group(item: TokenOrGroup) -> bool:
  return is_group_kind(item, EToken.LEFT_PARENTHESIS)


def is_identifier(item: TokenOrGroup, name: str) -> bool:
  return is_any_identifier(item) and typing.cast(Token, item).src == name


def split_on_commas(items: List[TokenOrGroup]) -> List[List[TokenOrGroup]]:
  """Split a list of items on the comma tokens in among the items."""
  offset_limit = len(items)

  def find_comma_or_end(offset: int) -> int:
    while offset < offset_limit:
      if is_token_kind(items[offset], EToken.OP_COMMA):
        break
      offset += 1
    return offset

  result: List[List[TokenOrGroup]] = []
  start_offset = 0
  while start_offset < offset_limit:
    end_offset = find_comma_or_end(start_offset)
    result.append(items[start_offset:end_offset])
    start_offset = end_offset + 1

  return result


@dataclasses.dataclass()
class EnumeratorDefinition:
  """Holds the definition of one enumerator."""

  name: str
  print_name: Optional[str]
  value: List[TokenOrGroup]  # Empty value of enumerator is unspecified.
  numeric_value: Optional[int] = dataclasses.field(default=None)

  def get_dq_print_name(self) -> str:
    """Return print_name or a default value for it, double quoted."""
    if self.print_name:
      return self.print_name
    name = self.name
    if re.match('k[A-Z]', name):
      name = name[1:]
    return f'"{name}"'

  def init_numeric_value(self, enum_def: 'EnumerationDefinition',
                         prior: 'EnumeratorDefinition') -> bool:
    if self.numeric_value is None:
      self.numeric_value = self.compute_numeric_value(enum_def, prior)
    return self.numeric_value is not None

  def compute_numeric_value(
      self, enum_def: 'EnumerationDefinition',
      prior: Optional['EnumeratorDefinition']) -> Optional[int]:
    """Return the numeric value of this enum, if easily computable."""

    # Maybe use enum_def for finding the values of other enums when resolving
    # identifier names. For now, suppressing the fact that it is unused.
    del enum_def

    if not self.value:
      # No explicitly specified value. Can we compute it?
      if not prior:
        return 0
      elif prior.numeric_value is not None:
        return prior.numeric_value + 1
      else:
        return

    # This enumerator has an explicit value. Can we evaluate it?
    if tokenize_cpp.is_not_numeric_expression(self.value):
      # The expression may contain identifiers, etc. We don't currently have an
      # easy way to evaluate this, so we don't.
      return
    if len(self.value) != 1:
      return
    token = self.value[0]
    if not is_token_kind(token, EToken.INTEGER):
      return

    # Maybe straightforward.
    token = typing.cast(Token, token)
    s = token.raw_src
    if s.startswith('0x') and len(s) > 2:
      return int(s, base=0)
    elif s.startswith('0b') and len(s) > 2:
      return int(s, base=0)
    elif s.startswith('0'):
      return int(s, 8)
    else:
      return int(s, base=10)

  def stringify_value(self) -> Optional[str]:
    if not self.value:
      return None
    return tokenize_cpp.stringify_grouped_tokens(self.value)


@dataclasses.dataclass()
class EnumerationDefinition:
  """Holds the definition of one enum and its enumerators."""

  # TODO(jamessynge): Include support for attributes on the enum, in particular
  # the ability to indicate whether printing functions should be generated by
  # make_enum_to_string, or not generated, whichever makes more sense.

  name: str
  is_scoped: bool
  underlying_type: Optional[str]
  enumerators: List[EnumeratorDefinition]
  _all_values_known: bool = dataclasses.field(default=True)
  maximum_enumerator: Optional[EnumeratorDefinition] = dataclasses.field(
      default=None)
  minimum_enumerator: Optional[EnumeratorDefinition] = dataclasses.field(
      default=None)

  def __post_init__(self):
    """Determines the values of the enumerators, if relatively easy."""

    if not self.enumerators:
      return

    prior: Optional[EnumeratorDefinition] = None
    for enumerator in self.enumerators:
      if not enumerator.init_numeric_value(self, prior):
        self._all_values_known = False
      prior = enumerator

    if not self._all_values_known:
      return

    maximum_enumerator = self.enumerators[0]
    maximum_value = maximum_enumerator.numeric_value
    maximum_value: int

    minimum_enumerator = maximum_enumerator
    minimum_value: int = maximum_value

    for enumerator in self.enumerators[1:]:
      if enumerator.numeric_value > maximum_value:
        maximum_enumerator = enumerator
        maximum_value = enumerator.numeric_value
      elif enumerator.numeric_value < minimum_value:
        minimum_enumerator = enumerator
        minimum_value = enumerator.numeric_value
    self.maximum_enumerator = maximum_enumerator
    self.minimum_enumerator = minimum_enumerator

  def all_values_known(self) -> bool:
    return self._all_values_known

  def maximum_value(self) -> Optional[int]:
    if self.maximum_enumerator:
      return self.maximum_enumerator.numeric_value
    return

  def minimum_value(self) -> Optional[int]:
    if self.minimum_enumerator:
      return self.minimum_enumerator.numeric_value
    return


def parse_enumerations_list(
    enumerations_list: List[TokenOrGroup]) -> List[EnumeratorDefinition]:
  """Returns a list of EnumeratorDefinitions for an enum."""
  if not enumerations_list:
    raise ValueError('Enumeration definitions list is empty.')

  def is_tas_enumerator_identifier(item: TokenOrGroup) -> bool:
    return is_identifier(item, 'TASENUMERATOR')

  def parse_tas_enumerator(group: Group) -> Tuple[str, str]:
    if (len(group.nested) == 3 and items_match(
        group.nested, [is_any_identifier, is_comma, is_string_literal])):
      return (typing.cast(Token, group.nested[0]).src,
              typing.cast(Token, group.nested[2]).src)
    raise ValueError(
        'Expected TASENUMERATOR to be followed by (identifier, string), '
        f'not {group}')

  def parse_enumerator_definition(
      items: List[TokenOrGroup]) -> EnumeratorDefinition:
    name: str = ''
    print_name: Optional[str] = None
    if items_match(items, [is_tas_enumerator_identifier, is_parens_group]):
      name, print_name = parse_tas_enumerator(items[1])
      offset = 2
    elif not is_any_identifier(items[0]):
      raise ValueError(f'Expected an enumerator name, not: {items[0]}')
    else:
      name = typing.cast(Token, items[0]).src
      offset = 1

    if offset < len(items):
      if not is_token_kind(items[offset], EToken.OP_ASSIGN):
        raise ValueError(
            'Expected an equal sign following the enumerator name, not: '
            f'{items[offset:]}')
      offset += 1
      if offset >= len(items):
        raise ValueError('Expected an enumerator value, after the equal sign: '
                         f'{items[offset:]}')
    return EnumeratorDefinition(
        name=name, print_name=print_name, value=items[offset:])

  # Last enumerator definition may be followed by a comma. If it is, remove the
  # trailing empty list. No other list is allowed to be empty, including the
  # first.
  lists = split_on_commas(enumerations_list)
  if len(lists) > 1 and not lists[-1]:
    lists.pop()

  result: List[EnumeratorDefinition] = []
  for items in lists:
    if not items:
      raise ValueError('Expected non-empty list of tokens between commas.')
    result.append(parse_enumerator_definition(items))
  return result


def maybe_parse_enumeration_definition(
    items: List[TokenOrGroup],
    offset: int) -> Optional[Tuple[EnumerationDefinition, int]]:
  """Returns an EnumerationDefinition if items[offset:] starts with one."""
  assert offset < len(items)
  # Need at least 4 items to define an enumeration:
  #     enum, name, group, semicolon
  if offset + 4 > len(items):
    return None

  if not is_identifier(items[offset], 'enum'):
    return None
  offset += 1

  is_scoped: bool = False
  if (is_identifier(items[offset], 'class') or
      is_identifier(items[offset], 'struct')):
    # Note that we don't detect whether or not an enum is defined inside of a
    # class, even though that represents a scope as well.
    is_scoped = True
    offset += 1

  if not is_any_identifier(items[offset]):
    return None
  name: str = typing.cast(Token, items[offset]).src
  offset += 1

  try:
    underlying_type: Optional[str] = None
    if is_token_kind(items[offset], EToken.COLON):
      offset += 1
      if offset >= len(items):
        return None
      if len(items) <= offset or not is_any_identifier(items[offset]):
        raise ValueError(
            f'Expected the identifier of an underlying type for enum {name}')
      underlying_type = typing.cast(Token, items[offset]).src
      offset += 1
      if offset >= len(items):
        return None

    if not is_brace_group(items[offset]):
      return None

    enumerators = parse_enumerations_list(
        typing.cast(Group, items[offset]).nested)
    offset += 1
    if offset >= len(items):
      return None

    # Final token must be a semicolon.
    if not is_token_kind(items[offset], EToken.SEMICOLON):
      return None

    definition = EnumerationDefinition(
        name=name,
        is_scoped=is_scoped,
        underlying_type=underlying_type,
        enumerators=enumerators)
    return definition, offset
  except ValueError as ex:
    if ex.args:
      msg = f'{ex.args[0]}\nWhile parsing enum definition for {name}'
      ex.args = (msg,) + ex.args[1:]
    raise


def get_enumeration_definitions(
    source: tokenize_cpp.CppSource) -> List[EnumerationDefinition]:
  """Returns the enum definitions found in source."""
  result: List[EnumerationDefinition] = []

  def find_enumeration_definitions(items: List[TokenOrGroup]) -> None:
    # TODO(jamessynge): Track current namespace, use to annotate the enum
    # definition.
    offset = 0
    while offset < len(items):
      opt_def_and_offset = maybe_parse_enumeration_definition(items, offset)
      if opt_def_and_offset:
        definition: EnumerationDefinition = opt_def_and_offset[0]
        offset = opt_def_and_offset[1]
        result.append(definition)
        continue
      if is_brace_group(items[offset]):
        find_enumeration_definitions(typing.cast(Group, items[offset]).nested)
      offset += 1

  find_enumeration_definitions(source.grouped_cpp_tokens)
  return result


def process_file(file_path: str, file_src: str = ''):
  """Reads a file and prints its enum definitions."""
  print()
  print('#' * 80)
  print('Finding enum definitions in file', file_path, flush=True)
  cpp_source = tokenize_cpp.CppSource(file_path=file_path, raw_source=file_src)
  for enum_def in get_enumeration_definitions(cpp_source):
    print()
    print('enum', end='')
    if enum_def.is_scoped:
      print(' class', end='')
    print(f' {enum_def.name}', end='')
    if enum_def.underlying_type:
      print(f' : {enum_def.underlying_type}', end='')
    print(' {')
    for enumerator in enum_def.enumerators:
      print('  ', end='')
      if enumerator.print_name:
        print(
            f'TASENUMERATOR({enumerator.name}, {enumerator.print_name})',
            end='')
      else:
        print(enumerator.name, end='')
      if enumerator.value:
        print(' = ', end='')
        print(tokenize_cpp.stringify_grouped_tokens(enumerator.value), end='')
      print(',')
    print('};')


def main(argv: List[str]):
  for arg in argv[1:]:
    process_file(arg)


if __name__ == '__main__':
  main(sys.argv[:])
