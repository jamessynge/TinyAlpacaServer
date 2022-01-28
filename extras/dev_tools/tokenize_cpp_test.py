"""Tests for tokenize_cpp."""

from typing import List, Optional
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

STRING_PREFIXES = ('', 'L', 'u8', 'u', 'U')
INT_SIZE_SUFFIXES = ('', 'l', 'L', 'll', 'LL', 'z', 'Z')
INT_SIGN_SUFFIXES = ('', 'U')
FP_TYPE_SUFFIXES = ('', 'f', 'F', 'l', 'L')


def make_token(kind: EToken, src: str, raw_start=0, raw_src=''):
  if not raw_src:
    raw_src = src
  return tokenize_cpp.Token(
      kind=kind,
      src=src,
      raw_src=raw_src,
      raw_start=raw_start,
      raw_end=raw_start + len(raw_src))


def make_expected_string_token(src, raw_start=0, raw_src=''):
  return make_token(EToken.STRING, src, raw_start=raw_start, raw_src=raw_src)


def make_expected_raw_string_token(raw_src, raw_start=0):
  return make_token(
      EToken.RAW_STRING, raw_src, raw_start=raw_start, raw_src=raw_src)


class TokenizeCppBaseTest(absltest.TestCase):

  def __init__(self, *args, **kwargs):
    super().__init__(*args, **kwargs)
    self.verbose = False

  def assertTokenizedAs(self,
                        raw_src: str,
                        kind: EToken,
                        prefix: str = '',
                        suffix: str = '',
                        src: Optional[str] = None,
                        file_src: Optional[str] = None,
                        source_only=True):
    if self.verbose:
      print('raw_src:', raw_src, f'    as py string: {raw_src!r}', flush=True)
    if not src:
      src = raw_src
    if file_src:
      raw_start = file_src.find(raw_src)
      self.assertGreaterEqual(raw_start, 0)
      self.assertEqual(prefix, '')
      self.assertEqual(suffix, '')
    else:
      file_src = f'{prefix}{raw_src}{suffix}'
      raw_start = len(prefix)
    expected_token = make_token(kind, src, raw_src=raw_src, raw_start=raw_start)
    if self.verbose:
      print(f'expected_token: {expected_token!r}', flush=True)
    try:
      tokenization = list(
          tokenize_cpp.generate_cpp_tokens(
              file_src=file_src, source_only=source_only))
    except Exception as ex:
      if ex.args:
        msg = f'{ex.args[0]}\nfile_src: {file_src!r}'
        ex.args = (msg,) + ex.args[1:]
      raise
    if self.verbose:
      print(tokenization, flush=True)
    self.assertSequenceEqual(tokenization, [expected_token])

  def assertHasNoTokens(self,
                        raw_src: str,
                        prefix: str = '',
                        suffix: str = '',
                        source_only=True):
    if self.verbose:
      print('raw_src:', raw_src, f'    as py string: {raw_src!r}', flush=True)
    file_src = f'{prefix}{raw_src}{suffix}'
    tokenization = list(
        tokenize_cpp.generate_cpp_tokens(
            file_src=file_src, source_only=source_only))
    self.assertSequenceEqual(tokenization, [], f'file_src: {file_src!r}')

  def assertTokenizeFails(self,
                          raw_src: str,
                          prefix: str = '',
                          suffix: str = '',
                          source_only=True) -> None:
    if self.verbose:
      print('raw_src:', raw_src, f'    as py string: {raw_src!r}', flush=True)
    file_src = f'{prefix}{raw_src}{suffix}'
    try:
      tokenization = list(
          tokenize_cpp.generate_cpp_tokens(
              file_src=file_src, source_only=source_only))
      print(tokenization, flush=True)
      self.fail(f'Should not have been able to tokenize {file_src!r}\n'
                f'As: {tokenization}')
    except ValueError as e:
      if self.verbose:
        print(e.args[0], flush=True)


