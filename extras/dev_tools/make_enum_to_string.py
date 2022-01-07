#!/usr/bin/env python3
"""Generates helpers for C++ enums, and tests of those helpers.

The input is a single C++ source file (e.g. a .h or .cc file) with one or more
enum definitions. We assume that the enums are not generated using macros.

Why test generated code? Because someone may modify the enum declaration, which
compiling the generated may not detect.
"""

import re
import sys
from typing import Any, Callable, Dict, Generator, List, Optional, Tuple, Union

import tokenize_cpp

EToken = tokenize_cpp.EToken


def generate_enum_definitions(
    file_path: str) -> Generator[Dict[str, Any], None, None]:
  """Yields the definitions of enums found in the file."""

  # Recursive type hints aren't supported by pytype yet, so I've disabled the
  # errors it reports as of June 2021.
  TokenHandler = Callable[[str, EToken], Optional['TokenHandler']]  # pytype: disable=not-supported-yet,name-error

  current_enum_info: Optional[Dict[str, Any]] = None

  current_enumerator: str = ''

  def find_enum_start(s: str, token: EToken) -> Optional['TokenHandler']:
    nonlocal current_enum_info
    current_enum_info = None
    nonlocal current_enumerator
    current_enumerator = ''
    if token == EToken.IDENTIFIER and s == 'enum':
      return find_name

    return None

  def find_name(s: str, token: EToken) -> Optional['TokenHandler']:
    nonlocal current_enum_info
    assert current_enum_info is None
    if token == EToken.IDENTIFIER:
      if s == 'class':
        return find_name
      # print('Found start of definition of enum', s)
      current_enum_info = dict(name=s, enumerators=[])
      return find_enumerator_list_start

    print('Expected the name of the enum type, not {s!r} ({token})')
    return None

  def find_enumerator_list_start(s: str,
                                 token: EToken) -> Optional['TokenHandler']:
    nonlocal current_enum_info
    assert current_enum_info is not None
    if token == EToken.LEFT_BRACE:
      return find_enumerator_name

    if token in [EToken.COLON, EToken.IDENTIFIER, EToken.OP_SCOPE_RESOLUTION]:
      return find_enumerator_list_start

    print(f'Expected start of enumeration list, not {s!r} ({token})')
    return find_enum_start

  def find_enumerator_name(s: str, token: EToken) -> Optional['TokenHandler']:
    # print(f'find_enumerator_name({s!r}, {token})')
    nonlocal current_enum_info
    assert isinstance(current_enum_info, dict)
    if token == EToken.IDENTIFIER:
      if s == 'TASENUMERATOR':
        return match_tasenum_lparen

      # print('Found enumerator', s)
      current_enum_info['enumerators'].append(s)
      return find_enumerator_separator

    if token == EToken.RIGHT_BRACE:
      # Woot, reached the end of the list of names.
      return None

    print(f'Expected enumerator name, not a {token}: {s!r}')
    return find_enum_start

  def match_tasenum_lparen(_: str, token: EToken) -> Optional['TokenHandler']:
    nonlocal current_enum_info
    assert isinstance(current_enum_info, dict)
    if token == EToken.LEFT_PARENTHESIS:
      return match_tasenum_id
    print('Expected left paren, not', token)
    return find_enum_start

  def match_tasenum_id(s: str, token: EToken) -> Optional['TokenHandler']:
    nonlocal current_enum_info
    assert isinstance(current_enum_info, dict)
    nonlocal current_enumerator
    assert not current_enumerator
    if token == EToken.IDENTIFIER:
      # print('Found enumerator id', s)
      current_enumerator = s
      return match_tasenum_comma
    print('Expected identifier in TASENUMERATOR, not', token)
    return find_enum_start

  def match_tasenum_comma(_: str, token: EToken) -> Optional['TokenHandler']:
    nonlocal current_enum_info
    assert isinstance(current_enum_info, dict)
    nonlocal current_enumerator
    assert current_enumerator
    if token == EToken.OP_COMMA:
      return match_tasenum_string
    print('Expected comma in TASENUMERATOR, not', token)
    return find_enum_start

  def match_tasenum_string(s: str, token: EToken) -> Optional['TokenHandler']:
    nonlocal current_enum_info
    assert isinstance(current_enum_info, dict)
    nonlocal current_enumerator
    assert current_enumerator
    if token == EToken.STRING:
      current_enum_info['enumerators'].append((current_enumerator, s))
      current_enumerator = ''
      return match_tasenum_rparen
    print('Expected identifier in TASENUMERATOR, not', token)
    return find_enum_start

  def match_tasenum_rparen(_: str, token: EToken) -> Optional['TokenHandler']:
    nonlocal current_enum_info
    assert isinstance(current_enum_info, dict)
    if token == EToken.RIGHT_PARENTHESIS:
      return find_enumerator_separator
    print('Expected right paren, not', token)
    return find_enum_start

  def find_enumerator_separator(_: str,
                                token: EToken) -> Optional['TokenHandler']:
    nonlocal current_enum_info
    assert current_enum_info is not None
    if token == EToken.OP_COMMA:
      return find_enumerator_name

    if token == EToken.RIGHT_BRACE:
      # Woot, reached the end of the list of names.
      return None
    # Can have pretty much anything in the value of a constexpr, so skip until
    # we get one of the above.... which risks getting the wrong nested level.
    return find_enumerator_separator

  handler: TokenHandler = find_enum_start

  for token in tokenize_cpp.generate_cpp_tokens(file_path=file_path):
    # print(f'{handler.__name__}({s!r}, {token})')

    # Recursive type hints aren't supported by pytype yet, so I've disabled the
    # errors it reports as of June 2021.
    handler = handler(token.src, token.kind)  # pytype: disable=annotation-type-mismatch
    if handler:
      continue
    if current_enum_info and current_enum_info['enumerators']:
      yield current_enum_info
      current_enum_info = None
    handler = find_enum_start


