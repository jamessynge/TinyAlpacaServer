#!/usr/bin/env python3
"""Split C++ source file into Tokens.

Why? In support of tools that read a source file and extract info, such as enum
definitions. While it would probably be better to build on tools like clang,
that requires installing a lot of tooling, while Python3 is commonly present and
doesn't require compiling the tool. Plus my experience writing a code analysis
tool using the clang API was annoying and it ran very slowly.

A Token instance tells us the kind of token found, the source text for the token
and the position (character index within the file).

Raises ValueError for cases where the C++ source appears invalid (e.g. an
unterminated string), and AssertionError where I think that the code must have
a bug (i.e. is called incorrectly, or has produced an invalid state).
"""

import dataclasses
import enum
import glob
import os.path
import re
import sys
from typing import Callable, Generator, List, Match, Optional, Sequence, Tuple, Union

import file_edit


class EToken(enum.Enum):
  """Types of tokens found by generate_cpp_tokens."""
  CHAR = enum.auto()
  COLON = enum.auto()
  IDENTIFIER = enum.auto()
  LEFT_BRACE = enum.auto()
  LEFT_BRACKET = enum.auto()
  LEFT_PARENTHESIS = enum.auto()
  FP_NUMBER = enum.auto()
  INTEGER = enum.auto()
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
  RAW_STRING = enum.auto()
  RIGHT_BRACE = enum.auto()
  RIGHT_BRACKET = enum.auto()
  RIGHT_PARENTHESIS = enum.auto()
  SEMICOLON = enum.auto()
  STRING = enum.auto()
  # The following aren't C++ language tokens. Comments are removed by a compiler
  # in phase 3, and preprocessor directives are removed in phase 4.
  COMMENT = enum.auto()
  PREPROCESSOR_DIRECTIVE = enum.auto()


NOT_CPP_LANGUAGE_TOKENS = set([EToken.COMMENT, EToken.PREPROCESSOR_DIRECTIVE])
START_AND_END_TOKENS = {
    EToken.LEFT_BRACE: EToken.RIGHT_BRACE,
    EToken.LEFT_BRACKET: EToken.RIGHT_BRACKET,
    EToken.LEFT_PARENTHESIS: EToken.RIGHT_PARENTHESIS,
}

# Tokens that could reasonably be in a purely numeric expression.
NUMERIC_EXPRESSION_TOKENS = set([
    EToken.CHAR,
    EToken.COLON,
    EToken.LEFT_PARENTHESIS,
    EToken.FP_NUMBER,
    EToken.INTEGER,
    EToken.OP_AND,
    EToken.OP_BITAND,
    EToken.OP_BITNOT,
    EToken.OP_BITOR,
    EToken.OP_BITXOR,
    EToken.OP_DIVIDE,
    EToken.OP_EQUAL,
    EToken.OP_GREATER_THAN,
    EToken.OP_GREATER_THAN_EQUAL,
    EToken.OP_LESS_THAN,
    EToken.OP_LESS_THAN_EQUAL,
    EToken.OP_MINUS,
    EToken.OP_MODULO,
    EToken.OP_NOT,
    EToken.OP_NOT_EQUAL,
    EToken.OP_OR,
    EToken.OP_PLUS,
    EToken.OP_SHIFT_LEFT,
    EToken.OP_SHIFT_RIGHT,
    EToken.OP_TERNARY,
    EToken.OP_TIMES,
    EToken.RIGHT_PARENTHESIS,
])


@dataclasses.dataclass()
class Token:
  """Represents one token in a C++ source file."""

  kind: EToken
  # Source of the token after phase 2 translation.
  src: str
  # Source of the token before phase 2 translation. In general, this is the
  # same as src, but not if a Raw String Literal with a backslash at the end
  # of a line in the string because in that case the backslash and newline
  # characters are removed by phase 2.
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


