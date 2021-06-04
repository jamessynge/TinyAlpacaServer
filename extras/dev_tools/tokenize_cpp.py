#!/usr/bin/env python3
"""Split C++ source file into tokens.

Why? In support of tools that read a source file and extract info, such as enum
definitions.

It would probably be better to build on tools like clang, but that requires
installing a lot of tooling.
"""

import enum
import re
import sys
from typing import Generator, Match, Optional, Sequence, Tuple


def find_char_end(file_src: str, start_pos: int) -> int:
  """Returns the pos just beyond the end of the char literal at start_pos."""
  # print('find_char_end start_pos:', start_pos)
  pos = start_pos + 1
  regexp = re.compile(r"'|\\")
  while pos < len(file_src):
    m = regexp.search(file_src, pos=pos)
    if not m:
      break
    if file_src[m.start()] != '\\':
      return m.end()
    # Found a backslash in the char. Skip the character after the backslash.
    pos = m.end() + 1
  raise AssertionError(f'Unterminated character starting at {start_pos}')


def find_char_literal_end(file_src: str, m: Match[str]) -> int:
  return find_char_end(file_src, m.end() - 1)


def find_string_end(file_src: str, start_pos: int) -> int:
  """Returns the pos just beyond the end of the string literal at start_pos."""
  pos = start_pos + 1
  regexp = re.compile(r'"|\\')
  while pos < len(file_src):
    m = regexp.search(file_src, pos=pos)
    if not m:
      break
    if file_src[m.start()] != '\\':
      return m.end()
    # Found a backslash in the string. Skip the character after the backslash.
    pos = m.end() + 1
  raise AssertionError(f'Unterminated string starting at {start_pos}')


def find_string_literal_end(file_src: str, m: Match[str]) -> int:
  """Returns the pos just beyond the end of the string that starts with m."""
  if not m.group(0).endswith('R"'):
    # Not a string literal, so relatively simple.
    return find_string_end(file_src, m.end() - 1)

  # We've matched the start of a Raw String Literal. Determine the delimiter,
  # then search for the end of the string.
  regexp = re.compile(r'[^()\\ \f\n\r\t\v]{0,16}\(')
  m2 = regexp.match(file_src, pos=m.end())
  if not m2:
    raise AssertionError(
        'Unable to locate opening delimiter of the Raw String Literal '
        f'starting at {m.start()}: {file_src[m.start():m.start()+32]!r}')

  needle = ')' + m2.group()[0:-1] + '"'
  pos1 = file_src.find(needle, m2.end())
  if pos1 < 0:
    raise AssertionError(
        'Unable to locate closing delimiter of the Raw String Literal '
        f'starting at {m.start()}: {file_src[m.start():m.start()+32]!r}')

  pos2 = pos1 + len(needle)
  return pos2


class EToken(enum.Enum):
  """Types of tokens found by generate_cpp_tokens."""
  CHAR = enum.auto()
  COLON = enum.auto()
  IDENTIFIER = enum.auto()
  LEFT_BRACE = enum.auto()
  LEFT_BRACKET = enum.auto()
  LEFT_PARENTHESIS = enum.auto()
  NUMBER = enum.auto()
  OP_AND = enum.auto()
  OP_ASSIGN = enum.auto()
  OP_BITAND = enum.auto()
  OP_BITAND_ASSIGN = enum.auto()
  OP_BITNOT = enum.auto()
  OP_BITNOT_ASSIGN = enum.auto()
  OP_BITOR = enum.auto()
  OP_BITOR_ASSIGN = enum.auto()
  OP_BITXOR = enum.auto()
  OP_BITXOR_ASSIGN = enum.auto()
  OP_COMMA = enum.auto()
  OP_DECREMENT = enum.auto()
  OP_DIVIDE = enum.auto()
  OP_DIVIDE_ASSIGN = enum.auto()
  OP_EQUAL = enum.auto()
  OP_GREATER_THAN = enum.auto()
  OP_GREATER_THAN_EQUAL = enum.auto()
  OP_INCREMENT = enum.auto()
  OP_LESS_THAN = enum.auto()
  OP_LESS_THAN_EQUAL = enum.auto()
  OP_MINUS = enum.auto()
  OP_MINUS_ASSIGN = enum.auto()
  OP_MODULO = enum.auto()
  OP_MODULO_ASSIGN = enum.auto()
  OP_NOT = enum.auto()
  OP_NOT_EQUAL = enum.auto()
  OP_OR = enum.auto()
  OP_PERIOD = enum.auto()
  OP_PLUS = enum.auto()
  OP_PLUS_ASSIGN = enum.auto()
  OP_POINTER_TO_MEMBER = enum.auto()
  OP_SCOPE_RESOLUTION = enum.auto()
  OP_SHIFT_LEFT = enum.auto()
  OP_SHIFT_LEFT_ASSIGN = enum.auto()
  OP_SHIFT_RIGHT = enum.auto()
  OP_SHIFT_RIGHT_ASSIGN = enum.auto()
  OP_TERNARY = enum.auto()
  OP_TIMES = enum.auto()
  OP_TIMES_ASSIGN = enum.auto()
  OP_VARIADIC = enum.auto()
  RIGHT_BRACE = enum.auto()
  RIGHT_BRACKET = enum.auto()
  RIGHT_PARENTHESIS = enum.auto()
  SEMICOLON = enum.auto()
  STRING = enum.auto()


