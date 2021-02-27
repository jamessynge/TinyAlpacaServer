#include "literal.h"

#include <cstring>

#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "extras/tests/test_utils.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace {
using ::testing::IsEmpty;

constexpr char kLowerStr[] PROGMEM = "some\\thing\twith\r\n\b\f\"quotes\".";
constexpr char kMixedStr[] PROGMEM = "Some\\thing\tWith\r\n\b\f\"Quotes\".";
constexpr char kUpperStr[] PROGMEM = "SOME\\THING\tWITH\r\n\b\f\"QUOTES\".";

constexpr StringView kLowerView(kLowerStr);
constexpr StringView kMixedView(kMixedStr);
constexpr StringView kUpperView(kUpperStr);

constexpr char kLowerJson[] PROGMEM =
    "\"some\\\\thing\\twith\\r\\n\\b\\f\\\"quotes\\\".\"";

TEST(LiteralTest, LowerComparison) {
  Literal literal(kLowerStr);
  EXPECT_EQ(literal.size(), std::strlen(kLowerStr));
  EXPECT_EQ(literal.size(), kLowerView.size());
  EXPECT_EQ(literal, kLowerView);

  EXPECT_NE(literal, kMixedView);
  EXPECT_NE(literal, kUpperView);

  // Make a copy so that we know that operator== isn't just comparing pointers.
  std::string str(kLowerStr);
  StringView view(str);
  EXPECT_EQ(literal, view);

  // This prefix will share the same pointer, but not the same length.
  EXPECT_NE(literal, kLowerView.prefix(8));

  // Case-insensitively equal to all of the variants.
  EXPECT_TRUE(literal.case_equal(view));
  EXPECT_TRUE(literal.case_equal(kLowerView));
  EXPECT_TRUE(literal.case_equal(kMixedView));
  EXPECT_TRUE(literal.case_equal(kUpperView));

  // Not case-insensitively equal to an empty string, nor to prefixes of itself.
  EXPECT_FALSE(literal.case_equal(""));
  EXPECT_FALSE(literal.case_equal(kLowerView.prefix(1)));
  EXPECT_FALSE(literal.case_equal(kLowerView.prefix(kLowerView.size() - 1)));

  // Equal to kLowerView if we lower-case the literal, but not equal to the
  // other variants.
  EXPECT_TRUE(literal.lowered_equal(kLowerView));
  EXPECT_FALSE(literal.lowered_equal(kMixedView));
  EXPECT_FALSE(literal.lowered_equal(kUpperView));
  EXPECT_FALSE(literal.lowered_equal(""));

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
  StringView view(str);
  EXPECT_EQ(literal, view);

  // This prefix will share the same pointer, but not the same length.
  EXPECT_NE(literal, kMixedView.prefix(8));

  // Case-insensitively equal to all of the variants.
  EXPECT_TRUE(literal.case_equal(view));
  EXPECT_TRUE(literal.case_equal(kLowerView));
  EXPECT_TRUE(literal.case_equal(kMixedView));
  EXPECT_TRUE(literal.case_equal(kUpperView));

  // Not case-insensitively equal to an empty string, nor to prefixes of itself.
  EXPECT_FALSE(literal.case_equal(""));
  EXPECT_FALSE(literal.case_equal(kMixedView.prefix(1)));
  EXPECT_FALSE(literal.case_equal(kMixedView.prefix(kMixedView.size() - 1)));

  // Equal to kLowerView if we lower-case the literal, but not equal to the
  // other variants.
  EXPECT_TRUE(literal.lowered_equal(kLowerView));
  EXPECT_FALSE(literal.lowered_equal(kMixedView));
  EXPECT_FALSE(literal.lowered_equal(kUpperView));
  EXPECT_FALSE(literal.lowered_equal(""));

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
  StringView view(str);
  EXPECT_EQ(literal, view);

  // This prefix will share the same pointer, but not the same length.
  EXPECT_NE(literal, kUpperView.prefix(8));

  // Case-insensitively equal to all of the variants.
  EXPECT_TRUE(literal.case_equal(view));
  EXPECT_TRUE(literal.case_equal(kLowerView));
  EXPECT_TRUE(literal.case_equal(kMixedView));
  EXPECT_TRUE(literal.case_equal(kUpperView));

  // Not case-insensitively equal to an empty string, nor to prefixes of itself.
  EXPECT_FALSE(literal.case_equal(""));
  EXPECT_FALSE(literal.case_equal(kUpperView.prefix(1)));
  EXPECT_FALSE(literal.case_equal(kUpperView.prefix(kUpperView.size() - 1)));

  // Equal to kLowerView if we lower-case the literal, but not equal to the
  // other variants.
  EXPECT_TRUE(literal.lowered_equal(kLowerView));
  EXPECT_FALSE(literal.lowered_equal(kMixedView));
  EXPECT_FALSE(literal.lowered_equal(kUpperView));
  EXPECT_FALSE(literal.lowered_equal(""));

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
  PrintToString out;
  EXPECT_EQ(literal.printTo(out), literal.size());
  EXPECT_EQ(out.str(), kMixedStr);
}

TEST(LiteralTest, PrintJsonEscapedTo) {
  Literal literal(kLowerStr);
  const std::string expected(kLowerJson);
  PrintToString out;
  EXPECT_EQ(literal.printJsonEscapedTo(out), expected.size());
  EXPECT_EQ(out.str(), expected);
}

TEST(LiteralTest, StreamMixed) {
  // This tests features used only on the host.
  Literal literal(kMixedStr);
  std::ostringstream oss;
  oss << literal;
  EXPECT_EQ(oss.str(), kMixedStr);
}

TEST(LiteralTest, StreamUpper) {
  // This tests features used only on the host.
  Literal literal(kUpperStr);
  std::ostringstream oss;
  oss << literal;
  EXPECT_EQ(oss.str(), kUpperStr);
}

TEST(LiteralTest, StreamJson) {
  Literal literal(kLowerStr);
  std::ostringstream oss;
  oss << literal.escaped();
  EXPECT_EQ(oss.str(), kLowerJson);
}

}  // namespace
}  // namespace alpaca
