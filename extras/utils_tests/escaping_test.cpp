#include "utils/escaping.h"

#include <utility>
#include <vector>

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "util/gtl/map_util.h"
#include "utils/literal.h"
#include "utils/string_view.h"

namespace alpaca {
namespace {

using ::testing::Contains;
using ::testing::IsEmpty;
using ::testing::Not;
using ::testing::SizeIs;

TEST(PrintJsonEscapedTest, OneByteAtATime) {
  std::vector<std::pair<char, std::string>> test_cases = {
      // clang-format off
      {'\b', "\\b"},
      {'\010', "\\b"},

      {'\f', "\\f"},
      {'\x0c', "\\f"},

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
    PrintToStdString inner;
    PrintJsonEscaped outer(inner);
    outer.print(c);
    EXPECT_EQ(inner.str(), expected);
  }
  for (char c = ' '; c < 127; ++c) {
    if (c == '"' || c == '\\') {
      EXPECT_THAT(tested, Contains(c));
    } else {
      EXPECT_THAT(tested, Not(Contains(c)));
      tested.insert(c);
      PrintToStdString inner;
      PrintJsonEscaped outer(inner);
      outer.print(c);
      EXPECT_THAT(inner.str(), SizeIs(1));
      EXPECT_EQ(inner.str().at(0), c);
    }
  }
  for (int i = -128; i <= 127; ++i) {
    char c = static_cast<char>(i);
    if (!gtl::ContainsKey(tested, c)) {
      tested.insert(c);
      PrintToStdString inner;
      PrintJsonEscaped outer(inner);
      outer.print(c);
      EXPECT_THAT(inner.str(), IsEmpty());
    }
  }
  EXPECT_THAT(tested, SizeIs(256));
}

TEST(PrintJsonEscapedTest, EmptyString) {
  PrintToStdString inner;
  PrintJsonEscaped outer(inner);

  outer.print("");
  EXPECT_THAT(inner.str(), IsEmpty());
}

TEST(PrintJsonEscapedTest, CLiteralStringWithEscapes) {
  PrintToStdString inner;
  PrintJsonEscaped outer(inner);

  outer.print("<tag attr=\"value with slash ('\\')\">\b\f\n\r\t</tag>");
  EXPECT_EQ(inner.str(),
            "<tag attr=\\\"value with slash ('\\\\')\\\">"
            "\\b\\f\\n\\r\\t</tag>");
}

TEST(PrintJsonEscapedToTest, StringViewWithEscapes) {
  StringView view("<tag attr=\"value with slash ('\\')\">\b\f\n\r\t</tag>");

  PrintToStdString out;
  const size_t count = PrintJsonEscapedTo(view, out);
  const std::string expected =
      "<tag attr=\\\"value with slash ('\\\\')\\\">"
      "\\b\\f\\n\\r\\t</tag>";

  EXPECT_EQ(out.str(), expected);
  EXPECT_EQ(count, expected.size());
}

TEST(PrintJsonEscapedStringToTest, LiteralWithEscapes) {
  Literal literal("<tag attr=\"value with slash ('\\')\">\b\f\n\r\t</tag>");

  PrintToStdString out;
  const size_t count = PrintJsonEscapedStringTo(literal, out);
  const std::string expected =
      "\"<tag attr=\\\"value with slash ('\\\\')\\\">"
      "\\b\\f\\n\\r\\t</tag>\"";

  EXPECT_EQ(out.str(), expected);
  EXPECT_EQ(count, expected.size());
}

}  // namespace
}  // namespace alpaca