Token = Tuple[EToken, str]

OPS_AND_PUNC = sorted(
    [
        ('>>=', EToken.OP_SHIFT_RIGHT_ASSIGN),
        ('<<=', EToken.OP_SHIFT_LEFT_ASSIGN),
        ('->*', EToken.OP_POINTER_TO_MEMBER),
        ('...', EToken.OP_VARIADIC),
        ('::', EToken.OP_SCOPE_RESOLUTION),
        ('==', EToken.OP_EQUAL),
        ('!=', EToken.OP_NOT_EQUAL),
        ('>=', EToken.OP_GREATER_THAN_EQUAL),
        ('<=', EToken.OP_LESS_THAN_EQUAL),
        ('>>', EToken.OP_SHIFT_RIGHT),
        ('<<', EToken.OP_SHIFT_LEFT),
        ('+=', EToken.OP_PLUS_ASSIGN),
        ('-=', EToken.OP_MINUS_ASSIGN),
        ('/=', EToken.OP_DIVIDE_ASSIGN),
        ('*=', EToken.OP_TIMES_ASSIGN),
        ('%=', EToken.OP_MODULO_ASSIGN),
        ('|=', EToken.OP_BITOR_ASSIGN),
        ('&=', EToken.OP_BITAND_ASSIGN),
        ('~=', EToken.OP_BITNOT_ASSIGN),
        ('^=', EToken.OP_BITXOR_ASSIGN),
        ('++', EToken.OP_INCREMENT),
        ('--', EToken.OP_DECREMENT),
        ('&&', EToken.OP_AND),
        ('||', EToken.OP_OR),
        ('+', EToken.OP_PLUS),
        ('-', EToken.OP_MINUS),
        ('/', EToken.OP_DIVIDE),
        ('*', EToken.OP_TIMES),
        ('%', EToken.OP_MODULO),
        ('|', EToken.OP_BITOR),
        ('&', EToken.OP_BITAND),
        ('~', EToken.OP_BITNOT),
        ('^', EToken.OP_BITXOR),
        ('!', EToken.OP_NOT),
        ('>', EToken.OP_GREATER_THAN),
        ('<', EToken.OP_LESS_THAN),
        ('=', EToken.OP_ASSIGN),
        ('.', EToken.OP_PERIOD),
        (',', EToken.OP_COMMA),
        ('?', EToken.OP_TERNARY),
        (':', EToken.COLON),  # Maybe an operator, maybe not.
        (';', EToken.SEMICOLON),
        ('[', EToken.LEFT_BRACKET),
        (']', EToken.RIGHT_BRACKET),
        ('(', EToken.LEFT_PARENTHESIS),
        (')', EToken.RIGHT_PARENTHESIS),
        ('{', EToken.LEFT_BRACE),
        ('}', EToken.RIGHT_BRACE),
    ],
    key=lambda x: len(x[0]),
    reverse=True)

OPS_AND_PUNC_CHAR_SET = set([v[0][0] for v in OPS_AND_PUNC])


def match_operator_or_punctuation(file_src: str, pos: int) -> Optional[Token]:
  for s, e in OPS_AND_PUNC:
    token = file_src[pos:pos + len(s)]
    if s == token:
      return (e, s)
  return None


