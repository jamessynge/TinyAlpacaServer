#!/usr/bin/env python3
"""Generates helpers for C++ enums, and tests of those helpers.

The input is a single C++ source file (e.g. a .h or .cc file) with one or more
enum definitions. We assume that the enums are not generated using macros.

Why test generated code? Because someone may modify the enum declaration, which
compiling the generated may not detect.

Note that support for updating files requires that the block of code generated
code is already in the appropriate file, and that the blocks are tagged as
printed by this file.
"""

import argparse
import contextlib
import io
import re
import sys
from typing import Any, Callable, Dict, Generator, List, Optional, Tuple, Union

import cpp_enum
import file_edit
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


def make_id_and_str(v: Union[str, Tuple[str, str]]) -> Tuple[str, str]:
  """Returns the identifier and string form of an enumerator."""

  # For enumerator identifiers that start with lower-case k followed by a
  # capital letter (e.g. kName or kFooBar), we strip the leading k from the
  # string that we print.
  if isinstance(v, tuple):
    return v
  identifier: str = v
  if re.match('k[A-Z]', identifier):
    return (identifier, f'"{identifier[1:]}"')
  return (identifier, f'"{identifier}"')


def declare_to_flash_string_helper(
    enum_def: cpp_enum.EnumerationDefinition) -> None:
  print(f'const __FlashStringHelper* ToFlashStringHelper({enum_def.name} v);')


def define_to_flash_string_helper(
    enum_def: cpp_enum.EnumerationDefinition) -> None:
  """Print the definition of ToFlashStringHelper for an enum."""

  name = enum_def.name
  print(
      f"""
const __FlashStringHelper* ToFlashStringHelper({name} v) {{
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {{""",
      end='')
  for enumerator in enum_def.enumerators:
    print(
        f"""
    case {name}::{enumerator.name}:
      return MCU_FLASHSTR({enumerator.get_dq_print_name()});""",
        end='')
  print(r"""
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH""", end='')
  for enumerator in enum_def.enumerators:
    print(
        f"""
  if (v == {name}::{enumerator.name}) {{
    return MCU_FLASHSTR({enumerator.get_dq_print_name()});
  }}""",
        end='')

  print(r"""
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}""")


def declare_print_value_to(enum_def: cpp_enum.EnumerationDefinition) -> None:
  print(f'size_t PrintValueTo({enum_def.name} v, Print& out);')


def define_print_value_to(enum_def: cpp_enum.EnumerationDefinition) -> None:
  """Print the definition of PrintValueTo for an enum."""
  name = enum_def.name
  print(f"""
size_t PrintValueTo({name} v, Print& out) {{
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {{
    return out.print(flash_string);
  }}
  return mcucore::PrintUnknownEnumValueTo(
        MCU_FLASHSTR("{name}"), static_cast<uint32_t>(v), out);
}}""")


def declare_stream_insert(enum_def: cpp_enum.EnumerationDefinition) -> None:
  print(f'std::ostream& operator<<(std::ostream& os, {enum_def.name} v);')


def define_stream_insert(enum_def: cpp_enum.EnumerationDefinition) -> None:
  """Print the definition of operator<< for an enum."""
  name = enum_def.name

  print(
      f"""
std::ostream& operator<<(std::ostream& os, {name} v) {{
  switch (v) {{""",
      end='')
  for enumerator in enum_def.enumerators:
    print(
        f"""
    case {name}::{enumerator.name}:
      return os << {enumerator.get_dq_print_name()};""",
        end='')
  print(f"""
  }}
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
}}""")


def print_function_declarations(
    enum_definitions: List[cpp_enum.EnumerationDefinition]) -> None:
  """Prints the enum print function declarations to stdout."""

  print()
  print('namespace alpaca {')
  print()

  for enum_def in enum_definitions:
    declare_to_flash_string_helper(enum_def)

  print()
  print()

  for enum_def in enum_definitions:
    declare_print_value_to(enum_def)

  print()
  print()

  print('#if MCU_HOST_TARGET')
  print('// Support for debug logging of enums.')
  for enum_def in enum_definitions:
    declare_stream_insert(enum_def)
  print('#endif  // MCU_HOST_TARGET')

  print()
  print('}  // namespace alpaca')


def print_function_definitions(
    enum_definitions: List[cpp_enum.EnumerationDefinition]) -> None:
  """Prints the enum print function definitions to stdout."""

  print()
  print('namespace alpaca {')
  print()

  for enum_def in enum_definitions:
    define_to_flash_string_helper(enum_def)
    continue

  print()
  print()

  for enum_def in enum_definitions:
    define_print_value_to(enum_def)

  print()
  print()

  print('#if MCU_HOST_TARGET')
  print('// Support for debug logging of enums.')

  for enum_def in enum_definitions:
    define_stream_insert(enum_def)

  print()
  print('#endif  // MCU_HOST_TARGET')
  print()
  print('}  // namespace alpaca')


