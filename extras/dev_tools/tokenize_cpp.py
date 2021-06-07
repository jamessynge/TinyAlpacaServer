#!/usr/bin/env python3
"""Split C++ source file into Tokens.

Why? In support of tools that read a source file and extract info, such as enum
definitions. While it would probably be better to build on tools like clang,
that requires installing a lot of tooling, while Python3 is commonly present and
doesn't require compiling the tool.

A Token instance tells us the kind of token found, the source text for the token
and the position (character index within the file).
"""

import enum
import re
import sys
from typing import Generator, List, Match, Optional, Sequence, Tuple

import dataclasses

# Regular expressions used to match various tokens, or parts of such tokens, in
# a C++ source file.

WHITESPACE_RE = re.compile(r'\s+', flags=re.MULTILINE)

# Simplistic attribute specifier matcher. Not sure that I need re.MULTILINE for
# any case I've had to deal with, and re.DOTALL might actually be what I meant!
ATTRIBUTE_RE = re.compile(r'\[\[.*\]\]', flags=re.MULTILINE)

PREPROCESSOR_RE = re.compile(r'^#.*', flags=re.MULTILINE)

LINE_COMMENT_RE = re.compile(r'//.*$', flags=re.MULTILINE)

MULTI_LINE_COMMENT_RE = re.compile(r'/\*.*?\*/', flags=re.DOTALL)

START_CHAR_LITERAL_RE = re.compile(r"(u8|u|U|L)?'")

START_STRING_LITERAL_RE = re.compile(r'(u8|u|U|L)?"')

START_RAW_STRING_LITERAL_RE = re.compile(r'(u8|u|U|L)?R"')

SINGLE_QUOTE_OR_BACKSLASH = re.compile(r"'|\\")

DOUBLE_QUOTE_OR_BACKSLASH = re.compile(r'"|\\')

FLOATING_POINT_RE = re.compile(r'[-+]?(\d+(\.\d*)?|\.\d+)([eE][-+]?\d+)?')

IDENTIFIER_RE = re.compile(r'[a-zA-Z_][a-zA-Z_0-9]*')

# Maybe useful later.
# integer_re = re.compile(r'[-+]?(0[xX][\dA-Fa-f]+|0[0-7]*|\d+)')
# rest_of_line_re = re.compile(r'.*$', flags=re.MULTILINE)


def find_char_end(file_src: str, start_pos: int) -> int:
  """Returns the pos just beyond the end of the char literal at start_pos."""
  # print('find_char_end start_pos:', start_pos)
  pos = start_pos + 1
  while pos < len(file_src):
    m = SINGLE_QUOTE_OR_BACKSLASH.search(file_src, pos=pos)
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
  """Returns the pos just beyond the string literal at start_pos."""
  pos = start_pos + 1
  while pos < len(file_src):
    m = DOUBLE_QUOTE_OR_BACKSLASH.search(file_src, pos=pos)
    if not m:
      break
    if file_src[m.start()] != '\\':
      return m.end()
    # Found a backslash in the string. Skip the character after the backslash.
    pos = m.end() + 1
  raise AssertionError(f'Unterminated string starting at {start_pos}')


def find_raw_string_literal_end(file_src: str, m: Match[str]) -> int:
  """Returns the pos just beyond the raw string literal that starts with m."""
  if not m.group(0).endswith('R"'):
    raise AssertionError(f'Expected start of raw string literal: {m.group()}')

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
  RAW_STRING = enum.auto()


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

# Kind of token, its phase 2 text, and its phase 2 start and beyond indices.
Phase2Token = Tuple[EToken, str, int, int]


def match_operator_or_punctuation(phase2_source: str,
                                  pos: int) -> Optional[Phase2Token]:
  for s, e in OPS_AND_PUNC:
    end_pos = pos + len(s)
    token = phase2_source[pos:end_pos]
    if s == token:
      return (e, s, pos, end_pos)
  return None