@dataclasses.dataclass()
class Group:
  """Represents a group of tokens surrounded by bookends.

  The bookends are those in START_AND_END_TOKENS.
  """
  start_token: Token
  nested: List[Union[Token, 'Group']]
  end_token: Token


TokenOrGroup = Union[Token, Group]

# Regular expressions used to match various tokens, or parts of such tokens, in
# a C++ source file.
# Without DOTALL, '.' does not match newlines. MULTILINE ensures that ^ will
# match the start of lines within the source, and '$' will match the end of
# lines within the source.

# Pattern that matches whitespace inside a line, but not \n or \r. This is
# pattern has a double-negative in it: \S means not whitespace, but that is in
# a complementing (i.e. NOT) character set, i.e. one that matches characters not
# in the set.
LINE_WS_PAT = r'[^\S\r\n]'

# Pattern that matches the end of a line. I don't expect carriage return (\r) to
# appear in practice because Python will regularize line endings to the newline
# character (\n).
NL_PAT = r'(?:\r\n?|\n)'

# A preprocessor directive can only appear on a line by itself, which means that
# we need to take care when skipping over whitespace that we don't lose track of
# the fact that the next item is a '#' that is logically the first thing (except
# whitespace) on the line.
PREPROCESSOR_RE = re.compile(
    f'^{LINE_WS_PAT}*#{LINE_WS_PAT}*\\w.*', flags=re.MULTILINE)

# We skip over whitespace in at most one line at a time.
WHITESPACE_RE = re.compile(f'(?:{LINE_WS_PAT}+{NL_PAT}?|{NL_PAT})')

# Simplistic attribute specifier matcher. Does not match alignas.
ATTRIBUTE_RE = re.compile(r'\[\[.+?\]\]', flags=re.DOTALL)

LINE_COMMENT_RE = re.compile(r'//.*$', flags=re.MULTILINE)

START_CHAR_LITERAL_RE = re.compile(r"(u8|u|U|L)?'")

START_STRING_LITERAL_RE = re.compile(r'(u8|u|U|L)?"')

START_RAW_STRING_LITERAL_RE = re.compile(r'(u8|u|U|L)?R"')

SINGLE_QUOTE_OR_BACKSLASH = re.compile(r"'|\\")

DOUBLE_QUOTE_OR_BACKSLASH = re.compile(r'"|\\')

# There are lots of different number formats in C++, and many of them will match
# at the start of a number, therefore we generally need the longest match. This
# regex matches the start of a sequence that could be a number, after which we
# can look for the longest match at that point to determine whether it is an
# integer or floating point number.
NUMBER_RE = re.compile(r'[0-9]|([.][0-9])')

# Patterns from: https://en.cppreference.com/w/cpp/language/floating_literal
# Note: Not yet supporting C++ 17's hexadecimal floating-point literals.
DIGIT_SEQUENCE_PAT = r'[0-9]+'
OPT_DIGIT_SEQUENCE_PAT = f'(?:{DIGIT_SEQUENCE_PAT})?'
DECIMAL_EXPONENT_PAT = r'[eE][-+]?' + DIGIT_SEQUENCE_PAT
OPT_DECIMAL_EXPONENT_PAT = f'(?:{DECIMAL_EXPONENT_PAT})?'

FP_CASE_1 = DIGIT_SEQUENCE_PAT + DECIMAL_EXPONENT_PAT
FP_CASE_2 = f'{DIGIT_SEQUENCE_PAT}[.]{OPT_DECIMAL_EXPONENT_PAT}'
FP_CASE_3 = f'{OPT_DIGIT_SEQUENCE_PAT}[.]{DIGIT_SEQUENCE_PAT}{OPT_DECIMAL_EXPONENT_PAT}'
FP_CASES = (
    f'(?:{FP_CASE_3})|'  # Order matters.
    f'(?:{FP_CASE_2})|'
    f'(?:{FP_CASE_1})')
FP_SUFFIX_PAT = '[fFlL]'

