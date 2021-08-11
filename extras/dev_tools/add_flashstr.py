#!/usr/bin/env python3
"""Convert bare strings in logging statements into FLASHSTR macro invocations."""

import sys
import typing
from typing import Generator, List, Union

import tokenize_cpp

EToken = tokenize_cpp.EToken
Group = tokenize_cpp.Group
Replacement = tokenize_cpp.Replacement
Token = tokenize_cpp.Token
TokenOrGroup = Union[Token, Group]


def is_logging_keyword(identifier: str) -> bool:
  if not identifier.startswith('TAS_'):
    return False
  for prefix in ['TAS_VLOG', 'TAS_CHECK', 'TAS_DCHECK']:
    if identifier.startswith(prefix):
      return True
  return False


def find_tas_stream_statements(
    grouped_tokens: List[TokenOrGroup]
) -> Generator[List[TokenOrGroup], None, None]:
  """Yields statements starting with TAS_VLOG, TAS_CHECK or TAS_DCHECK."""

  ndx_limit = len(grouped_tokens)

  def find_next_semicolon(start_ndx: int) -> int:
    for ndx in range(start_ndx, ndx_limit):
      elem = grouped_tokens[ndx]
      if isinstance(elem, Token) and elem.kind == EToken.SEMICOLON:
        return ndx
    raise AssertionError(f'Expected to find semicolon starting at {start_ndx}')

  ndx = 0
  while ndx < ndx_limit:
    elem = grouped_tokens[ndx]
    if (isinstance(elem, Token) and elem.kind == EToken.IDENTIFIER and
        is_logging_keyword(elem.src)):
      # Found the start.
      beyond_ndx = find_next_semicolon(ndx + 1) + 1
      yield grouped_tokens[ndx:beyond_ndx]
      ndx = beyond_ndx
      continue
    if isinstance(elem, Group) and elem.start_token.kind == EToken.LEFT_BRACE:
      yield from find_tas_stream_statements(elem.nested)
    ndx += 1


def generate_add_flashstr(
    grouped_tokens: List[TokenOrGroup]) -> Generator[Replacement, None, None]:
  """Yields Replacements which add FLASHSTR(x) around each string literal x."""
  for elem in grouped_tokens:
    if not isinstance(elem, Token):
      continue
    token = typing.cast(Token, elem)
    if token.kind not in (EToken.STRING, EToken.RAW_STRING):
      continue
    yield Replacement.before(token, 'FLASHSTR(')
    yield Replacement.after(token, ')')


def process_source_file(file_path: str) -> None:
  """Reads a C++ source file and prints its logging statements."""
  print()
  print('#' * 80)
  print('Finding TAS logging statements in file', file_path, flush=True)
  print()

  all_replacements = []
  cpp_source = tokenize_cpp.CppSource(file_path=file_path)
  for lst in find_tas_stream_statements(cpp_source.grouped_cpp_tokens):
    replacements = list(generate_add_flashstr(lst))
    if not replacements:
      continue
    tokenize_cpp.dump_grouped_tokens(lst)
    print()
    print(tokenize_cpp.stringify_token_groups(lst))
    print()
    print(replacements)
    all_replacements.extend(replacements)
    print(flush=True)
  if all_replacements:
    print(f'Applying {len(all_replacements)} replacements')
    cpp_source.edit_raw_source(all_replacements)
    cpp_source.update_file()


def main(argv: List[str]):
  for arg in argv[1:]:
    process_source_file(arg)


if __name__ == '__main__':
  main(sys.argv[:])
  sys.stdout.flush()
