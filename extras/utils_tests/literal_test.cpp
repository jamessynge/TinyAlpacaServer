#include "utils/literal.h"

#include <cstring>
#include <iosfwd>
#include <string>

#include "absl/strings/string_view.h"
#include "extras/test_tools/literal_utils.h"
#include "extras/test_tools/string_view_utils.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mcucore/extrastest_tools/print_to_std_string.h"
#include "mcucore_platform.h"
#include "o_print_stream.h"
#include "utils/hex_escape.h"
#include "utils/string_compare.h"
#include "utils/string_view.h"

namespace alpaca {
namespace test {
namespace {
using ::testing::IsEmpty;

constexpr char kLowerStr[] AVR_PROGMEM = "some\\thing\twith\r\n\b\f\"quotes\".";
constexpr char kMixedStr[] AVR_PROGMEM = "Some\\thing\tWith\r\n\b\f\"Quotes\".";
constexpr char kUpperStr[] AVR_PROGMEM = "SOME\\THING\tWITH\r\n\b\f\"QUOTES\".";

constexpr StringView kLowerView(kLowerStr);
constexpr StringView kMixedView(kMixedStr);
constexpr StringView kUpperView(kUpperStr);

constexpr char kLowerJson[] AVR_PROGMEM =
    "\"some\\\\thing\\twith\\r\\n\\b\\f\\\"quotes\\\".\"";
constexpr char kLowerHexEscaped[] AVR_PROGMEM =
    "\"some\\\\thing\\twith\\r\\n\\x08\\x0C\\\"quotes\\\".\"";
constexpr char kUpperHexEscaped[] AVR_PROGMEM =
    "\"SOME\\\\THING\\tWITH\\r\\n\\x08\\x0C\\\"QUOTES\\\".\"";

TEST(LiteralTest, LowerComparison) {
  Literal literal(kLowerStr);
  EXPECT_EQ(literal.size(), std::strlen(kLowerStr));
  EXPECT_EQ(literal.size(), kLowerView.size());
  EXPECT_EQ(literal, kLowerView);

  EXPECT_NE(literal, kMixedView);
  EXPECT_NE(literal, kUpperView);

  // Make a copy so that we know that operator== isn't just comparing pointers.
  std::string str(kLowerStr);
  StringView view = MakeStringView(str);
  EXPECT_EQ(literal, view);

  // This prefix will share the same pointer, but not the same length.
  EXPECT_NE(literal, kLowerView.prefix(8));

  // Case-insensitively equal to all of the variants.
  EXPECT_TRUE(CaseEqual(literal, view));
  EXPECT_TRUE(CaseEqual(literal, kLowerView));
  EXPECT_TRUE(CaseEqual(literal, kMixedView));
  EXPECT_TRUE(CaseEqual(literal, kUpperView));

  // Not case-insensitively equal to an empty string, nor to prefixes of itself.
  EXPECT_FALSE(CaseEqual(literal, StringView()));
  EXPECT_FALSE(CaseEqual(literal, StringView("")));
  EXPECT_FALSE(CaseEqual(literal, kLowerView.prefix(1)));
  EXPECT_FALSE(CaseEqual(literal, kLowerView.prefix(kLowerView.size() - 1)));

  // at() will return the appropriate character.
  EXPECT_EQ(literal.at(0), 's');
  EXPECT_EQ(literal.at(literal.size() - 1), '.');
}

TEST(LiteralTest, MixedComparison) {
  Literal literal(kMixedStr);
  EXPECT_EQ(literal.size(), std::strlen(kMixedStr));
  EXPECT_EQ(literal.size(), kMixedView.size());
  EXPECT_EQ(literal, kMixedView);

  EXPECT_NE(literal, kLowerView);
  EXPECT_NE(literal, kUpperView);

  // Make a copy so that we know that operator== isn't just comparing pointers.
  std::string str(kMixedStr);
  StringView view = MakeStringView(str);
  EXPECT_EQ(literal, view);

  // This prefix will share the same pointer, but not the same length.
  EXPECT_NE(literal, kMixedView.prefix(8));

  // Case-insensitively equal to all of the variants.
  EXPECT_TRUE(CaseEqual(literal, view));
  EXPECT_TRUE(CaseEqual(literal, kLowerView));
  EXPECT_TRUE(CaseEqual(literal, kMixedView));
  EXPECT_TRUE(CaseEqual(literal, kUpperView));

  // Not case-insensitively equal to an empty string, nor to prefixes of itself.
  EXPECT_FALSE(CaseEqual(literal, StringView("")));
  EXPECT_FALSE(CaseEqual(literal, StringView()));
  EXPECT_FALSE(CaseEqual(literal, kMixedView.prefix(1)));
  EXPECT_FALSE(CaseEqual(literal, kMixedView.prefix(kMixedView.size() - 1)));

  // at() will return the appropriate character.
  EXPECT_EQ(literal.at(0), 'S');
  EXPECT_EQ(literal.at(literal.size() - 1), '.');
}

TEST(LiteralTest, UpperComparison) {
  Literal literal(kUpperStr);
  EXPECT_EQ(literal.size(), std::strlen(kUpperStr));
  EXPECT_EQ(literal.size(), kUpperView.size());
  EXPECT_EQ(literal, kUpperView);

  EXPECT_NE(literal, kMixedView);
  EXPECT_NE(literal, kLowerView);

  // Make a copy so that we know that operator== isn't just comparing pointers.
  std::string str(kUpperStr);
  StringView view = MakeStringView(str);
  EXPECT_EQ(literal, view);

  // This prefix will share the same pointer, but not the same length.
  EXPECT_NE(literal, kUpperView.prefix(8));

  // Case-insensitively equal to all of the variants.
  EXPECT_TRUE(CaseEqual(literal, view));
  EXPECT_TRUE(CaseEqual(literal, kLowerView));
  EXPECT_TRUE(CaseEqual(literal, kMixedView));
  EXPECT_TRUE(CaseEqual(literal, kUpperView));

  // Not case-insensitively equal to an empty string, nor to prefixes of itself.
  EXPECT_FALSE(CaseEqual(literal, StringView("")));
  EXPECT_FALSE(CaseEqual(literal, kUpperView.prefix(1)));
  EXPECT_FALSE(CaseEqual(literal, kUpperView.prefix(kUpperView.size() - 1)));

  // at() will return the appropriate character.
  EXPECT_EQ(literal.at(0), 'S');
  EXPECT_EQ(literal.at(literal.size() - 1), '.');
}

TEST(LiteralTest, Copy) {
  Literal literal(kMixedStr);
  EXPECT_EQ(literal.size(), sizeof(kMixedStr) - 1);

  // Make a buffer that is one byte bigger than needed for copyTo so that we can
  // add a NUL at the end.
  char buffer[sizeof kMixedStr] = "";
  buffer[kMixedView.size()] = 0;
  EXPECT_EQ(literal.size() + 1, sizeof buffer);
  EXPECT_THAT(absl::string_view(buffer), IsEmpty());

  // Can't copy if the destination size is too small.
  EXPECT_FALSE(literal.copyTo(buffer, 0));
  EXPECT_THAT(absl::string_view(buffer), IsEmpty());
  EXPECT_FALSE(literal.copyTo(buffer, literal.size() - 1));
  EXPECT_THAT(absl::string_view(buffer), IsEmpty());

  // And can copy if the destination is the right size or larger.
  EXPECT_TRUE(literal.copyTo(buffer, literal.size()));
  EXPECT_EQ(absl::string_view(buffer), kMixedStr);
  EXPECT_TRUE(literal.copyTo(buffer, sizeof buffer));
  EXPECT_EQ(absl::string_view(buffer), kMixedStr);
}

TEST(LiteralTest, PrintTo) {
  Literal literal(kMixedStr);
  mcucore::test::PrintToStdString out;
  EXPECT_EQ(literal.printTo(out), literal.size());
  EXPECT_EQ(out.str(), kMixedStr);
}

TEST(LiteralTest, StreamMixed) {
  Literal literal(kMixedStr);

  mcucore::test::PrintToStdString p2ss;
  OPrintStream out(p2ss);
  out << literal;
  EXPECT_EQ(p2ss.str(), kMixedStr);

  std::ostringstream oss;
  oss << literal;
  EXPECT_EQ(oss.str(), kMixedStr);
}

TEST(LiteralTest, StreamUpper) {
  Literal literal(kUpperStr);

  mcucore::test::PrintToStdString p2ss;
  OPrintStream out(p2ss);
  out << literal;
  EXPECT_EQ(p2ss.str(), kUpperStr);

  std::ostringstream oss;
  oss << literal;
  EXPECT_EQ(oss.str(), kUpperStr);
}

TEST(LiteralTest, StreamHexEscaped) {
  Literal literal(kLowerStr);
  mcucore::test::PrintToStdString p2ss;
  OPrintStream out(p2ss);
  out << HexEscaped(literal);
  EXPECT_EQ(p2ss.str(), kLowerHexEscaped);
}

TEST(LiteralTest, Equality) {
  Literal literal1("abc");
  EXPECT_EQ(literal1, literal1);
  EXPECT_TRUE(literal1.same(literal1));

  std::string abc("abc");
  Literal literal2(abc.data(), abc.size());
  EXPECT_EQ(literal1, literal2);
  EXPECT_FALSE(literal1.same(literal2));

  Literal literal3(abc.data(), abc.size());
  EXPECT_EQ(literal1, literal3);
  EXPECT_EQ(literal2, literal3);
  EXPECT_TRUE(literal2.same(literal3));
}

TEST(LiteralTest, Inequality) {
  Literal literal1("aBC");
  Literal literal2("abc");
  EXPECT_NE(literal1, literal2);

  Literal literal3("");
  EXPECT_NE(literal1, literal3);
}

}  // namespace
}  // namespace test
}  // namespace alpaca
