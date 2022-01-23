"""Tests for tokenize_cpp."""

import unittest.mock

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

  def test_char_literal(self):
    for prefix in self.STRING_PREFIXES:
      for contents in ['a', r"\'", r'\a', r'\001', r'\xAA', r'\\']:
        s = f"{prefix}'{contents}'"
        # print('s:', s, f'    rs: {s!r}', flush=True)
        src = f'\n{s}\t'
        expected_token = tokenize_cpp.Token(
            kind=tokenize_cpp.EToken.CHAR,
            src=s,
            raw_src=s,
            raw_start=1,
            raw_end=1 + len(s))
        tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
        # print(tokenization, flush=True)
        self.assertSequenceEqual(tokenization, [expected_token])

  def test_invalid_char_literal(self):
    verbose = True
    short_esq = '\\'
    long_esq = '\\' + ' ' * 30 + "'"
    unterminated_esq = '\\   '
    for prefix in self.STRING_PREFIXES:
      for contents in [
          "a '", "'", "\n'", r"\? '", short_esq, long_esq, unterminated_esq
      ]:
        s = f"{prefix}'{contents}"
        if verbose:
          print('\ns:', s, f'    rs: {s!r}', flush=True)
        try:
          tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=s))
        except AssertionError as e:
          if verbose:
            print(e.args[0], flush=True)
          continue
        print(tokenization, flush=True)
        self.fail('Should not reach here!')

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

  def test_has_source_extension(self):
    self.assertTrue(tokenize_cpp.has_source_extension('a/b.cc'))
    self.assertTrue(tokenize_cpp.has_source_extension('/a/b.cxx'))
    self.assertTrue(tokenize_cpp.has_source_extension('b.cpp'))
    self.assertFalse(tokenize_cpp.has_source_extension('a/b.h'))
    self.assertFalse(tokenize_cpp.has_source_extension('/a/b.hxx'))
    self.assertFalse(tokenize_cpp.has_source_extension('b.hpp'))

  def test_has_header_extension(self):
    self.assertFalse(tokenize_cpp.has_header_extension('a/b.cc'))
    self.assertFalse(tokenize_cpp.has_header_extension('/a/b.cxx'))
    self.assertFalse(tokenize_cpp.has_header_extension('b.cpp'))
    self.assertTrue(tokenize_cpp.has_header_extension('a/b.h'))
    self.assertTrue(tokenize_cpp.has_header_extension('/a/b.hxx'))
    self.assertTrue(tokenize_cpp.has_header_extension('b.hpp'))

  @unittest.mock.patch('glob.glob')
  def test_find_same_base_files(self, mock_glob: unittest.mock.Mock):
    mock_glob.return_value = ['foo.bar', 'bar.baz', 'baz.foo']
    self.assertListEqual(
        tokenize_cpp.find_same_base_files('bar.baz'), ['foo.bar', 'baz.foo'])
    mock_glob.assert_called_with('bar.*')

    mock_glob.return_value = []
    self.assertListEqual(tokenize_cpp.find_same_base_files('baz.bar'), [])
    mock_glob.assert_called_with('baz.*')

  @unittest.mock.patch('glob.glob')
  def test_find_source_paths(self, mock_glob: unittest.mock.Mock):
    mock_glob.return_value = ['foo.cc', 'foo.h', 'foo.inc', 'foo.cpp']
    self.assertListEqual(
        tokenize_cpp.find_source_paths('path/foo.h'), ['foo.cc', 'foo.cpp'])
    mock_glob.assert_called_with('path/foo.*')

    # Returns the passed in path if it is a source path, doesn't use glob.
    mock_glob.side_effect = Exception('Boom')
    self.assertListEqual(
        tokenize_cpp.find_source_paths('path/foo.cc'), ['path/foo.cc'])

  @unittest.mock.patch('glob.glob')
  def test_find_header_paths(self, mock_glob: unittest.mock.Mock):
    mock_glob.return_value = ['foo.cc', 'foo.h', 'foo.inc', 'foo.cpp']
    self.assertListEqual(tokenize_cpp.find_header_paths('/foo.cpp'), ['foo.h'])
    mock_glob.assert_called_with('/foo.*')

    # Returns the passed in path if it is a header path, doesn't use glob.
    mock_glob.side_effect = Exception('Boom')
    self.assertListEqual(
        tokenize_cpp.find_header_paths('path/foo.h'), ['path/foo.h'])


if __name__ == '__main__':
  absltest.main()
