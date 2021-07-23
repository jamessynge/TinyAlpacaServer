#include "utils/hex_escape.h"

#include <stddef.h>

#include <cstdio>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "extras/test_tools/print_to_std_string.h"
#include "extras/test_tools/sample_printable.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "util/gtl/map_util.h"
#include "utils/literal.h"
#include "utils/string_view.h"

namespace alpaca {
namespace test {
namespace {

using ::testing::Contains;
using ::testing::IsEmpty;
using ::testing::Not;
using ::testing::SizeIs;
using ::testing::StartsWith;

TEST(PrintCharHexEscapedTest, TestAll) {
  std::vector<std::pair<char, std::string>> test_cases = {
      // clang-format off
      {'\n', "\\n"},
      {'\x0a', "\\n"},

      {'\r', "\\r"},
      {'\015', "\\r"},

      {'\t', "\\t"},
      {'\011', "\\t"},
      {'\x09', "\\t"},

      {'\\', "\\\\"},
      {'"', "\\\""},
      // clang-format on
  };
  std::set<char> tested;
  for (auto [c, expected] : test_cases) {
    tested.insert(c);
    PrintToStdString out;
    PrintCharHexEscaped(out, c);
    EXPECT_EQ(out.str(), expected);
  }
  for (char c = ' '; c < 127; ++c) {
    if (c == '"' || c == '\\') {
      EXPECT_THAT(tested, Contains(c));
    } else {
      EXPECT_THAT(tested, Not(Contains(c)));
      tested.insert(c);
      PrintToStdString out;
      PrintCharHexEscaped(out, c);
      EXPECT_THAT(out.str(), SizeIs(1));
      EXPECT_EQ(out.str().at(0), c);
    }
  }
  for (int i = -128; i <= 127; ++i) {
    char c = static_cast<char>(i);
    if (!gtl::ContainsKey(tested, c)) {
      tested.insert(c);
      PrintToStdString out;
      PrintCharHexEscaped(out, c);
      EXPECT_THAT(out.str(), SizeIs(4));
      char buffer[16];
      std::string_view expected(
          buffer, std::snprintf(buffer, sizeof buffer, "\\x%02X",
                                static_cast<unsigned int>(c + 0)));
      EXPECT_THAT(out.str(), expected);
    }
  }
  EXPECT_THAT(tested, SizeIs(256));
}

TEST(PrintHexEscapedTest, EmptyString) {
  PrintToStdString inner;
  PrintHexEscaped outer(inner);

  outer.print("");
  EXPECT_THAT(inner.str(), IsEmpty());
}

constexpr char kStringLiteral[] =
    "<tag attr=\"value with slash ('\\')\">\b\f\n\r\t</tag>";
constexpr char kEscapedLiteral[] =
    "<tag attr=\\\"value with slash ('\\\\')\\\">"
    "\\x08\\x0C\\n\\r\\t</tag>";
constexpr char kQuotedEscapedLiteral[] =
    "\"<tag attr=\\\"value with slash ('\\\\')\\\">"
    "\\x08\\x0C\\n\\r\\t</tag>\"";

TEST(PrintHexEscapedTest, CLiteralStringWithEscapes) {
  PrintToStdString inner;
  PrintHexEscaped outer(inner);

  outer.print(kStringLiteral);
  EXPECT_EQ(inner.str(), kEscapedLiteral);
}

TEST(HexEscapedPrintableTest, PrintableWithEscapes) {
  SamplePrintable original("abc\r\n");
  auto printable = HexEscapedPrintable<SamplePrintable>(original);

  PrintToStdString out;
  const size_t count = printable.printTo(out);

  const std::string expected = "\"abc\\r\\n\"";
  EXPECT_EQ(out.str(), expected);
  EXPECT_EQ(count, expected.size());
}

TEST(HexEscapedPrintableTest, StringViewWithEscapes) {
  StringView original(kStringLiteral);
  auto printable = HexEscapedPrintable<StringView>(original);

  PrintToStdString out;
  const size_t count = printable.printTo(out);

  const std::string expected = kQuotedEscapedLiteral;
  EXPECT_EQ(out.str(), expected);
  EXPECT_EQ(count, expected.size());
}

TEST(HexEscapedPrintableTest, LiteralWithEscapes) {
  Literal original(kStringLiteral);
  auto printable = HexEscapedPrintable<Literal>(original);

  PrintToStdString out;
  const size_t count = printable.printTo(out);

  const std::string expected = kQuotedEscapedLiteral;
  EXPECT_EQ(out.str(), expected);
  EXPECT_EQ(count, expected.size());
}

TEST(HexEscapedTest, PrintableWithEscapes) {
  SamplePrintable original("abc\r\n");
  auto printable = HexEscaped(original);

  PrintToStdString out;
  const size_t count = out.print(printable);

  const std::string expected = "\"abc\\r\\n\"";
  EXPECT_EQ(out.str(), expected);
  EXPECT_EQ(count, expected.size());
}

TEST(HexEscapedTest, LiteralWithEscapes) {
  Literal literal(kStringLiteral);
  auto printable = HexEscaped(literal);

  PrintToStdString out;
  const size_t count = out.print(printable);

  const std::string expected = kQuotedEscapedLiteral;
  EXPECT_EQ(out.str(), expected);
  EXPECT_EQ(count, expected.size());
}

}  // namespace
}  // namespace test
}  // namespace alpaca