def capture_stdout(func, *args, **kwargs) -> str:
  with contextlib.redirect_stdout(io.StringIO()) as f:
    func(*args, **kwargs)
  return f.getvalue()


def generate_decls_and_defns(
    cpp_source: tokenize_cpp.CppSource) -> Optional[Tuple[str, str]]:
  enum_definitions = cpp_enum.get_enumeration_definitions(cpp_source)
  if not enum_definitions:
    return

  declarations = capture_stdout(print_function_declarations, enum_definitions)
  definitions = capture_stdout(print_function_definitions, enum_definitions)

  return (declarations, definitions)


TAG_SUFFIX = 'GENERATED_BY_MAKE_ENUM_TO_STRING'
BEGIN_HEADER_TAG = '// BEGIN_HEADER_' + TAG_SUFFIX
END_HEADER_TAG = '// END_HEADER_' + TAG_SUFFIX
BEGIN_SOURCE_TAG = '// BEGIN_SOURCE_' + TAG_SUFFIX
END_SOURCE_TAG = '// END_SOURCE_' + TAG_SUFFIX


def update_header_file(file_contents: file_edit.FileContents,
                       declarations: str):
  return file_contents.replace_tagged_section(BEGIN_HEADER_TAG, END_HEADER_TAG,
                                              '\n' + declarations + '\n')


def update_source_file(file_contents: file_edit.FileContents,
                       declarations: str):
  return file_contents.replace_tagged_section(BEGIN_SOURCE_TAG, END_SOURCE_TAG,
                                              '\n' + declarations + '\n')


def get_header_file_contents(
    cpp_source: tokenize_cpp.CppSource) -> Optional[file_edit.FileContents]:
  if cpp_source.is_header_file:
    return cpp_source.file_contents
  paths = tokenize_cpp.find_header_paths(cpp_source.file_path)
  if len(paths) == 1:
    return file_edit.FileContents(paths[0])
  elif not paths:
    print('Unable to find a header file corresponding to', cpp_source.file_path)
  else:
    print('Unable to choose a single header file corresponding to',
          cpp_source.file_path)
    print('Candidates:', *paths, '', sep='\n')


def get_source_file_contents(
    cpp_source: tokenize_cpp.CppSource) -> Optional[file_edit.FileContents]:
  if cpp_source.is_source_file:
    return cpp_source.file_contents
  paths = tokenize_cpp.find_source_paths(cpp_source.file_path)
  if len(paths) == 1:
    return file_edit.FileContents(paths[0])
  elif not paths:
    print('Unable to find a source file corresponding to', cpp_source.file_path)
  else:
    print('Unable to choose a single source file corresponding to',
          cpp_source.file_path)
    print('Candidates:', *paths, '', sep='\n')


def print_tagged_header_section(declarations: str) -> None:
  print(BEGIN_HEADER_TAG)
  print(declarations)
  print(END_HEADER_TAG)


def print_tagged_source_section(definitions: str) -> None:
  print(BEGIN_SOURCE_TAG)
  print(definitions)
  print(END_SOURCE_TAG)


def process_file(file_path: str, update_files: bool = False):
  """Reads a file, finds enum definitions, emits print functions."""

  cpp_source = tokenize_cpp.CppSource(file_path=file_path)
  decls_and_defns = generate_decls_and_defns(cpp_source)
  if not decls_and_defns:
    return
  print(
      'Generated functions for enum definitions in file', file_path, flush=True)

  declarations, definitions = decls_and_defns

  # Attempt to update the files, if permitted. If unable, then we'll print them
  # out.

  header_file_contents: Optional[file_edit.FileContents] = None
  source_file_contents: Optional[file_edit.FileContents] = None
  if update_files:
    header_file_contents = get_header_file_contents(cpp_source)
    source_file_contents = get_source_file_contents(cpp_source)
    if header_file_contents:
      if update_header_file(header_file_contents, declarations):
        header_file_contents.update_file()
      else:
        header_file_contents = None
    if source_file_contents:
      if update_source_file(source_file_contents, definitions):
        source_file_contents.update_file()
      else:
        source_file_contents = None

  if not header_file_contents:
    print('-' * 80)
    print('Please add this content to the appropriate header file:')
    print('-' * 80)
    print()
    print_tagged_header_section(declarations)
    print()

  if not source_file_contents:
    print('-' * 80)
    print('Please add this content to the appropriate source file:')
    print('-' * 80)
    print()
    print_tagged_source_section(definitions)
    print()
  sys.stdout.flush()


def main(argv: List[str]):
  args = argv[1:]
  parser = argparse.ArgumentParser()
  parser.add_argument(
      '--update', action='store_true', help='Update generated sections.')
  parser.add_argument(
      'file_paths',
      metavar='FILE',
      type=str,
      nargs='+',
      help='C++ source or header path.')
  parsed_args = parser.parse_args(args)
  for file_path in parsed_args.file_paths:
    process_file(file_path, parsed_args.update)


if __name__ == '__main__':
  main(sys.argv[:])