def generate_cpp_tokens(file_src: str = '',
                        file_path: str = '') -> Generator[Token, None, None]:
  """Yields relevant C++ tokens, not every valid/required C++ token."""
  if not file_src:
    with open(file_path, mode='r') as f:
      file_src: str = f.read()

  # print('len(file_src):', len(file_src))

  # Phase 2 of C++ translation: combine lines separated by a backslash newline.
  file_src = file_src.replace('\\\n', '')

  # print('len(file_src) after removing backslash lines:', len(file_src))

  whitespace_re = re.compile(r'\s+', flags=re.MULTILINE)
  # Simplistic attribute matcher.
  attribute_re = re.compile(r'\[\[.*\]\]', flags=re.MULTILINE)

  preprocessor_re = re.compile(r'^#.*', flags=re.MULTILINE)

  fp_re = re.compile(r'[-+]?(\d+(\.\d*)?|\.\d+)([eE][-+]?\d+)?')

  identifier_re = re.compile(r'[a-zA-Z_][a-zA-Z_0-9]*')

  line_comment_re = re.compile(r'//.*$', flags=re.MULTILINE)

  other_comment_re = re.compile(r'/\*.*?\*/', flags=re.DOTALL)

  start_char_literal_re = re.compile(r"(u8|u|U|L)?'")

  start_string_literal_re = re.compile(r'(u8|u|U|L)?R?"')

  # Maybe useful.
  # integer_re = re.compile(r'[-+]?(0[xX][\dA-Fa-f]+|0[0-7]*|\d+)')
  # rest_of_line_re = re.compile(r'.*$', flags=re.MULTILINE)

  pos = 0
  while pos < len(file_src):
    # print('pos=', pos)

    # Skip whitespace.
    m = whitespace_re.match(file_src, pos=pos)
    if m:
      # print(f'whitespace_re matched [{pos}:{m.end()}]')
      pos = m.end()
      continue

    # Skip attribute (e.g. [[noreturn]]).
    m = attribute_re.match(file_src, pos=pos)
    if m:
      # print(f'attribute_re matched [{pos}:{m.end()}]')
      pos = m.end()
      continue

    # Skip preprocessor lines.
    m = preprocessor_re.match(file_src, pos=pos)
    if m:
      # print(f'preprocessor_re matched [{pos}:{m.end()}]')
      pos = m.end()
      continue

    # Skip comments.
    m = line_comment_re.match(file_src, pos=pos)
    if m:
      # print(f'line_comment_re matched [{pos}:{m.end()}]')
      pos = m.end()
      continue
    m = other_comment_re.match(file_src, pos=pos)
    if m:
      # print(f'other_comment_re matched [{pos}:{m.end()}]')
      pos = m.end()
      continue

    # Match strings and characters.
    m = start_char_literal_re.match(file_src, pos=pos)
    if m:
      # print(f'start_char_literal_re matched [{pos}:{m.end()}]')
      end_pos = find_char_literal_end(file_src, m)
      token = file_src[pos:end_pos]
      yield (EToken.CHAR, token)
      pos = end_pos
      continue

    m = start_string_literal_re.match(file_src, pos=pos)
    if m:
      # print(f'start_string_literal_re matched [{pos}:{m.end()}]')
      end_pos = find_string_literal_end(file_src, m)
      token = file_src[pos:end_pos]
      yield (EToken.STRING, token)
      pos = end_pos
      continue

    #     if file_src[pos] == '"':
    #       end_pos = find_string_end(file_src, pos)
    #       token = file_src[pos:end_pos]
    #       yield (EToken.STRING, token)
    #       pos = end_pos
    #       continue

    #     if file_src[pos] == "'":
    #       end_pos = find_char_end(file_src, pos + 1)
    #       token = file_src[pos:end_pos]
    #       yield (EToken.CHAR, token)
    #       pos = end_pos
    #       continue

    # Match numbers.
    m = fp_re.match(file_src, pos=pos)
    if m:
      # print(f'fp_re matched [{pos}:{m.end()}]')
      token = file_src[pos:m.end()]
      yield (EToken.NUMBER, token)
      pos = m.end()
      continue

    # Match operators and other punctuation. Not attempting to make them
    # complete tokens (i.e. '->' will be yielded as '-' and '>').

    if file_src[pos] in OPS_AND_PUNC_CHAR_SET:
      # print(f'Found operator or punctuation at {pos}')
      v = match_operator_or_punctuation(file_src, pos)
      if v:
        yield v
        pos += len(v[1])
        continue
      # This is surprising!
      raise ValueError(f'Unable to match {file_src[pos:pos+20]!r}')

    # Match identifiers.
    m = identifier_re.match(file_src, pos=pos)
    if m:
      # print(f'identifier_re matched [{pos}:{m.end()}]')
      token = file_src[pos:m.end()]
      yield (EToken.IDENTIFIER, token)
      pos = m.end()
      continue

    raise ValueError(f'Unable to match {file_src[pos:pos+20]!r}')


def process_file(file_path: str):
  """Reads a file, finds enum definitions, emits print functions."""
  print()
  print('#' * 80)
  print('Tokenizing file', file_path, flush=True)
  for token, s in generate_cpp_tokens(file_path=file_path):
    print(f'{token}\t{s!r}')
  print()


def main(argv: Sequence[str]) -> None:
  for arg in argv[1:]:
    process_file(arg)


if __name__ == '__main__':
  main(sys.argv)