class TokenizeIgnoresNonTokensTest(TokenizeCppBaseTest):

  def test_skips_whitespace(self):
    tokenization = list(
        tokenize_cpp.generate_cpp_tokens(file_src=' \r\n\t\v\f '))
    self.assertEmpty(tokenization)

  def test_skips_attributes(self):
    tokenization: List[tokenize_cpp.TokenOrGroup] = list(
        tokenize_cpp.generate_cpp_tokens(file_src="""
0
1 [[attr]]
2 [[attr1,attr2,attr3(arg)]]
3 [[attr_ns::attr(arg1, arg2)]]
4 [[ using X: attr ]]
5 [[using X:attr1, attr2]]
6
        """))
    self.assertSequenceEqual([x.kind for x in tokenization],
                             [EToken.INTEGER] * len(tokenization), tokenization)
    self.assertSequenceEqual([x.src for x in tokenization],
                             [str(i) for i in range(len(tokenization))],
                             tokenization)


class TokenizePreprocessorDirectiveTest(TokenizeCppBaseTest):

  def test_matches_preprocessor_directive(self):
    for prefix in ['', '\n', ' \n']:
      for suffix in ['', '\n', '\n ']:
        self.assertTokenizedAs(
            '#else',
            EToken.PREPROCESSOR_DIRECTIVE,
            prefix=prefix,
            suffix=suffix,
            source_only=False)
        self.assertTokenizedAs(
            ' #\telse',
            EToken.PREPROCESSOR_DIRECTIVE,
            prefix=prefix,
            suffix=suffix,
            source_only=False)
        self.assertTokenizedAs(
            '\t# else',
            EToken.PREPROCESSOR_DIRECTIVE,
            prefix=prefix,
            suffix=suffix,
            source_only=False)
        self.assertHasNoTokens(
            '#else', prefix=prefix, suffix=suffix, source_only=True)

  def test_optionally_skips_preprocessor_directive(self):
    tokenization: List[tokenize_cpp.TokenOrGroup] = list(
        tokenize_cpp.generate_cpp_tokens(file_src="""
0
# if 1
1
 #elif 2
2
#else
3
 # endif
4
        """))
    self.assertSequenceEqual([x.kind for x in tokenization],
                             [EToken.INTEGER] * len(tokenization), tokenization)
    self.assertSequenceEqual([x.src for x in tokenization],
                             [str(i) for i in range(len(tokenization))],
                             tokenization)

  def test_rejects_invalid_preprocessor_directive(self):
    self.assertTokenizeFails('a #')
    self.assertTokenizeFails('#')


class TokenizeCommentsTest(TokenizeCppBaseTest):

  def assertCommentTokenizedOrSkipped(self, file_src, src):
    if self.verbose:
      print(f'file_src: {file_src!r}')
      print(f'src: {src!r}', flush=True)
    self.assertTokenizedAs(
        src, EToken.COMMENT, file_src=file_src, source_only=False)
    self.assertHasNoTokens(file_src, source_only=True)

  def test_matches_line_comment(self):
    for prefix in ['', '\n', ' ', '\t']:
      for content in ['', ' ', 'a', '\ta ', ' a b /* */', '////']:
        comment = f'//{content}'
        self.assertCommentTokenizedOrSkipped(f'{prefix}{comment}', comment)
        self.assertCommentTokenizedOrSkipped(f'{prefix}{comment}\n\n', comment)

  def test_matches_multi_line_comment(self):
    for prefix in ['', '\n', ' ', '\t']:
      for suffix in ['', '\n', '\n ', ' ']:
        for content in ['', ' ', 'a', ' //\n// ', '*\n *\n ', ' /* ']:
          comment = f'/*{content}*/'
          self.assertCommentTokenizedOrSkipped(f'{prefix}{comment}{suffix}',
                                               comment)

  def test_rejects_unterminated_multi_line_comment(self):
    self.assertTokenizeFails('/*')
    self.assertTokenizeFails('/* abc')
    self.assertTokenizeFails('/* *')


