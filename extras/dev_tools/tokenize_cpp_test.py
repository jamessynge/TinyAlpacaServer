"""Tests for tokenize_cpp."""

from absl import flags

import tokenize_cpp
import absltest

EToken = tokenize_cpp.EToken
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

  def test_replacement(self):
    cpp_source = tokenize_cpp.CppSource(raw_source=' "str" ')
    self.assertSequenceEqual(cpp_source.all_tokens,
                             [make_expected_string_token('"str"', raw_start=1)])
    self.assertSequenceEqual(cpp_source.all_tokens, cpp_source.cpp_tokens)
    self.assertSequenceEqual(cpp_source.all_tokens,
                             cpp_source.grouped_cpp_tokens)
    token = cpp_source.all_tokens[0]
    # In backwards ordered deliberately.
    replacements = [
        tokenize_cpp.Replacement.after(token, ')'),
        tokenize_cpp.Replacement.before(token, 'X('),
    ]
    cpp_source.edit_raw_source(replacements)
    self.assertEqual(cpp_source.raw_source, ' X("str") ')
    self.assertLen(cpp_source.all_tokens, 4)
    self.assertSequenceEqual(cpp_source.all_tokens, cpp_source.cpp_tokens)
    self.assertSequenceEqual([t.kind for t in cpp_source.all_tokens], [
        EToken.IDENTIFIER, EToken.LEFT_PARENTHESIS, EToken.STRING,
        EToken.RIGHT_PARENTHESIS
    ])


if __name__ == '__main__':
  absltest.main()