FLOATING_POINT_RE = re.compile(f'(?:{FP_CASES}){FP_SUFFIX_PAT}?')

INT_CASE_1 = '[1-9][0-9]*'  # Decimal literal
INT_CASE_2 = '0[0-7]*'  # Octal literal
INT_CASE_3 = '0[xX][0-9a-fA-F]+'  # Hex literal
INT_CASE_4 = '0[bB][01]+'  # Binary literal
# Need the octal case to come last, else it will match the zero at the start of
# hex and binary literals, and stop the match at that one digit.
INT_CASES = (f'(?:{INT_CASE_1})|'
             f'(?:{INT_CASE_3})|'
             f'(?:{INT_CASE_4})|'
             f'(?:{INT_CASE_2})')
# Need 'll' to come before 'l', else 'l' will match the first character of 'll'
# and stop the match at that one character.
INT_SIZE_PAT = '(?:ll|LL|l|L|z|Z)'
INT_SIZE_UNSIGNED_PAT = f'(?:{INT_SIZE_PAT}[uU]?)'
INT_UNSIGNED_SIZE_PAT = f'(?:[uU]{INT_SIZE_PAT}?)'
INT_SUFFIX_PAT = f'(?:{INT_SIZE_UNSIGNED_PAT}|{INT_UNSIGNED_SIZE_PAT})'

INTEGER_RE = re.compile(f'(?:{INT_CASES}){INT_SUFFIX_PAT}?')
# # r'([1-9][0-9]*)|(0[0-7]*)|(0x[0-9a-f]+)|(0b[01]+)', flags=re.IGNORECASE)
# UNSIGNED_SUFFIX_RE = re.compile(r'U', flags=re.IGNORECASE)
# SIZE_SUFFIX_RE = re.compile(r'l|L|ll|LL|z|Z')

# Not worrying here about supporting non-English alphabets.
IDENTIFIER_RE = re.compile(r'[a-zA-Z_][a-zA-Z_0-9]*')


def find_char_end(file_src: str, start_pos: int, pos: int) -> int:
  """Returns the position just beyond the end of the char literal.

  Args:
    file_src: The C++ source.
    start_pos: The start of the character literal. Might be a prefix like L.
    pos: The start of the contents of the character literal, i.e. after the
      starting single quote.

  Returns:
    The position just after the ending single quote.
  """
  # print(f'find_char_end(, {start_pos}, {pos})')
  if file_src[pos] != '\\':
    # Not an escaped character, so must not be single quote or newline, and the
    # character after that must be a single quote.
    if file_src[pos] not in "'\n" and file_src[pos + 1] == "'":
      return pos + 2
    if file_src[pos] == "'":
      raise ValueError(f'Empty character literal starting at {start_pos}: '
                       f'{file_src[start_pos:start_pos+20]}')
    raise ValueError(f'Too many characters in literal starting at {start_pos}: '
                     f'{file_src[start_pos:start_pos+20]}')

  # Contains an escaped character. Is it a simple escape?
  pos += 1
  if file_src[pos] in r"""'"?\abfnrtv""":
    # Next character must be a single quote.
    if file_src[pos + 1] == "'":
      return pos + 2
    raise ValueError(f'Too many characters in literal starting at {start_pos}: '
                     f'{file_src[start_pos:start_pos+20]}')

  # A more complicated escaped literal. Not (yet) bothering to validate the
  # contents make sense, just looking for the closing quote.
  sq_pos = file_src.find("'", pos)
  if sq_pos != -1:
    if sq_pos - pos > 10:
      raise ValueError(
          f'Too many characters in literal starting at {start_pos}: '
          f'{file_src[start_pos:start_pos+20]}')
    return sq_pos + 1
  raise ValueError(f'Unterminated character starting at {start_pos}: '
                   f'{file_src[start_pos:start_pos+20]}')