class TokenizeStringLiteralsTest(TokenizeCppBaseTest):

  def test_empty_string_literals(self):
    for prefix in STRING_PREFIXES:
      s = prefix + '""'
      src = s
      tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
      # print(tokenization, flush=True)
      self.assertSequenceEqual(tokenization, [make_expected_string_token(s)])

  def test_non_empty_string_literals(self):
    for prefix in STRING_PREFIXES:
      s = prefix + '"abc"'
      src = '\n' + s + '\t'
      tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
      # print(tokenization, flush=True)
      self.assertSequenceEqual(tokenization,
                               [make_expected_string_token(s, raw_start=1)])

  def test_string_literals_with_continuation(self):
    for prefix in STRING_PREFIXES:
      raw = prefix + '"a\\\nb\\\nc"'
      s = prefix + '"abc"'
      src = '\n' + raw + '\t'
      tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
      # print(tokenization, flush=True)
      self.assertSequenceEqual(
          tokenization,
          [make_expected_string_token(s, raw_start=1, raw_src=raw)])

  def test_rejects_unterminated_string_literal(self):
    for prefix in STRING_PREFIXES:
      for string_content in ['', 'abc', '\\', '\\\\', r'abc\"']:
        self.assertTokenizeFails(f'{prefix}"{string_content}', suffix='')


class TokenizeRawStringLiteralsTest(TokenizeCppBaseTest):

  def test_tokenizes_empty_literal(self):
    for prefix in STRING_PREFIXES:
      for delimiter in ['', '1234567890123456', '"']:
        s = f'{prefix}R"{delimiter}(){delimiter}"'
        src = ' ' + s
        tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
        # print(tokenization, flush=True)
        self.assertSequenceEqual(
            tokenization, [make_expected_raw_string_token(s, raw_start=1)])

  def test_tokenizes_non_empty_literal(self):
    inner_string = 'Some text with "" (quotes) and with () (parentheses)'
    for prefix in STRING_PREFIXES:
      for delimiter in ['', '1234567890123456', '"']:
        s = f'{prefix}R"{delimiter}({inner_string}){delimiter}"'
        src = s + ' '
        tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
        # print(tokenization, flush=True)
        self.assertSequenceEqual(tokenization,
                                 [make_expected_raw_string_token(s)])

  def test_tokenizes_literal_with_continuation(self):
    continuations = '\\\n\\\n'
    inner_string = 'Some text with a continued \\\n line'
    for prefix in STRING_PREFIXES:
      for delimiter in ['', '1234567890123456', '"']:
        s = f'{prefix}R"{delimiter}({inner_string}){delimiter}"'
        src = continuations + s
        expected = make_expected_raw_string_token(
            s, raw_start=len(continuations))
        # print(expected, flush=True)
        tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
        # print(tokenization, flush=True)
        self.assertSequenceEqual(tokenization, [expected])

  def test_rejects_invalid_raw_string_literal(self):
    for open_raw_str in [f'{prefix}R"' for prefix in STRING_PREFIXES]:
      # Not a valid start to the string.
      for string_content in ['', 'a', '\n', ('a' * 17) + '(', ')', r'"']:
        self.assertTokenizeFails(f'{open_raw_str}"{string_content}', suffix='')

      # Not a valid end to the string
      for string_content in ['', ' ', '\n', ')', ')def"', ')abcd"']:
        self.assertTokenizeFails(f'{open_raw_str}"({string_content}', suffix='')
        self.assertTokenizeFails(
            f'{open_raw_str}"(xyz{string_content}', suffix='')
      for string_content in ['', ' ', ')"', '\n', ')', ')def"', ')abcd"']:
        self.assertTokenizeFails(
            f'{open_raw_str}"abc({string_content}', suffix='')
        self.assertTokenizeFails(
            f'{open_raw_str}"abc(xyz{string_content}', suffix='')


class TokenizeCharactersTest(TokenizeCppBaseTest):

  def test_char_literal(self):
    for prefix in STRING_PREFIXES:
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
    verbose = False
    short_esq = '\\'  # esq = escaped single quoted character
    long_esq = '\\' + ' ' * 30 + "'"
    unterminated_esq = '\\   '
    for prefix in STRING_PREFIXES:
      for contents in [
          "a '", "'", "\n'", r"\? '", short_esq, long_esq, unterminated_esq
      ]:
        s = f"{prefix}'{contents}"
        if verbose:
          print('\ns:', s, f'    rs: {s!r}', flush=True)
        try:
          tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=s))
          print(tokenization, flush=True)
          self.fail('Should not reach here!')
        except ValueError as e:
          if verbose:
            print(e.args[0], flush=True)


