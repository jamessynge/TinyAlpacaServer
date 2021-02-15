#include "alpaca-decoder/string_view.h"

#include <string>

#include "alpaca-decoder/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace alpaca {
constexpr StringView kSomeText("some-text");

std::string ToString(const StringView& view) {
  return std::string(view.data(), view.size());
}

TEST(StringViewTest, LiteralsAreSharedIfInitializeFromLiterals) {
  LOG(INFO) << "kSomeText: " << kSomeText;
  StringView another("some-text");
  LOG(INFO) << "another: " << another;
  EXPECT_EQ(kSomeText.data(), another.data());
}

TEST(StringViewTest, CreateEmpty) {
  {
    StringView view;
    EXPECT_EQ(view.data(), nullptr);
    EXPECT_EQ(view.size(), 0);
  }
  {
    StringView view("");
    EXPECT_NE(view.data(), nullptr);
    EXPECT_EQ(view.size(), 0);
  }
}

TEST(StringViewTest, CreateFromConstExpr) {
  constexpr char kConstStr[] = "123";
  StringView view(kConstStr);
  EXPECT_EQ(view.data(), kConstStr);
  EXPECT_EQ(view.size(), 3);
}

TEST(StringViewTest, CreateFromLiteral) {
  StringView view("123");
  EXPECT_EQ(view.size(), 3);
  std::string a = "123";
  EXPECT_EQ(ToString(view), a);
}

TEST(StringViewTest, StartsWithStringView) {
  StringView view("1234");
  EXPECT_TRUE(view.starts_with(StringView("")));
  EXPECT_TRUE(view.starts_with(StringView("1")));
  EXPECT_TRUE(view.starts_with(StringView("12")));
  EXPECT_TRUE(view.starts_with(StringView("123")));
  EXPECT_TRUE(view.starts_with(StringView("1234")));
  EXPECT_FALSE(view.starts_with(StringView("12345")));
  EXPECT_FALSE(view.starts_with(StringView("13")));
  EXPECT_FALSE(view.starts_with(StringView("234")));
}

TEST(StringViewTest, RemovePrefix) {
  const StringView master("123456");
  for (int n = 0; n <= 6; ++n) {
    StringView view = master;
    view.remove_prefix(n);
    EXPECT_EQ(view.data(), master.data() + n);
    EXPECT_EQ(view.size(), master.size() - n);
  }
}

TEST(StringViewDeathTest, RemovePrefixTooLong) {
  constexpr char kConstStr[] = "123";
  StringView view(kConstStr);
  EXPECT_EQ(view.data(), kConstStr);
  EXPECT_EQ(view.size(), 3);

  EXPECT_DEBUG_DEATH(
      {
        view.remove_prefix(4);
        EXPECT_EQ(view.data(), kConstStr + 4);
      },
      "prefix_length");
}

}  // namespace alpaca
