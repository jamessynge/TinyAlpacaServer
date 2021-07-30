#include "utils/progmem_string_view.h"

#include <cstring>
#include <iosfwd>
#include <string>

#include "absl/strings/string_view.h"
#include "extras/test_tools/print_to_std_string.h"
#include "extras/test_tools/progmem_string_view_utils.h"
#include "extras/test_tools/string_view_utils.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "utils/hex_escape.h"
#include "utils/o_print_stream.h"
#include "utils/platform.h"
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

TEST(ProgmemStringViewTest, LowerComparison) {
  ProgmemStringView psv(kLowerStr);
  EXPECT_EQ(psv.size(), std::strlen(kLowerStr));
  EXPECT_EQ(psv.size(), kLowerView.size());
  EXPECT_EQ(psv, kLowerView);

  EXPECT_NE(psv, kMixedView);
  EXPECT_NE(psv, kUpperView);

  // Make a copy so that we know that operator== isn't just comparing pointers.
  std::string str(kLowerStr);
  StringView view = MakeStringView(str);
  EXPECT_EQ(psv, view);

  // This prefix will share the same pointer, but not the same length.
  EXPECT_NE(psv, kLowerView.prefix(8));

  // Case-insensitively equal to all of the variants.
  EXPECT_TRUE(CaseEqual(psv, view));
  EXPECT_TRUE(CaseEqual(psv, kLowerView));
  EXPECT_TRUE(CaseEqual(psv, kMixedView));
  EXPECT_TRUE(CaseEqual(psv, kUpperView));

  // Equal to kLowerStr if we lower-case the ProgmemStringView, but not equal to
  // the other variants.
  EXPECT_TRUE(LoweredEqual(psv, view));
  EXPECT_TRUE(LoweredEqual(psv, kLowerView));
  EXPECT_FALSE(LoweredEqual(psv, kMixedView));
  EXPECT_FALSE(LoweredEqual(psv, kUpperView));

  // Not case-insensitively equal to an empty string, nor to prefixes of itself.
  EXPECT_FALSE(CaseEqual(psv, StringView()));
  EXPECT_FALSE(CaseEqual(psv, StringView("")));
  EXPECT_FALSE(CaseEqual(psv, kLowerView.prefix(1)));
  EXPECT_FALSE(CaseEqual(psv, kLowerView.prefix(kLowerView.size() - 1)));

  // at() will return the appropriate character.
  EXPECT_EQ(psv.at(0), 's');
  EXPECT_EQ(psv.at(psv.size() - 1), '.');
}

TEST(ProgmemStringViewTest, MixedComparison) {
  ProgmemStringView psv(kMixedStr);
  EXPECT_EQ(psv.size(), std::strlen(kMixedStr));
  EXPECT_EQ(psv.size(), kMixedView.size());
  EXPECT_EQ(psv, kMixedView);

  EXPECT_NE(psv, kLowerView);
  EXPECT_NE(psv, kUpperView);

  // Make a copy so that we know that operator== isn't just comparing pointers.
  std::string str(kMixedStr);
  StringView view = MakeStringView(str);
  EXPECT_EQ(psv, view);

  // This prefix will share the same pointer, but not the same length.
  EXPECT_NE(psv, kMixedView.prefix(8));

  // Case-insensitively equal to all of the variants.
  EXPECT_TRUE(CaseEqual(psv, view));
  EXPECT_TRUE(CaseEqual(psv, kLowerView));
  EXPECT_TRUE(CaseEqual(psv, kMixedView));
  EXPECT_TRUE(CaseEqual(psv, kUpperView));

  // Not case-insensitively equal to an empty string, nor to prefixes of itself.
  EXPECT_FALSE(CaseEqual(psv, StringView("")));
  EXPECT_FALSE(CaseEqual(psv, StringView()));
  EXPECT_FALSE(CaseEqual(psv, kMixedView.prefix(1)));
  EXPECT_FALSE(CaseEqual(psv, kMixedView.prefix(kMixedView.size() - 1)));

  // Equal to kLowerStr if we lower-case the ProgmemStringView, but not equal to
  // the other variants.
  EXPECT_FALSE(LoweredEqual(psv, view));
  EXPECT_TRUE(LoweredEqual(psv, kLowerView));
  EXPECT_FALSE(LoweredEqual(psv, kMixedView));
  EXPECT_FALSE(LoweredEqual(psv, kUpperView));

  // at() will return the appropriate character.
  EXPECT_EQ(psv.at(0), 'S');
  EXPECT_EQ(psv.at(psv.size() - 1), '.');
}