def find_char_literal_end(file_src: str, m: Match[str]) -> int:
  try:
    return find_char_end(file_src, m.start(), m.end())
  except IndexError:
    pass
  raise ValueError(f'Unterminated character starting at {m.start()}: '
                   f'{file_src[m.start():m.start()+20]}')


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
  raise ValueError(f'Unterminated string starting at {start_pos}')


def find_raw_string_literal_end(file_src: str, m: Match[str]) -> int:
  """Returns the pos just beyond the raw string literal that starts with m."""
  if not m.group(0).endswith('R"'):
    raise AssertionError(  # COV_NF_START
        f'Expected start of raw string literal: {m.group()}')  # COV_NF_END

  # We've matched the start of a raw string literal. Determine the delimiter,
  # then search for the end of the string.
  regexp = re.compile(r'[^()\\ \f\n\r\t\v]{0,16}\(')
  m2 = regexp.match(file_src, pos=m.end())
  if not m2:
    raise ValueError(
        'Unable to locate opening delimiter of the raw string literal '
        f'starting at {m.start()}: {file_src[m.start():m.start()+32]!r}')

  needle = ')' + m2.group()[0:-1] + '"'
  pos1 = file_src.find(needle, m2.end())
  if pos1 < 0:
    raise ValueError(
        'Unable to locate closing delimiter of the raw string literal '
        f'starting at {m.start()}: {file_src[m.start():m.start()+32]!r}')

  pos2 = pos1 + len(needle)
  return pos2


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
  return None  # COV_NF_LINE


def attempt_fp_match(phase2_source: str, pos: int) -> Optional[Phase2Token]:
  """Return an FP_NUMBER Phase2Token if able to match an FP number."""

  m: Match[str] = FLOATING_POINT_RE.match(phase2_source, pos=pos)
  if not m:
    return None
  end_pos = m.end()
  return (EToken.FP_NUMBER, phase2_source[pos:end_pos], pos, end_pos)


