#include "literal.h"

#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "extras/tests/test_utils.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace {
using ::testing::IsEmpty;

constexpr char kStr[] = "something";

TEST(LiteralTest, Basic) {
  Literal literal(kStr);
  EXPECT_EQ(literal.size(), 9);

  std::string str(kStr);
  StringView view(str);
  EXPECT_EQ(literal, view);
  EXPECT_TRUE(literal.case_equal(view));

  EXPECT_NE(literal, view.prefix(8));

  EXPECT_NE(literal, view.suffix(8));

  EXPECT_TRUE(literal.case_equal("SOMETHING"));
  EXPECT_TRUE(literal.case_equal("SomeThing"));
  EXPECT_FALSE(literal.case_equal("SomeThing."));
  EXPECT_FALSE(literal.case_equal("SomeThin."));

  char buffer[sizeof kStr] = "";
  EXPECT_EQ(literal.size() + 1, sizeof buffer);
  EXPECT_THAT(absl::string_view(buffer), IsEmpty());
  EXPECT_FALSE(literal.copyTo(buffer, 0));
  EXPECT_THAT(absl::string_view(buffer), IsEmpty());
  EXPECT_FALSE(literal.copyTo(
      buffer, literal.size()));  // Need room for the NUL terminator.
  EXPECT_THAT(absl::string_view(buffer), IsEmpty());
  EXPECT_TRUE(literal.copyTo(buffer, literal.size() + 1));
  EXPECT_EQ(absl::string_view(buffer), kStr);

  {
    PrintToString out;
    EXPECT_EQ(literal.printTo(out), literal.size());
    EXPECT_EQ(out.str(), kStr);
  }

  {
    auto expected = absl::StrCat("\"", kStr, "\"");
    PrintToString out;
    EXPECT_EQ(literal.printJsonEscapedTo(out), expected.size());
    EXPECT_EQ(out.str(), expected);
  }
}

}  // namespace
}  // namespace alpaca
