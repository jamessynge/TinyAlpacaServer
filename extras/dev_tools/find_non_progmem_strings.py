#!/usr/bin/env python3
"""Find string literals that don't appear to be stored in PROGMEM."""

import sys
import typing
from typing import Generator, List, Tuple, Union

import tokenize_cpp

EToken = tokenize_cpp.EToken
Group = tokenize_cpp.Group
Replacement = tokenize_cpp.Replacement
Token = tokenize_cpp.Token
TokenOrGroup = Union[Token, Group]


def is_progmem_id(identifier: str) -> bool:
  if identifier.startswith('TAS_DEFINE_') and '_LITERAL' in identifier:
    return True
  if identifier in set(['TASLIT', 'FLASHSTR', 'TASENUMERATOR']):
    return True
  return False


def is_string_token(v) -> bool:
  return isinstance(v, Token) and v.kind in [EToken.STRING, EToken.RAW_STRING]


def find_id_parens_group(
    grouped_tokens: List[TokenOrGroup]
) -> Generator[Tuple[Token, Group], None, None]:
  """Yields an IDENTIFIER token followed by a parenthetic Group.

  The search continues inside parenthetic Groups, so later results may be inside
  an earlier result.

  Args:
    grouped_tokens: Tokens to search for IDENTIFIER(tokens...).
  """

  for ndx, elem in enumerate(grouped_tokens):
    if not isinstance(elem, Group):
      continue
    elem = typing.cast(Group, elem)
    if ndx > 0 and elem.start_token.kind == EToken.LEFT_PARENTHESIS:
      # Is the preceding element an IDENTIFIER?
      prior = grouped_tokens[ndx - 1]
      if isinstance(prior, Token) and prior.kind == EToken.IDENTIFIER:
        # Yes!
        prior = typing.cast(Token, prior)
        yield (prior, elem)
    yield from find_id_parens_group(elem.nested)


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
  print('Finding ID(...) in source file', file_path, flush=True)
  print()

  cpp_source = tokenize_cpp.CppSource(file_path=file_path)
  for id_token, group in find_id_parens_group(cpp_source.grouped_cpp_tokens):
    if is_progmem_id(id_token.src) or id_token.src == 'static_assert':
      continue
    if not any([is_string_token(elem) for elem in group.nested]):
      continue
    lst = [id_token, group]
    tokenize_cpp.dump_grouped_tokens(lst)
    print()
    print(tokenize_cpp.stringify_token_groups(lst))
    print(flush=True)


def main(argv: List[str]):
  for arg in argv[1:]:
    process_source_file(arg)


if __name__ == '__main__':
  main(sys.argv[:])
  sys.stdout.flush()