def attempt_int_match(phase2_source: str, pos: int) -> Optional[Phase2Token]:
  """Return an INTEGER Phase2Token if able to match an integer."""

  start_pos = pos
  m = INTEGER_RE.match(phase2_source, pos=pos)
  if not m:
    return None  # COV_NF_LINE
  end_pos = m.end()
  return (EToken.INTEGER, phase2_source[start_pos:end_pos], start_pos, end_pos)


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

  def confirm_number_properly_terminated(end_pos: int, kind: str) -> None:
    # A number must not be followed immediately by an identifier, nor by another
    # number.
    m = (
        IDENTIFIER_RE.match(phase2_source, pos=end_pos) or
        NUMBER_RE.match(phase2_source, pos=end_pos))
    if not m:
      return
    raise ValueError(f'Matched {phase2_source[pos:end_pos]!r}, spanning '
                     f'[{pos}:{end_pos}] as {kind}, but it is followed by an '
                     f'invalid character: {phase2_source[end_pos]!r}')

  while pos < len(phase2_source):
    # print('pos=', pos)
    try:
      # Match preprocessor lines.
      m = PREPROCESSOR_RE.match(phase2_source, pos=pos)
      if m:
        # print(f'PREPROCESSOR_RE matched [{pos}:{m.end()}]')
        token = phase2_source[pos:m.end()]
        yield (EToken.PREPROCESSOR_DIRECTIVE, token, pos, m.end())
        pos = m.end()
        continue

      # Skip whitespace, up through the end of the current line. We don't skip
      # multiple lines so that the PREPROCESSOR_RE match below will be able to
      # determine whether we're at the start of a line.
      m = WHITESPACE_RE.match(phase2_source, pos=pos)
      if m:
        # print(f'WHITESPACE_RE matched [{pos}:{m.end()}]')
        pos = m.end()
        continue

      # Skip attribute specifiers (e.g. [[noreturn]]). This pattern doesn't
      # match 'interesting' attribute specifier sequences, but so far those
      # haven't been needed. And so far I don't have a desire for the tokenizer
      # to return the parts of an attribute specifier, which is convenient since
      # I don't have a good idea how to reliably identify more complex attribute
      # specifiers... especially their ends (i.e. "[[" only appears at the start
      # of an attribute specifier, but "]]" often appear in nested index
      # expressions).
      m = ATTRIBUTE_RE.match(phase2_source, pos=pos)
      if m:
        # print(f'ATTRIBUTE_RE matched [{pos}:{m.end()}]')
        pos = m.end()
        continue

      # Match comments.
      m = LINE_COMMENT_RE.match(phase2_source, pos=pos)
      if m:
        # print(f'LINE_COMMENT_RE matched [{pos}:{m.end()}]')
        token = phase2_source[pos:m.end()]
        yield (EToken.COMMENT, token, pos, m.end())
        pos = m.end()
        continue

      if phase2_source.startswith('/*', pos):
        # print(f'Multi-line comment starts at {pos}')
        end_pos = phase2_source.find('*/', pos + 1)
        if end_pos != -1:
          end_pos += 2
          token = phase2_source[pos:end_pos]
          yield (EToken.COMMENT, token, pos, end_pos)
          pos = end_pos
          continue
        raise ValueError('Unterminated multi-line comment starts with '
                         f'{phase2_source[pos:pos+20]!r} at {pos}, '
                         f'in file {file_path!r}')

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
      m = NUMBER_RE.match(phase2_source, pos=pos)
      if m:
        # Maybe a floating point number.
        opt_token: Optional[Phase2Token] = attempt_fp_match(phase2_source, pos)
        if opt_token:
          end_pos = opt_token[3]
          confirm_number_properly_terminated(end_pos,
                                             'a floating point literal')
          yield opt_token
          pos = end_pos
          continue

        opt_token = attempt_int_match(phase2_source, pos)
        if opt_token:
          end_pos = opt_token[3]
          confirm_number_properly_terminated(end_pos, 'an integer literal')
          yield opt_token
          pos = end_pos
          continue

        # This is surprising!
        # COV_NF_START
        raise ValueError(f'Unable to match {phase2_source[pos:pos+20]!r} '
                         f'as a number in file {file_path!r}')  # COV_NF_END

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
        raise ValueError(f'Unable to match {phase2_source[pos:pos+20]!r} in '
                         f'file {file_path!r}')

      # Match identifiers.
      m = IDENTIFIER_RE.match(phase2_source, pos=pos)
      if m:
        # print(f'identifier_re matched [{pos}:{m.end()}]')
        token = phase2_source[pos:m.end()]
        yield (EToken.IDENTIFIER, token, pos, m.end())
        pos = m.end()
        continue
    except ValueError as ex:
      # Add some detail to the exception message.
      if ex.args:
        msg = (f'{ex.args[0]}\nWhile matching {phase2_source[pos:pos+20]!r} in '
               f'file {file_path!r}')
        ex.args = (msg,) + ex.args[1:]
      raise
    except:
      print(
          'Exception while matching '
          f'{phase2_source[pos:pos+20]!r} in file {file_path!r}',
          file=sys.stderr,
          flush=True)
      raise

    # We've tried all our available patterns, none match!
    raise ValueError(
        f'Unable to match {phase2_source[pos:pos+20]!r} in file {file_path!r}')


IndexPair = Tuple[int, int]


