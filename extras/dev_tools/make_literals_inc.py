#!/usr/bin/env python3
"""Update literals.inc with literals defined in other files.

And determine which literals are actually in use, flag for removal.

NOT YET IMPLEMENTED AS DESCRIBED ABOVE.
"""

import sys
import typing
from typing import Generator, List, Union

import tokenize_cpp

EToken = tokenize_cpp.EToken
Group = tokenize_cpp.Group
Replacement = tokenize_cpp.Replacement
Token = tokenize_cpp.Token
TokenOrGroup = Union[Token, Group]


def process_source_file(file_path: str) -> None:
  """Reads a C++ source file and prints its logging statements."""
  print()
  print('#' * 80)
  print('Finding TAS logging statements in file', file_path, flush=True)
  print()

  all_replacements = []
  cpp_source = tokenize_cpp.CppSource(file_path=file_path)
  # for lst in find_tas_stream_statements(cpp_source.grouped_cpp_tokens):
  #   replacements = list(generate_add_flashstr(lst))
  #   if not replacements:
  #     continue
  #   tokenize_cpp.dump_grouped_tokens(lst)
  #   print()
  #   print(tokenize_cpp.stringify_token_groups(lst))
  #   print()
  #   print(replacements)
  #   all_replacements.extend(replacements)
  #   print(flush=True)
  # if all_replacements:
  #   print(f'Applying {len(all_replacements)} replacements')
  #   cpp_source.edit_raw_source(all_replacements)
  #   cpp_source.update_file()


def main(argv: List[str]):
  for arg in argv[1:]:
    process_source_file(arg)


if __name__ == '__main__':
  main(sys.argv[:])
  sys.stdout.flush()