TEST(ProgmemStringViewTest, UpperComparison) {
  ProgmemStringView psv(kUpperStr);
  EXPECT_EQ(psv.size(), std::strlen(kUpperStr));
  EXPECT_EQ(psv.size(), kUpperView.size());
  EXPECT_EQ(psv, kUpperView);

  EXPECT_NE(psv, kMixedView);
  EXPECT_NE(psv, kLowerView);

  // Make a copy so that we know that operator== isn't just comparing pointers.
  std::string str(kUpperStr);
  StringView view = MakeStringView(str);
  EXPECT_EQ(psv, view);

  // This prefix will share the same pointer, but not the same length.
  EXPECT_NE(psv, kUpperView.prefix(8));

  // Case-insensitively equal to all of the variants.
  EXPECT_TRUE(CaseEqual(psv, view));
  EXPECT_TRUE(CaseEqual(psv, kLowerView));
  EXPECT_TRUE(CaseEqual(psv, kMixedView));
  EXPECT_TRUE(CaseEqual(psv, kUpperView));

  // Not case-insensitively equal to an empty string, nor to prefixes of itself.
  EXPECT_FALSE(CaseEqual(psv, StringView("")));
  EXPECT_FALSE(CaseEqual(psv, kUpperView.prefix(1)));
  EXPECT_FALSE(CaseEqual(psv, kUpperView.prefix(kUpperView.size() - 1)));

  // Equal to kLowerStr if we lower-case the ProgmemStringView, but not equal to
  // the other variants.
  EXPECT_FALSE(LoweredEqual(psv, view));
  EXPECT_TRUE(LoweredEqual(psv, kLowerView));
  EXPECT_FALSE(LoweredEqual(psv, kMixedView));
  EXPECT_FALSE(LoweredEqual(psv, kUpperView));

  // at() will return the appropriate character.
  EXPECT_EQ(psv.at(0), 'S');
  EXPECT_EQ(psv.at(psv.size() - 1), '.');
}

TEST(ProgmemStringViewTest, Copy) {
  ProgmemStringView psv(kMixedStr);
  EXPECT_EQ(psv.size(), sizeof(kMixedStr) - 1);

  // Make a buffer that is one byte bigger than needed for CopyTo so that we can
  // add a NUL at the end.
  char buffer[sizeof kMixedStr] = "";
  buffer[kMixedView.size()] = 0;
  EXPECT_EQ(psv.size() + 1, sizeof buffer);
  EXPECT_THAT(absl::string_view(buffer), IsEmpty());

  // Can't copy if the destination size is too small.
  EXPECT_FALSE(psv.CopyTo(buffer, 0));
  EXPECT_THAT(absl::string_view(buffer), IsEmpty());
  EXPECT_FALSE(psv.CopyTo(buffer, psv.size() - 1));
  EXPECT_THAT(absl::string_view(buffer), IsEmpty());

  // And can copy if the destination is the right size or larger.
  EXPECT_TRUE(psv.CopyTo(buffer, psv.size()));
  EXPECT_EQ(absl::string_view(buffer), kMixedStr);
  EXPECT_TRUE(psv.CopyTo(buffer, sizeof buffer));
  EXPECT_EQ(absl::string_view(buffer), kMixedStr);
}

TEST(ProgmemStringViewTest, PrintTo) {
  ProgmemStringView psv(kMixedStr);
  PrintToStdString out;
  EXPECT_EQ(psv.printTo(out), psv.size());
  EXPECT_EQ(out.str(), kMixedStr);
}

TEST(ProgmemStringViewTest, StreamMixed) {
  ProgmemStringView psv(kMixedStr);

  PrintToStdString p2ss;
  OPrintStream out(p2ss);
  out << psv;
  EXPECT_EQ(p2ss.str(), kMixedStr);

  std::ostringstream oss;
  oss << psv;
  EXPECT_EQ(oss.str(), kMixedStr);
}

TEST(ProgmemStringViewTest, StreamUpper) {
  ProgmemStringView psv(kUpperStr);

  PrintToStdString p2ss;
  OPrintStream out(p2ss);
  out << psv;
  EXPECT_EQ(p2ss.str(), kUpperStr);

  std::ostringstream oss;
  oss << psv;
  EXPECT_EQ(oss.str(), kUpperStr);
}

TEST(ProgmemStringViewTest, StreamHexEscaped) {
  ProgmemStringView psv(kLowerStr);
  PrintToStdString p2ss;
  OPrintStream out(p2ss);
  out << HexEscaped(psv);
  EXPECT_EQ(p2ss.str(), kLowerHexEscaped);
}

TEST(ProgmemStringViewTest, Equality) {
  ProgmemStringView psv1("abc");
  EXPECT_EQ(psv1, psv1);
  EXPECT_TRUE(psv1.Identical(psv1));

  std::string abc("abc");
  ProgmemStringView psv2(abc.data(), abc.size());
  EXPECT_EQ(psv1, psv2);
  EXPECT_FALSE(psv1.Identical(psv2));

  ProgmemStringView psv3(abc.data(), abc.size());
  EXPECT_EQ(psv1, psv3);
  EXPECT_EQ(psv2, psv3);
  EXPECT_TRUE(psv2.Identical(psv3));
}

TEST(ProgmemStringViewTest, Inequality) {
  ProgmemStringView psv1("aBC");
  ProgmemStringView psv2("abc");
  EXPECT_NE(psv1, psv2);

  ProgmemStringView psv3("");
  EXPECT_NE(psv1, psv3);
}

}  // namespace
}  // namespace test
}  // namespace alpaca