class Phase2Source(object):
  """Provides mapping to the raw source from its phase2 translation."""

  def __init__(self, raw_source: str, file_path: str):
    self.raw_source: str = raw_source
    self.file_path: str = file_path
    self.phase2_source: str = ''
    # A list of tuples: phase2_source index to raw_source index.
    self.phase2_source_to_raw_source: List[IndexPair] = []

    # start and end are indices in raw_source
    start = 0
    len_raw_source = len(raw_source)

    def append_phase2_segment(end: int):
      if start > end:
        raise AssertionError(f'start > end: {start} > {end}')
      elif start == end:
        # Continuation line marker at the start of the file or right after the
        # last such marker.
        return

      self.phase2_source_to_raw_source.append((len(self.phase2_source), start))
      if start == 0 and end == len_raw_source:
        # This is the most common case, and so we can avoid making a copy.
        if self.phase2_source:
          raise AssertionError('Why is phase2_source already set?')
        self.phase2_source = raw_source
      else:
        segment = raw_source[start:end]
        self.phase2_source = self.phase2_source + segment

    # Remove occurrences of backlash-newline and record where that happened.
    while start < len_raw_source:
      end = raw_source.find('\\\n', start)
      if end == -1:
        break
      append_phase2_segment(end)
      start = end + 2

    # There are no more continued lines.
    if start < len_raw_source:
      append_phase2_segment(len_raw_source)

    self.phase2_source_to_raw_source.append(
        (len(self.phase2_source), len_raw_source))
    self.phase2_source_to_raw_source.reverse()

  def phase2_index_to_raw_source_index(self, ndx) -> int:
    if ndx < 0 or ndx > len(self.phase2_source):
      raise ValueError(f'ndx ({ndx} is not in [0, {len(self.phase2_source)}])')
    # TODO(jamessynge): Maybe use binary search here.
    for phase2_index, raw_index in self.phase2_source_to_raw_source:
      if phase2_index <= ndx:
        return raw_index + (ndx - phase2_index)
    raise AssertionError(f'Unable to locate segment start at or before {ndx}; '
                         f'segment map: {self.phase2_source_to_raw_source}')

  def tokenize(self, source_only=True) -> Generator[Token, None, None]:
    """Yields C++ Tokens found in the phase2 source."""
    for kind, src, phase2_start, phase2_end in generate_phase2_tokenization(
        self.phase2_source, file_path=self.file_path):
      if phase2_start >= phase2_end:
        raise AssertionError(
            f'phase2_start >= phase2_end ({phase2_start} > {phase2_end}) '
            f'for token {src!r} of kind {kind}')
      if (source_only and kind in NOT_CPP_LANGUAGE_TOKENS):
        continue
      raw_start = self.phase2_index_to_raw_source_index(phase2_start)
      raw_end = self.phase2_index_to_raw_source_index(phase2_end - 1) + 1
      yield Token(kind, src, self.raw_source[raw_start:raw_end], raw_start,
                  raw_end)


def group_cpp_tokens(tokens: List[Token]) -> List[TokenOrGroup]:
  """Replace tokens within grouping tokens with a Group."""

  ndx_limit = len(tokens)

  def nest_until_token(start_ndx: int) -> Tuple[Group, int]:
    """abc."""
    start_token = tokens[start_ndx]
    target_kind = START_AND_END_TOKENS[start_token.kind]
    nested: List[TokenOrGroup] = []
    ndx = start_ndx + 1
    while ndx < ndx_limit:
      if tokens[ndx].kind == target_kind:
        group = Group(
            start_token=start_token, nested=nested, end_token=tokens[ndx])
        return group, ndx + 1
      if tokens[ndx].kind in START_AND_END_TOKENS:
        inner, ndx = nest_until_token(ndx)
        nested.append(inner)
        continue
      nested.append(tokens[ndx])
      ndx += 1
      continue
    raise ValueError(
        f'Starting at token #{start_ndx} ({start_token}), failed to find '
        f'closing token of type {target_kind}')

  result: List[TokenOrGroup] = []
  ndx = 0
  while ndx < ndx_limit:
    if tokens[ndx].kind in START_AND_END_TOKENS:
      inner, ndx = nest_until_token(ndx)
      result.append(inner)
    else:
      result.append(tokens[ndx])
      ndx += 1
  return result


