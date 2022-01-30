"""Tests for cpp_enum."""

import json
import pprint
from typing import Any, Dict, List, Optional
# import unittest.mock

from absl import flags

import cpp_enum
import tokenize_cpp
import absltest

EToken = tokenize_cpp.EToken
FLAGS = flags.FLAGS

flags.DEFINE_string(
    name='vmodule',
    required=False,
    default='',
    help='Ignored; defined just to be ignored if provided to all tests.')
FAKE_TOKEN = cpp_enum.Token(EToken.IDENTIFIER, 'NoValue', 'NoValue', 0, 7)
FAKE_VALUE = [FAKE_TOKEN]


def get_enum_defs(file_src: str) -> List[cpp_enum.EnumerationDefinition]:
  cpp_source = tokenize_cpp.CppSource(file_path='fake.h', raw_source=file_src)
  return cpp_enum.get_enumeration_definitions(cpp_source)


def make_enum_val(
    name,
    numeric_value: Optional[int] = None,
    print_name: Optional[str] = None) -> cpp_enum.EnumeratorDefinition:
  # FAKE_VALUE will suppress computing numeric_value if we haven't provided one.
  return cpp_enum.EnumeratorDefinition(
      name=name,
      print_name=print_name,
      value=FAKE_VALUE if numeric_value is None else [],
      numeric_value=numeric_value)


def make_enum_def(
    name: str,
    enumerators: List[cpp_enum.EnumeratorDefinition],
    is_scoped: bool = False,
    underlying_type: Optional[str] = None) -> cpp_enum.EnumerationDefinition:
  return cpp_enum.EnumerationDefinition(
      name=name,
      is_scoped=is_scoped,
      underlying_type=underlying_type,
      enumerators=enumerators)


def printable_enum_val(val: cpp_enum.EnumeratorDefinition) -> Dict[str, Any]:
  return dict([
      (k, v) for k, v in val.__dict__.items() if k != 'value' and v is not None
  ])


def printable_enum_def(e: cpp_enum.EnumerationDefinition) -> Dict[str, Any]:
  d = dict([(k, v)
            for k, v in e.__dict__.items()
            if 'enumerator' not in k and v is not None])
  d['enumerators'] = [printable_enum_val(v) for v in e.enumerators]
  if e.maximum_enumerator:
    d['maximum_enumerator'] = e.maximum_enumerator.name
  if e.minimum_enumerator:
    d['minimum_enumerator'] = e.minimum_enumerator.name
  return d


def pprint_enum_def(e: cpp_enum.EnumerationDefinition) -> str:
  p = printable_enum_def(e)
  s = json.dumps(p, indent=2)
  # print('-' * 80)
  # print(p)
  # print()
  # print(s)
  # print(flush=True)
  return s


class CppEnumTest(absltest.TestCase):

  def __init__(self, *args, **kwargs):
    super().__init__(*args, **kwargs)
    self.addTypeEqualityFunc(cpp_enum.EnumerationDefinition,
                             self.compare_enumeration)

  def compare_enumeration(self,
                          actual: cpp_enum.EnumerationDefinition,
                          expected: cpp_enum.EnumerationDefinition,
                          msg=None):
    self.assertMultiLineEqual(
        pprint_enum_def(actual), pprint_enum_def(expected), msg=msg)

  def test_finds_nothing(self):
    self.assertEqual(get_enum_defs(' '), [])
    self.assertEqual(
        get_enum_defs("""
class Foo;
int a;
enum X x;
std::vector<long>;
"""), [])

  def test_finds_enum_defs(self):
    enum_defs = get_enum_defs("""
class Foo;
enum X { kX };
int a;
enum class Y { y1, y2 =2, y3 };
enum X x;
enum class Z { a, b , c = a, d, e=7, f, g };
std::vector<long>;
""")
    self.assertEqual(len(enum_defs), 3)
    self.assertEqual(enum_defs[0],
                     make_enum_def('X', [make_enum_val('kX', numeric_value=0)]))
    self.assertEqual(
        enum_defs[1],
        make_enum_def(
            'Y', [
                make_enum_val('y1', numeric_value=0),
                make_enum_val('y2', numeric_value=2),
                make_enum_val('y3', numeric_value=3)
            ],
            is_scoped=True))
    self.assertEqual(
        enum_defs[2],
        make_enum_def(
            'Z', [
                make_enum_val('a', numeric_value=0),
                make_enum_val('b', numeric_value=1),
                make_enum_val('c'),
                make_enum_val('d'),
                make_enum_val('e', numeric_value=7),
                make_enum_val('f', numeric_value=8),
                make_enum_val('g', numeric_value=9)
            ],
            is_scoped=True))

  def test_captures_print_name(self):
    enum_defs = get_enum_defs("""
enum class EDef : uint8_t {
  TASENUMERATOR( Alpha, "abc" ) = a + b,
  TASENUMERATOR( Beta, "def" ) = 3,
  Gamma = 5,
  kDelta,
};
""")
    self.assertEqual(
        enum_defs[0],
        make_enum_def(
            'EDef', [
                make_enum_val('Alpha', print_name='"abc"'),
                make_enum_val('Beta', numeric_value=3, print_name='"def"'),
                make_enum_val('Gamma', numeric_value=5),
                make_enum_val('kDelta', numeric_value=6)
            ],
            is_scoped=True,
            underlying_type='uint8_t'))


if __name__ == '__main__':
  absltest.main()