def process_file(file_path: str):
  """Reads a file, finds enum definitions, emits print functions."""
  enum_definitions = list(generate_enum_definitions(file_path))
  if not enum_definitions:
    return

  print(
      'Generating functions for enum definitions in file',
      file_path,
      file=sys.stderr)

  # For enumerator identifiers that start with lower-case k followed by a
  # capital letter (e.g. kName or kFooBar), we strip the leading k from the
  # string that we print.
  def make_id_and_str(v: Union[str, Tuple[str, str]]) -> Tuple[str, str]:
    if isinstance(v, tuple):
      return v
    identifier: str = v
    if re.match('k[A-Z]', identifier):
      return (identifier, f'"{identifier[1:]}"')
    return (identifier, f'"{identifier}"')

  def fix_enumerators(enum_def: Dict[str, Any]) -> None:
    enum_def['enumerators'] = [
        make_id_and_str(v) for v in enum_def['enumerators']
    ]

  for enum_def in enum_definitions:
    fix_enumerators(enum_def)

  ##############################################################################

  print('namespace alpaca {')
  print()
  for enum_def in enum_definitions:
    name = enum_def['name']
    print(f'size_t PrintValueTo({name} v, Print& out);')
  print()
  for enum_def in enum_definitions:
    name = enum_def['name']
    print(f'const __FlashStringHelper* ToFlashStringHelper({name} v);')
  print()
  print('#if MCU_HOST_TARGET')
  print('// Support for debug logging of enums.')
  for enum_def in enum_definitions:
    name = enum_def['name']
    print(f'std::ostream& operator<<(std::ostream& os, {name} v);')
  print('#endif  // MCU_HOST_TARGET')
  print()
  print('}  // namespace alpaca')
  print()
  print()

  print('namespace alpaca {')
  for enum_def in enum_definitions:
    name = enum_def['name']
    print(f"""
size_t PrintValueTo({name} v, Print& out) {{
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {{
    return out.print(flash_string);
  }}
  return mcucore::PrintUnknownEnumValueTo(
        MCU_FLASHSTR("{name}"), static_cast<uint32_t>(v), out);
}}""")
  print()
  print()

  for enum_def in enum_definitions:
    name = enum_def['name']
    enumerators = enum_def['enumerators']
    print(
        f"""
const __FlashStringHelper* ToFlashStringHelper({name} v) {{
  switch (v) {{""",
        end='')
    for enumerator, dq_string in enumerators:
      print(
          f"""
    case {name}::{enumerator}:
      return MCU_FLASHSTR({dq_string});""",
          end='')
    print(r"""
  }
  return nullptr;
}""")
  print()

  print()
  print('#if MCU_HOST_TARGET')
  print('// Support for debug logging of enums.')

  for enum_def in enum_definitions:
    name = enum_def['name']
    enumerators = enum_def['enumerators']
    print(
        f"""
std::ostream& operator<<(std::ostream& os, {name} v) {{
  switch (v) {{""",
        end='')
    for enumerator, dq_string in enumerators:
      print(
          f"""
    case {name}::{enumerator}:
      return os << {dq_string};""",
          end='')
    print(f"""
  }}
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
}}""")

  print('#endif  // MCU_HOST_TARGET')
  print()
  print('}  // namespace alpaca')


def main(argv: List[str]):
  for arg in argv[1:]:
    process_file(arg)


if __name__ == '__main__':
  main(sys.argv[:])