def dump_grouped_tokens(grouped_tokens: List[TokenOrGroup],
                        indent: int = 0) -> None:
  """Prints the grouped tokens indented based on nesting level."""
  prefix = '  ' * indent
  for elem in grouped_tokens:
    if isinstance(elem, Group):
      print(f'{prefix} {elem.start_token}')
      dump_grouped_tokens(elem.nested, indent + 1)
      print(f'{prefix} {elem.end_token}')
    else:
      print(f'{prefix}{elem}')


def flatten_grouped_tokens(
    grouped_tokens: List[TokenOrGroup]) -> Generator[Token, None, None]:
  """Yields the Tokens in the list whose elements are Tokens and/or Groups."""
  for elem in grouped_tokens:
    if isinstance(elem, Token):
      yield elem
    else:
      elem: Group
      yield from flatten_group(elem)


def flatten_group(group: Group) -> Generator[Token, None, None]:
  """Yields the Tokens in the Group."""
  yield group.start_token
  yield from flatten_grouped_tokens(group.nested)
  yield group.end_token


def is_not_numeric_expression(grouped_tokens: List[TokenOrGroup]) -> bool:
  """Returns True if group_tokens aren't valid in purely numeric expressions."""
  return any(
      map(lambda x: x not in NUMERIC_EXPRESSION_TOKENS,
          flatten_grouped_tokens(grouped_tokens)))


def stringify_grouped_tokens(grouped_tokens: List[TokenOrGroup]) -> str:
  """Join the raw_src of the flattened grouped tokens into a single string."""
  if not grouped_tokens:
    return ''
  tokens = list(flatten_grouped_tokens(grouped_tokens))
  strings: List[str] = []
  pos = tokens[0].raw_start
  for token in tokens:
    if pos < token.raw_start:
      strings.append(' ')
    strings.append(token.raw_src)
    pos = token.raw_end
  return ''.join(strings)


@dataclasses.dataclass()
class Replacement:
  """Represents a replacement of a section of raw source with some text."""

  # The insertion point for the replacement text.
  start: int

  # The end of the block to be replaced. If the same as raw_start, then no
  # existing text is replaced, only an insertion is performed.
  end: int

  # Text to insert.
  text: str

  def __post_init__(self):
    if self.start < 0 or self.start > self.end:
      raise AssertionError(f'Invalid replacement range: {self}')
    if self.start == self.end and not self.text:
      raise AssertionError(f'No-op replacement is not permitted: {self}')

  @staticmethod
  def before(token: Token, text: str) -> 'Replacement':
    return Replacement(start=token.raw_start, end=token.raw_start, text=text)

  @staticmethod
  def after(token: Token, text: str) -> 'Replacement':
    return Replacement(start=token.raw_end, end=token.raw_end, text=text)


def has_source_extension(file_path: str) -> bool:
  """Returns True if the file extension indicates this is a C++ source file."""
  if not file_path:
    return False
  _, ext = os.path.splitext(file_path)
  return ext.lower() in ('.cc', '.cpp', '.cxx', '.c++', '.cp') or ext == '.C'


def has_header_extension(file_path: str) -> bool:
  """Returns True if the file extension indicates this is a C++ header file."""
  if not file_path:
    return False
  _, ext = os.path.splitext(file_path)
  return ext.lower() in ('.h', '.hpp', '.hxx', '.h++')


def find_same_base_files(
    file_path: str,
    filter_fn: Optional[Callable[[str], bool]] = None) -> List[str]:
  """Returns the other files with the same basename as file_path."""
  base, _ = os.path.splitext(file_path)
  paths = glob.glob(base + '.*')
  if filter_fn:
    paths = filter(filter_fn, paths)
  return [f for f in paths if f != file_path]


def find_source_paths(file_path: str) -> List[str]:
  if has_source_extension(file_path):
    return [file_path]
  return find_same_base_files(file_path, has_source_extension)


def find_header_paths(file_path: str) -> List[str]:
  if has_header_extension(file_path):
    return [file_path]
  return find_same_base_files(file_path, has_header_extension)