def generate_phase2_tokenization(
    phase2_source: str,
    file_path: str = '') -> Generator[Phase2Token, None, None]:
  """Yields relevant C++ tokens, not every valid/required C++ token.

  Only those matching those things I've needed for the tool(s) I've written
  (i.e. for matching enum definitions and generating enum logging support).

  Args:
    phase2_source: Source after phase 2 of translation.
    file_path: File name or path, used only in error messages.
  """

  pos = 0
  while pos < len(phase2_source):
    # print('pos=', pos)

    # Skip whitespace.
    m = WHITESPACE_RE.match(phase2_source, pos=pos)
    if m:
      # print(f'WHITESPACE_RE matched [{pos}:{m.end()}]')
      pos = m.end()
      continue

    # Skip attribute specifiers (e.g. [[noreturn]]). This pattern doesn't match
    # 'interesting' attribute specifier sequences, but so far those haven't been
    # needed.
    m = ATTRIBUTE_RE.match(phase2_source, pos=pos)
    if m:
      # print(f'ATTRIBUTE_RE matched [{pos}:{m.end()}]')
      pos = m.end()
      continue

    # Skip preprocessor lines.
    m = PREPROCESSOR_RE.match(phase2_source, pos=pos)
    if m:
      # print(f'PREPROCESSOR_RE matched [{pos}:{m.end()}]')
      pos = m.end()
      continue

    # Skip comments.
    m = LINE_COMMENT_RE.match(phase2_source, pos=pos)
    if m:
      # print(f'LINE_COMMENT_RE matched [{pos}:{m.end()}]')
      pos = m.end()
      continue
    m = MULTI_LINE_COMMENT_RE.match(phase2_source, pos=pos)
    if m:
      # print(f'MULTI_LINE_COMMENT_RE matched [{pos}:{m.end()}]')
      pos = m.end()
      continue

    # Match strings and characters.
    m = START_CHAR_LITERAL_RE.match(phase2_source, pos=pos)
    if m:
      # print(f'START_CHAR_LITERAL_RE matched [{pos}:{m.end()}]')
      end_pos = find_char_literal_end(phase2_source, m)
      token = phase2_source[pos:end_pos]
      yield (EToken.CHAR, token, pos, end_pos)
      pos = end_pos
      continue

    m = START_STRING_LITERAL_RE.match(phase2_source, pos=pos)
    if m:
      # print(f'START_STRING_LITERAL_RE matched [{pos}:{m.end()}]')
      end_pos = find_string_end(phase2_source, m.end() - 1)
      token = phase2_source[pos:end_pos]
      yield (EToken.STRING, token, pos, end_pos)
      pos = end_pos
      continue

    m = START_RAW_STRING_LITERAL_RE.match(phase2_source, pos=pos)
    if m:
      # print(f'START_RAW_STRING_LITERAL_RE matched [{pos}:{m.end()}]')
      end_pos = find_raw_string_literal_end(phase2_source, m)
      token = phase2_source[pos:end_pos]
      yield (EToken.RAW_STRING, token, pos, end_pos)
      pos = end_pos
      continue

    # Match numbers.
    m = FLOATING_POINT_RE.match(phase2_source, pos=pos)
    if m:
      # print(f'FLOATING_POINT_RE matched [{pos}:{m.end()}]')
      token = phase2_source[pos:m.end()]
      yield (EToken.NUMBER, token, pos, end_pos)
      pos = m.end()
      continue

    # Match operators and other punctuation. Not attempting to make them
    # complete tokens (i.e. '->' will be yielded as '-' and '>').

    if phase2_source[pos] in OPS_AND_PUNC_CHAR_SET:
      # print(f'Found operator or punctuation at {pos}')
      v = match_operator_or_punctuation(phase2_source, pos)
      if v:
        yield v
        pos += len(v[1])
        continue
      # This is surprising!
      raise ValueError(
          f'Unable to match {phase2_source[pos:pos+20]!r} in file {file_path!r}'
      )

    # Match identifiers.
    m = IDENTIFIER_RE.match(phase2_source, pos=pos)
    if m:
      # print(f'identifier_re matched [{pos}:{m.end()}]')
      token = phase2_source[pos:m.end()]
      yield (EToken.IDENTIFIER, token, pos, m.end())
      pos = m.end()
      continue

    raise ValueError(
        f'Unable to match {phase2_source[pos:pos+20]!r} in file {file_path!r}')


