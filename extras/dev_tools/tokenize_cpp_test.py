"""Tests for tokenize_cpp."""

from absl import flags

from google3.experimental.users.jamessynge.tiny_alpaca_server.extras.dev_tools import tokenize_cpp
from google3.testing.pybase import googletest

FLAGS = flags.FLAGS

flags.DEFINE_string(
    name='vmodule',
    required=False,
    default='',
    help='Ignored; defined just to be ignored if provided to all tests.')


class TokenizeCppTest(googletest.TestCase):

  STRING_PREFIXES = ['', 'L', 'u8', 'u', 'U']

  def test_empty_string_literals(self):
    for prefix in self.STRING_PREFIXES:
      s = prefix + '""'
      src = s
      tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
      print(tokenization, flush=True)
      self.assertSequenceEqual(tokenization, [(tokenize_cpp.EToken.STRING, s)])

  def test_non_empty_string_literals(self):
    for prefix in self.STRING_PREFIXES:
      s = prefix + '"abc"'
      src = '\n' + s + '\t'
      tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
      print(tokenization, flush=True)
      self.assertSequenceEqual(tokenization, [(tokenize_cpp.EToken.STRING, s)])

  def test_empty_raw_string_literals(self):
    for prefix in self.STRING_PREFIXES:
      for delimiter in ['', '1234567890123456', '"']:
        s = f'{prefix}R"{delimiter}(){delimiter}"'
        src = ' ' + s
        tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
        print(tokenization, flush=True)
        self.assertSequenceEqual(tokenization,
                                 [(tokenize_cpp.EToken.STRING, s)])

  def test_non_empty_raw_string_literals(self):
    inner_string = 'Some text with "" (quotes) and with () (parentheses)'
    for prefix in self.STRING_PREFIXES:
      for delimiter in ['', '1234567890123456', '"']:
        s = f'{prefix}R"{delimiter}({inner_string}){delimiter}"'
        src = s + ' '
        tokenization = list(tokenize_cpp.generate_cpp_tokens(file_src=src))
        print(tokenization, flush=True)
        self.assertSequenceEqual(tokenization,
                                 [(tokenize_cpp.EToken.STRING, s)])


if __name__ == '__main__':
  googletest.main()