class CppSource(object):
  """FooBar."""

  def __init__(self, file_path: str = '', raw_source: str = ''):
    self.file_contents = file_edit.FileContents(
        file_path=file_path, raw_source=raw_source)
    self._phase2_source = None
    self._all_tokens = None

  @property
  def file_path(self) -> str:
    return self.file_contents.file_path

  @property
  def is_source_file(self) -> bool:
    return has_source_extension(self.file_path)

  @property
  def is_header_file(self) -> bool:
    return has_header_extension(self.file_path)

  @property
  def raw_source(self) -> str:
    return self.file_contents.raw_source

  @raw_source.setter
  def raw_source(self, value):
    self.file_contents.raw_source = value
    self._phase2_source = None
    self._all_tokens: Optional[List[Token]] = None

  @property
  def phase2_source(self) -> Phase2Source:
    if self._phase2_source is None:
      self._phase2_source = Phase2Source(self.file_contents.raw_source,
                                         self.file_contents.file_path)
    return self._phase2_source

  def _get_all_tokens(self) -> List[Token]:
    if self._all_tokens is None:
      self._all_tokens = list(self.phase2_source.tokenize(source_only=False))
    return self._all_tokens

  @property
  def all_tokens(self) -> List[Token]:
    return list(self._get_all_tokens())

  @property
  def cpp_tokens(self) -> List[Token]:
    return [
        t for t in self._get_all_tokens()
        if t.kind not in NOT_CPP_LANGUAGE_TOKENS
    ]

  @property
  def grouped_cpp_tokens(self) -> List[TokenOrGroup]:
    return group_cpp_tokens(self.cpp_tokens)

  def edit_raw_source(self, replacements: List[Replacement]) -> None:
    """Edit the file, applying the specified Replacements."""
    raw_source = self.raw_source
    replacements = sorted(replacements, key=lambda repl: repl.start)
    for n in range(1, len(replacements)):
      if replacements[n - 1].end >= replacements[n].start:
        raise AssertionError('Replacements overlap:\n'
                             f'replacements[{n-1}]: {replacements[n-1]}\n'
                             f'replacements[{n}]: {replacements[n]}')
    parts: List[str] = []
    pos = 0
    for replacement in replacements:
      replacement: Replacement
      if replacement.end > len(raw_source):
        raise AssertionError(
            f'Replacement is beyond end of source: {replacement}')
      if pos < replacement.start:
        parts.append(raw_source[pos:replacement.start])
      parts.append(replacement.text)
      pos = replacement.end
    if pos < len(raw_source):
      parts.append(raw_source[pos:])
    new_raw_source = ''.join(parts)
    self.raw_source = new_raw_source

  def update_file(self):
    self.file_contents.update_file()


def generate_cpp_tokens(file_src: str = '',
                        file_path: str = '',
                        source_only=True) -> List[Token]:
  """Returns the C++ tokens found in the file.

  Not a perfect tokenizer, and if source_only=False, also emits Token instances
  for comments and for preprocessor directives.

  Args:
    file_src: Body of the file to tokenize.
    file_path: Name of the file.
    source_only: Whether to only yield C++ source tokens.
  """
  cpp_source = CppSource(file_path=file_path, raw_source=file_src)
  if source_only:
    return cpp_source.cpp_tokens
  else:
    return cpp_source.all_tokens


def process_file(file_path: str):
  """Reads a file and prints its C++ tokenization with nesting of groups."""
  print()
  print('#' * 80)
  print('Finding nested C++ tokens in file', file_path, flush=True)
  cpp_source = CppSource(file_path=file_path)
  dump_grouped_tokens(cpp_source.grouped_cpp_tokens)


def main(argv: Sequence[str]) -> None:
  for arg in argv[1:]:
    process_file(arg)


if __name__ == '__main__':
  main(sys.argv)
  sys.stdout.flush()