@dataclasses.dataclass()
class Token:
  """Represents one token in a C++ source file."""

  kind: EToken
  # Source of the token after phase 2 translation.
  src: str
  # Source of the token before phase 2 translation. In general, this is the
  # same as src, but not if a Raw String Literal with a backslash at the end
  # of a line in the string.
  raw_src: str

  # First character index in the raw file source, which here means before phase
  # 2 translation that collapses continued lines.
  raw_start: int

  # The character just beyond token in the raw file source.
  raw_end: int

  def __post_init__(self):
    if self.kind == EToken.RAW_STRING:
      # Just in case phase 2 processing removed backslash-newline pairs, we
      # replace src with raw_src.
      self.src = self.raw_src


IndexPair = Tuple[int, int]


class CppSource(object):
  """Provides mapping to the raw source from its phase2 translation."""

  def __init__(self, raw_source: str, file_path: str = ''):
    self.raw_source: str = raw_source
    self.file_path = file_path
    self.phase2_source: str = ''
    # A list of tuples: phase2_source index to raw_source index.
    self.phase2_source_to_raw_source: List[IndexPair] = []
    self.perform_phase2()

  def perform_phase2(self):
    """Remove occurrences of backlash-newline, tracks where that happened."""
    self.phase2_source = ''
    self.phase2_source_to_raw_source = []

    # start and end are indices in self.raw_source
    start = 0

    def append_phase2_segment(end: int):
      if start > end:
        raise AssertionError(f'start > end: {start} > {end}')
      elif start == end:
        # Continuation line marker at the start of the file or right after the
        # last such marker.
        return

      self.phase2_source_to_raw_source.append((len(self.phase2_source), start))
      if start == 0 and end == len(self.raw_source):
        # This is the most common case, and so we can avoid making a copy.
        if self.phase2_source:
          raise AssertionError('Why is phase2_source already set?')
        self.phase2_source = self.raw_source
      else:
        segment = self.raw_source[start:end]
        self.phase2_source = self.phase2_source + segment

    while start < len(self.raw_source):
      end = self.raw_source.find('\\\n', start)
      if end == -1:
        break
      append_phase2_segment(end)
      start = end + 2

    # There are no more continued lines.
    if start < len(self.raw_source):
      append_phase2_segment(len(self.raw_source))

    self.phase2_source_to_raw_source.append(
        (len(self.phase2_source), len(self.raw_source)))
    self.phase2_source_to_raw_source.reverse()

  def phase2_index_to_raw_source_index(self, ndx) -> int:
    if ndx < 0 or ndx > len(self.phase2_source):
      raise ValueError(f'ndx ({ndx} is not in [0, {len(self.phase2_source)}])')
    for phase2_index, raw_index in self.phase2_source_to_raw_source:
      if phase2_index <= ndx:
        return raw_index + (ndx - phase2_index)
    raise AssertionError(f'Unable to locate segment start at or before {ndx}; '
                         f'segment map: {self.phase2_source_to_raw_source}')

  def tokenize(self) -> Generator[Token, None, None]:
    for kind, src, phase2_start, phase2_end in generate_phase2_tokenization(
        self.phase2_source, file_path=self.file_path):
      if phase2_start >= phase2_end:
        raise AssertionError(
            f'phase2_start >= phase2_end ({phase2_start} > {phase2_end}) '
            f'for token {src!r} of kind {kind}')
      raw_start = self.phase2_index_to_raw_source_index(phase2_start)
      raw_end = self.phase2_index_to_raw_source_index(phase2_end - 1) + 1
      yield Token(kind, src, self.raw_source[raw_start:raw_end], raw_start,
                  raw_end)


def generate_cpp_tokens(file_src: str = '',
                        file_path: str = '') -> Generator[Token, None, None]:
  """Yields relevant C++ tokens, not every valid/required C++ token."""
  if not file_src:
    with open(file_path, mode='r') as f:
      file_src: str = f.read()

  cpp_source = CppSource(file_src, file_path=file_path)
  yield from cpp_source.tokenize()


def process_file(file_path: str):
  """Reads a file and prints its C++ tokenization."""
  print()
  print('#' * 80)
  print('Tokenizing file', file_path, flush=True)
  for token in generate_cpp_tokens(file_path=file_path):
    print(token)
  print()


def main(argv: Sequence[str]) -> None:
  for arg in argv[1:]:
    process_file(arg)


if __name__ == '__main__':
  main(sys.argv)
