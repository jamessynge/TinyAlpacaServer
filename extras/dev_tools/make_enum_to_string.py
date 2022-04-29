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
from typing import Dict, List, Optional, Tuple

import cpp_enum
import file_edit
import tokenize_cpp

EToken = tokenize_cpp.EToken
EnumerationDefinition = cpp_enum.EnumerationDefinition


def capture_stdout(func, *args, **kwargs) -> str:
  with contextlib.redirect_stdout(io.StringIO()) as f:
    func(*args, **kwargs)
  return f.getvalue()


def enumerator_dict_is_compact(
    n_to_enumerator: Dict[int, cpp_enum.EnumeratorDefinition]) -> bool:
  ns = sorted(list(n_to_enumerator.keys()))
  for a, b in zip(ns, ns[1:]):
    if a + 1 != b:
      return False
  return True


def type_is_unsigned(int_type: str) -> bool:
  if re.fullmatch(r'(?:std::)?uint(?:8|16|32|64)_t', int_type):
    return True
  elif re.fullmatch(r'unsigned(?:\s+(?:char|short|int|long(?:\s+long)?))?',
                    int_type):
    return True
  else:
    return False


def value_to_enum_type_and_name(enum_def: EnumerationDefinition,
                                value: int) -> str:
  n_to_enumerator = enum_def.get_numeric_value_to_enumerator()
  if value not in n_to_enumerator:
    raise ValueError(
        f'{value} is not the value of an enumerator for {enum_def.name}')
  return f'{enum_def.name}::{n_to_enumerator[value].name}'


def print_to_flash_string_via_table_body(enum_def: EnumerationDefinition,
                                         var_name: str) -> bool:
  """Print the body of a function for looking up a string via a table."""

  # Consider emitting a static assert that the mapping is still correct, and
  # maybe an unused function with a switch on the enum values which will fail to
  # compile (with certain warnings as errors) if the enumerators have changed.

  if not enum_def.all_values_known():
    return False

  int_type = enum_def.get_int_type()
  if not int_type:
    raise AssertionError('enum_def.get_int_type() did not return a type')

  min_max = enum_def.extreme_values()
  if not min_max:
    raise AssertionError('enum_def.extreme_values() did not return values')
  minimum, maximum = min_max

  n_to_enumerator = enum_def.get_numeric_value_to_enumerator()
  if not n_to_enumerator:
    raise AssertionError(
        'enum_def.get_numeric_value_to_enumerator() did not return a dict')
  if not enumerator_dict_is_compact(n_to_enumerator):
    return False

  items = sorted(n_to_enumerator.items())
  if items[0][0] != minimum:
    raise AssertionError(
        'minimum value does not match that of sorted enumerators: '
        f'{items[0][0]} != {minimum}')
  if items[-1][0] != maximum:
    raise AssertionError(
        'maximum value does not match that of sorted enumerators: '
        f'{items[-1][0]} != {maximum}')

  print('  static MCU_FLASH_STRING_TABLE( ')
  print('      flash_string_table,', end='')

  for nv, enumerator in items:
    print_name = enumerator.get_dq_print_name()
    print(
        f"""
      MCU_FLASHSTR({print_name}),  // {nv}: {enumerator.name}""",
        end='')

  print(
      f"""
  );
  return mcucore::LookupFlashStringForDenseEnum<{int_type}>(
      flash_string_table, """,
      end='')

  # Include the min and max if the underlying type isn't unsigned or the minimum
  # is non-zero.
  if not (minimum == 0 and type_is_unsigned(int_type)):
    print(f'{value_to_enum_type_and_name(enum_def, minimum)}, ')
    print(f'{value_to_enum_type_and_name(enum_def, maximum)}, ')

  print(f'{var_name});', end='')

  return True


def declare_to_flash_string_helper(enum_def: EnumerationDefinition) -> None:
  print(f'const __FlashStringHelper* ToFlashStringHelper({enum_def.name} v);')


def define_to_flash_string_helper(enum_def: EnumerationDefinition) -> None:
  """Print the definition of ToFlashStringHelper for an enum."""

  via_table_body = capture_stdout(print_to_flash_string_via_table_body,
                                  enum_def, 'v')

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
  return nullptr;""")

  if via_table_body:
    print('#elif defined(TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS)', end='')
  else:
    print('#else  // Use if statements.', end='')

  for enumerator in enum_def.enumerators:
    print(
        f"""
  if (v == {name}::{enumerator.name}) {{
    return MCU_FLASHSTR({enumerator.get_dq_print_name()});
  }}""",
        end='')
  print("""
  return nullptr;""", end='')

  if via_table_body:
    print(f"""
#else  // Use flash string table.
{via_table_body}""", end='')

  print(r"""
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
}""")


def declare_print_value_to(enum_def: EnumerationDefinition) -> None:
  print(f'size_t PrintValueTo({enum_def.name} v, Print& out);')


def define_print_value_to(enum_def: EnumerationDefinition) -> None:
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


def declare_stream_insert(enum_def: EnumerationDefinition) -> None:
  print(f'std::ostream& operator<<(std::ostream& os, {enum_def.name} v);')


def define_stream_insert(enum_def: EnumerationDefinition) -> None:
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
    enum_definitions: List[EnumerationDefinition]) -> None:
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
    enum_definitions: List[EnumerationDefinition]) -> None:
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
INCLUDES = """
#include <McuCore.h>
#include <McuCore.h>
#include <McuCore.h>
"""


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
  """Return the header file corresponding to `cpp_source`."""
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
  """Return the source file corresponding to `cpp_source`."""
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
  definitions = INCLUDES + definitions

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
