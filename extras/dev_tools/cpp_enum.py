#!/usr/bin/env python3
"""Extract the enumeration definitions from a CppSource instance."""

import dataclasses
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
  name: str
  print_name: Optional[str]
  value: List[TokenOrGroup]  # Empty value of enumerator is unspecified.


@dataclasses.dataclass()
class EnumerationDefinition:
  name: str
  is_scoped: bool
  underlying_type: Optional[str]
  enumerators: List[EnumeratorDefinition]


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


def process_file(file_path: str):
  """Reads a file and prints its enum definitions."""
  print()
  print('#' * 80)
  print('Finding nested C++ tokens in file', file_path, flush=True)
  cpp_source = tokenize_cpp.CppSource(file_path=file_path)
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
        print(tokenize_cpp.stringify_token_groups(enumerator.value), end='')
      print(',')
    print('};')


def main(argv: List[str]):
  for arg in argv[1:]:
    process_file(arg)


if __name__ == '__main__':
  main(sys.argv[:])
