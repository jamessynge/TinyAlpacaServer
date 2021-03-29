#include "utils/literal.h"

#include <cstring>

#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "utils/string_compare.h"
#include "utils/string_view.h"

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
constexpr char kLowerHexEscaped[] PROGMEM =
    "\"some\\\\thing\\twith\\r\\n\\x08\\x0c\\\"quotes\\\".\"";

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
  StringView view(str);
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
  StringView view(str);
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
  PrintToStdString out;
  EXPECT_EQ(literal.printTo(out), literal.size());
  EXPECT_EQ(out.str(), kMixedStr);
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

TEST(LiteralTest, StreamHexEscaped) {
  Literal literal(kLowerStr);
  std::ostringstream oss;
  oss << HexEscaped(literal);
  EXPECT_EQ(oss.str(), kLowerHexEscaped);
}

}  // namespace
}  // namespace alpaca
