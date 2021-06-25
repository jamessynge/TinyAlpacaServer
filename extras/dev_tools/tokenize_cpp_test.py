"""Tests for tokenize_cpp."""

from absl import flags

import tokenize_cpp
import absltest

FLAGS = flags.FLAGS

flags.DEFINE_string(
    name='vmodule',
    required=False,
    default='',
    help='Ignored; defined just to be ignored if provided to all tests.')


def make_expected_string_token(src, raw_start=0, raw_src=''):
  if not raw_src:
    raw_src = src
  return tokenize_cpp.Token(
      kind=tokenize_cpp.EToken.STRING,
      src=src,
      raw_src=raw_src,
      raw_start=raw_start,
      raw_end=raw_start + len(raw_src))


def make_expected_raw_string_token(raw_src, raw_start=0):
  return tokenize_cpp.Token(
      kind=tokenize_cpp.EToken.RAW_STRING,
      src=raw_src,
      raw_src=raw_src,
      raw_start=raw_start,
      raw_end=raw_start + len(raw_src))


class TokenizeCppTest(absltest.TestCase):

  STRING_PREFIXES = ['', 'L', 'u8', 'u', 'U']

  def test_empty_string_literals(self):
    for prefix in self.STRING_PREFIXES:
      s = prefix + '""'
      src = s
      tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
      # print(tokenization, flush=True)
      self.assertSequenceEqual(tokenization, [make_expected_string_token(s)])

  def test_non_empty_string_literals(self):
    for prefix in self.STRING_PREFIXES:
      s = prefix + '"abc"'
      src = '\n' + s + '\t'
      tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
      # print(tokenization, flush=True)
      self.assertSequenceEqual(tokenization,
                               [make_expected_string_token(s, raw_start=1)])

  def test_string_literals_with_continuation(self):
    for prefix in self.STRING_PREFIXES:
      raw = prefix + '"a\\\nb\\\nc"'
      s = prefix + '"abc"'
      src = '\n' + raw + '\t'
      tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
      # print(tokenization, flush=True)
      self.assertSequenceEqual(
          tokenization,
          [make_expected_string_token(s, raw_start=1, raw_src=raw)])

  def test_empty_raw_string_literals(self):
    for prefix in self.STRING_PREFIXES:
      for delimiter in ['', '1234567890123456', '"']:
        s = f'{prefix}R"{delimiter}(){delimiter}"'
        src = ' ' + s
        tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
        # print(tokenization, flush=True)
        self.assertSequenceEqual(
            tokenization, [make_expected_raw_string_token(s, raw_start=1)])

  def test_non_empty_raw_string_literals(self):
    inner_string = 'Some text with "" (quotes) and with () (parentheses)'
    for prefix in self.STRING_PREFIXES:
      for delimiter in ['', '1234567890123456', '"']:
        s = f'{prefix}R"{delimiter}({inner_string}){delimiter}"'
        src = s + ' '
        tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
        # print(tokenization, flush=True)
        self.assertSequenceEqual(tokenization,
                                 [make_expected_raw_string_token(s)])

  def test_raw_string_literal_with_continuation(self):
    continuations = '\\\n\\\n'
    inner_string = 'Some text with a continued \\\n line'
    for prefix in self.STRING_PREFIXES:
      for delimiter in ['', '1234567890123456', '"']:
        s = f'{prefix}R"{delimiter}({inner_string}){delimiter}"'
        src = continuations + s
        expected = make_expected_raw_string_token(
            s, raw_start=len(continuations))
        # print(expected, flush=True)
        tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
        # print(tokenization, flush=True)
        self.assertSequenceEqual(tokenization, [expected])


if __name__ == '__main__':
  absltest.main()