class TokenizeNumbersTest(TokenizeCppBaseTest):
  # Note that the tokenizer doesn't convert numeric literals into integers or
  # floating point values, just identifies their span within the text.

  def test_tokenizes_floating_point_literals(self):
    numbers = ['0', '1', '9', '0123', '987']
    exponents = ['']
    for indicator in ('e', 'E'):
      for sign in ('', '-', '+'):
        for i in numbers:
          exponents.append(f'{indicator}{sign}{i}')

    for whole_number in [''] + numbers + [f'{n}.' for n in numbers]:
      for fractional_number in [''] + numbers:
        if not whole_number and not fractional_number:
          continue
        number = whole_number + '.' + fractional_number
        number = number.replace('..', '.')
        for exponent in exponents:
          for type_suffix in FP_TYPE_SUFFIXES:
            s = f'{number}{exponent}{type_suffix}'
            if s == '0':
              continue
            self.assertTokenizedAs(s, EToken.FP_NUMBER)

  def test_rejects_floating_point_literals(self):
    self.assertTokenizeFails('.0E')
    self.assertTokenizeFails('0E')
    self.assertTokenizeFails('0E+')
    self.assertTokenizeFails('0E-')
    self.assertTokenizeFails('0.EF')

  def test_integers(self):

    def check_decimal(i, size, sign):
      s = f'{i:d}{size}{sign}'
      self.assertTokenizedAs(s, EToken.INTEGER)

    def check_octal(i, size, sign):
      s = f'{i:o}{size}{sign}'
      if i != 0:
        s = '0' + s
      self.assertTokenizedAs(s, EToken.INTEGER)

    def check_hex(i, size, sign):
      s = f'{i:x}{size}{sign}'
      self.assertTokenizedAs('0x' + s, EToken.INTEGER)
      self.assertTokenizedAs('0X' + s, EToken.INTEGER)
      s = f'{i:X}{size}{sign}'
      self.assertTokenizedAs('0x' + s, EToken.INTEGER)
      self.assertTokenizedAs('0X' + s, EToken.INTEGER)

    def check_binary(i, size, sign):
      s = f'{i:b}{size}{sign}'
      self.assertTokenizedAs('0b' + s, EToken.INTEGER)
      self.assertTokenizedAs('0B' + s, EToken.INTEGER)

    # Checking various values that are appropriate for each base in which
    # integers can be represented.

    for i in list(range(1, 20)) + list(range(20, 101, 10)):
      for size_suffix in INT_SIZE_SUFFIXES:
        for sign_suffix in INT_SIGN_SUFFIXES:
          check_decimal(i, size_suffix, sign_suffix)

    for i in list(range(8)) + list(range(8, 70, 3)):
      for size_suffix in INT_SIZE_SUFFIXES:
        for sign_suffix in INT_SIGN_SUFFIXES:
          check_octal(i, size_suffix, sign_suffix)

    for i in list(range(16)) + list(range(16, 260, 7)) + [255, 256]:
      for size_suffix in INT_SIZE_SUFFIXES:
        for sign_suffix in INT_SIGN_SUFFIXES:
          check_hex(i, size_suffix, sign_suffix)

    for i in range(8):
      for size_suffix in INT_SIZE_SUFFIXES:
        for sign_suffix in INT_SIGN_SUFFIXES:
          check_binary(i, size_suffix, sign_suffix)

  def test_invalid_integer(self):
    """Confirm that malformed integers are rejected."""
    self.verbose = False

    self.assertTokenizeFails('08')
    self.assertTokenizeFails('0128')
    self.assertTokenizeFails('0f')
    self.assertTokenizeFails('9f')
    self.assertTokenizeFails('1x')
    self.assertTokenizeFails('0b2')
    self.assertTokenizeFails('0b012')
    self.assertTokenizeFails('0b01a')
    self.assertTokenizeFails('0xg')
    self.assertTokenizeFails('0x1g')


class CppSourceTest(TokenizeCppBaseTest):

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


class PathHandlingTest(TokenizeCppBaseTest):

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
