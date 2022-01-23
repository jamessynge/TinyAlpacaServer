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
import sys
from typing import List, Optional, Tuple

import cpp_enum
import file_edit
import tokenize_cpp

EToken = tokenize_cpp.EToken


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
