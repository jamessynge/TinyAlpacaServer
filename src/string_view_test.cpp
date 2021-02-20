#include "string_view.h"

// Author: james.synge@gmail.com

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "config.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "logging.h"

namespace alpaca {

namespace {
using ::testing::IsEmpty;

ALPACA_SERVER_CONSTEXPR_FUNC const StringView kSomeText("some-text");

std::string ToString(const StringView& view) {
  return std::string(view.data(), view.size());
}

////////////////////////////////////////////////////////////////////////////////
// Constructor Tests.

TEST(StringViewTest, CreateFromConstExpr) {
  // This demonstrates that constants of type "constexpr char[]" each have their
  // own copy of the string. As a result, we should avoid these for initializing
  // StringView's where we might want to have the same literal strings multiple
  // times in the same program.
  constexpr char kConstStr1[] = "123";
  constexpr char kConstStr2[] = "123";

  EXPECT_NE(kConstStr1, kConstStr2);

  StringView view1(kConstStr1);
  EXPECT_EQ(view1.data(), kConstStr1);
  EXPECT_EQ(view1.size(), 3);
  EXPECT_FALSE(view1 == nullptr);
  EXPECT_EQ(view1.ToString(), "123");
  EXPECT_EQ(view1.ToHexEscapedString(), "\"123\"");

  std::string str1(kConstStr1);
  EXPECT_EQ(str1, view1);
  EXPECT_NE(str1.data(), view1.data());

  StringView view2(kConstStr2);
  EXPECT_EQ(view2.data(), kConstStr2);
  EXPECT_EQ(view2.size(), 3);

  EXPECT_NE(view1.data(), view2.data());
  EXPECT_EQ(view1, view2);
}

TEST(StringViewTest, CreateFromLiteral) {
  // This demonstrates that multiple StringViews initialized from literals with
  // the same value will point to the same underlying storage.
  StringView view("123");
  EXPECT_EQ(view.size(), 3);

  StringView view1("123");
  EXPECT_EQ(view1.data(), "123");
  EXPECT_EQ(view1.size(), 3);

  std::string str1("123");
  EXPECT_EQ(str1, view1);
  EXPECT_NE(str1.data(), view1.data());

  StringView view2("123");
  EXPECT_EQ(view2.data(), "123");
  EXPECT_EQ(view2.size(), 3);

  EXPECT_EQ(view1.data(), view2.data());
  EXPECT_EQ(view1, view2);
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
  {
    std::string empty;
    StringView view(empty);
    EXPECT_EQ(view.data(), empty.data());
    EXPECT_EQ(view.size(), 0);
  }
}

TEST(StringViewTest, CopyConstructor) {
  StringView view1("123");
  EXPECT_NE(view1.data(), nullptr);
  EXPECT_EQ(view1.data(), "123");
  EXPECT_EQ(view1.size(), 3);
  EXPECT_EQ(view1, std::string("123"));

  StringView view2(view1);
  EXPECT_EQ(view1.data(), view2.data());
  EXPECT_EQ(view1.size(), view2.size());
  EXPECT_EQ(view2, std::string("123"));
}

////////////////////////////////////////////////////////////////////////////////
// Mutating method tests.

TEST(StringViewTest, AssignmentOperator) {
  StringView view1("123");
  EXPECT_NE(view1.data(), nullptr);
  EXPECT_EQ(view1.data(), "123");
  EXPECT_EQ(view1.size(), 3);
  EXPECT_EQ(view1, std::string("123"));

  StringView view2;
  EXPECT_THAT(view2, IsEmpty());
  EXPECT_EQ(view2.data(), nullptr);
  EXPECT_EQ(view2, std::string_view());

  view2 = view1;

  EXPECT_EQ(view1.data(), view2.data());
  EXPECT_EQ(view1.size(), view2.size());
  EXPECT_EQ(view2, std::string("123"));
}

TEST(StringViewTest, MatchAndConsume) {
  const StringView view_all("123456789");
  StringView view = view_all.substr(1, view_all.size() - 2);
  EXPECT_EQ(view, std::string("2345678"));
  EXPECT_EQ(view.data(), view_all.data() + 1);
  EXPECT_EQ(view.size(), view_all.size() - 2);

  EXPECT_FALSE(view.match_and_consume("123"));
  EXPECT_FALSE(view.match_and_consume("23456789"));
  EXPECT_FALSE(view.match_and_consume("678"));

  EXPECT_EQ(view, std::string("2345678"));

  EXPECT_TRUE(view.match_and_consume("23"));
  EXPECT_EQ(view, view_all.substr(3, 5));
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

////////////////////////////////////////////////////////////////////////////////
// Non-mutating method tests.

TEST(StringViewTest, Equals) {
  // Case: both strings are empty, though ptr == nullptr for the first, and the
  // other points at a literal string, which is empty.
  StringView view1;
  StringView view2("");

  EXPECT_EQ(view1, view2);  // Tests operator==
  EXPECT_TRUE(view1 == nullptr);
  EXPECT_THAT(view1.ToString(), IsEmpty());
  EXPECT_EQ(view1.ToHexEscapedString(), "\"\"");
  EXPECT_NE(view1.data(), view2.data());
  EXPECT_EQ(view1.size(), 0);
  EXPECT_EQ(view2.size(), 0);
  EXPECT_THAT(view2.ToString(), IsEmpty());
  EXPECT_EQ(view2.ToHexEscapedString(), "\"\"");

  // Case: both strings have different pointers, but point to underlying strings
  // with the same value.
  view1 = "123";
  constexpr char kConstStr[] = "123";
  view2 = kConstStr;

  EXPECT_EQ(view1, view2);
  EXPECT_NE(view1.data(), view2.data());
  EXPECT_EQ(view1.size(), 3);
  EXPECT_EQ(view2.size(), 3);

  // Case: both strings have the same pointer but different lengths.
  view1 = "1234";
  view2 = view1.prefix(3);
  EXPECT_NE(view1, view2);  // Tests operator!=
  EXPECT_EQ(view1.data(), view2.data());
  EXPECT_EQ(view1.size(), 4);
  EXPECT_EQ(view2.size(), 3);
}

TEST(StringViewTest, ContainsChar) {
  StringView view1("abcdefghijkl", 9);
  for (const char c : StringView("ihgfedcba")) {
    EXPECT_TRUE(view1.contains(c));
  }
  for (const char c : StringView("jkl")) {
    EXPECT_FALSE(view1.contains(c));
  }

  auto view2 = view1.substr(3, 3);
  for (const char c : StringView("def")) {
    EXPECT_TRUE(view2.contains(c));
  }
  for (const char c : StringView("abcghijkl")) {
    EXPECT_FALSE(view2.contains(c));
  }
}

TEST(StringViewTest, ContainsStringView) {
  const std::string buffer("abcdefghijkl");
  StringView view(buffer);

  EXPECT_EQ(view, buffer);
  EXPECT_EQ(view, StringView("abcdefghijkl"));
  EXPECT_TRUE(view.contains("abcdefghijkl"));
  EXPECT_TRUE(view.contains("abc"));
  EXPECT_TRUE(view.contains("def"));
  EXPECT_TRUE(view.contains("jkl"));

  view.remove_prefix(3);
  view.remove_suffix(3);

  EXPECT_FALSE(view.contains("abcdefghijkl"));
  EXPECT_FALSE(view.contains("abc"));
  EXPECT_TRUE(view.contains("defghi"));
  EXPECT_TRUE(view.contains("def"));
  EXPECT_TRUE(view.contains("ghi"));
  EXPECT_FALSE(view.contains("ghijkl"));
  EXPECT_FALSE(view.contains("jkl"));
}

TEST(StringViewTest, EndsWithChar) {
  StringView view("12345", 4);
  EXPECT_FALSE(view.ends_with('1'));
  EXPECT_FALSE(view.ends_with('2'));
  EXPECT_FALSE(view.ends_with('3'));
  EXPECT_TRUE(view.ends_with('4'));
  EXPECT_FALSE(view.ends_with('5'));
}

TEST(StringViewTest, EqualsOtherLowered) {
  const char kLower[] = "some text with numbers 123";
  const char kMixed[] = "Some Text WITH numbers 123";
  const StringView token(kLower);
  const std::string buffer(kMixed);
  const StringView input(buffer);

  EXPECT_EQ(token, token);
  EXPECT_EQ(input, input);
  EXPECT_NE(token, input);
  EXPECT_TRUE(token.equals_other_lowered(token));
  EXPECT_TRUE(token.equals_other_lowered(input));
  EXPECT_FALSE(token.equals_other_lowered("ome Text WITH numbers 123"));
  EXPECT_FALSE(token.equals_other_lowered("some text with numbers 12"));
  EXPECT_FALSE(token.equals_other_lowered("some text with numbers 124"));
  EXPECT_FALSE(token.equals_other_lowered("some text-with numbers 123"));
}

TEST(StringViewDeathTest, EqualsOtherLoweredDcheckThisIsLower) {
  const char kLower[] = "some text with numbers 123";
  const char kMixed[] = "Some Text WITH numbers 123";
  const StringView token(kLower);
  const std::string buffer(kMixed);
  const StringView input(buffer);

  EXPECT_EQ(token, token);
  EXPECT_EQ(input, input);
  EXPECT_NE(token, input);
  EXPECT_TRUE(token.equals_other_lowered(token));
  EXPECT_TRUE(token.equals_other_lowered(input));

  EXPECT_DEBUG_DEATH({ EXPECT_FALSE(input.equals_other_lowered(input)); },
                     "isupper");
  EXPECT_DEBUG_DEATH({ EXPECT_FALSE(input.equals_other_lowered(token)); },
                     "isupper");
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

TEST(StringViewTest, StartsWithChar) {
  StringView view("12345");
  EXPECT_TRUE(view.starts_with('1'));
  EXPECT_FALSE(view.starts_with('2'));
  EXPECT_FALSE(view.starts_with('3'));
  EXPECT_FALSE(view.starts_with('4'));
  EXPECT_FALSE(view.starts_with('5'));

  view = view.substr(1, 3);
  EXPECT_FALSE(view.starts_with('1'));
  EXPECT_TRUE(view.starts_with('2'));
  EXPECT_FALSE(view.starts_with('3'));
  EXPECT_FALSE(view.starts_with('4'));
  EXPECT_FALSE(view.starts_with('5'));
}

TEST(StringViewTest, Suffix) {
  StringView view("12345");
  EXPECT_EQ(view.suffix(9), view);  // Too long becomes actual size.
  EXPECT_EQ(view.suffix(7), StringView("12345"));
  EXPECT_EQ(view.suffix(4), StringView("2345"));
  EXPECT_EQ(view.suffix(3), StringView("345"));
  EXPECT_EQ(view.suffix(2), StringView("45"));
  EXPECT_EQ(view.suffix(1), StringView("5"));
  EXPECT_EQ(view.suffix(0), StringView());
}

TEST(StringViewTest, ToUint32) {
  std::vector<std::pair<std::string, uint32_t>> test_cases({
      {"0", 0},
      {"1", 1},
      {"10", 10},
      {"99", 99},
      {"0000000000123456789", 123456789},
      {"987654321", 987654321},
      {"4294967295", 4294967295},
      {"0000004294967295", 4294967295},
  });
  for (const auto& test_case : test_cases) {
    StringView view(test_case.first);
    uint32_t out = 0;
    EXPECT_TRUE(view.to_uint32(out));
    EXPECT_EQ(out, test_case.second);

    out = ~out;  // Flip all the bits, try again.
    EXPECT_TRUE(view.to_uint32(out));
    EXPECT_EQ(out, test_case.second);
  }
}

TEST(StringViewTest, ToUint32Fails) {
  uint32_t tester = 1734594785;  // Vaguely random, value doesn't really matter.
  for (const std::string not_a_uint32 : {
           "-1",          // Negative number
           "+1",          // Explicitly positive number.
           "",            // Empty string
           "123,456",     // Number with non-digit.
           "4294967296",  // One too large.
           "4294967300"   // 5 too large, hits a different case.
       }) {
    StringView view(not_a_uint32);
    uint32_t out = tester;
    EXPECT_FALSE(view.to_uint32(out));
    EXPECT_EQ(out, tester);
    ++tester;
  }
}

TEST(StringViewTest, StreamOutOperator) {
  std::ostringstream oss;
  const std::string s("abc'\"\t\r\e");
  const StringView view(s);
  oss << view;
  EXPECT_EQ(oss.str(), s);
}

TEST(StringViewTest, StreamOutOperatorObeysLimits) {
  {
    // Confirm that it doesn't print beyond the size it has.
    std::ostringstream oss;
    StringView view("abcdef", 4);
    oss << view;
    EXPECT_EQ(oss.str(), "abcd");
  }
  {
    // Confirm that it doesn't print beyond the size it has.
    std::ostringstream oss;
    StringView view("abcdefghi");
    view.remove_prefix(3);
    view.remove_suffix(3);
    oss << view;
    EXPECT_EQ(oss.str(), "def");
  }
  {
    // Confirm that it doesn't print beyond the size it has.
    std::ostringstream oss;
    StringView view;
    oss << view;
    EXPECT_EQ(oss.str(), "");
  }
}

TEST(JsonStringViewTest, StreamOutOperator) {
  {
    std::ostringstream oss;
    StringView view("abc");
    JsonStringView json(view);
    oss << json;
    EXPECT_EQ(oss.str(), "\"abc\"");
  }
  {
    // Note: Even though JSON allows a forward slash ('/') to be escaped, it
    // does not require it.
    std::ostringstream oss;
    StringView view("<tag attr=\"value with slash ('\\')\">\b\f\n\r\t</tag>");
    JsonStringView json(view);
    oss << json;
    EXPECT_EQ(oss.str(),
              "\"<tag attr=\\\"value with slash ('\\\\')\\\">"
              "\\b\\f\\n\\r\\t</tag>\"");
  }
}

TEST(JsonStringViewTest, StreamOutOperatorObeysLimits) {
  {
    // Confirm that it doesn't print beyond the size it has.
    std::ostringstream oss;
    StringView view("abcdef", 4);
    JsonStringView json(view);
    oss << json;
    EXPECT_EQ(oss.str(), "\"abcd\"");
  }
  {
    // Confirm that it doesn't print beyond the size it has.
    std::ostringstream oss;
    StringView view("\tbcdefgh\t");
    view.remove_prefix(3);
    view.remove_suffix(3);
    JsonStringView json(view);
    oss << json;
    EXPECT_EQ(oss.str(), "\"def\"");
  }
  {
    // Confirm that it doesn't print beyond the size it has.
    std::ostringstream oss;
    StringView view;
    JsonStringView json(view);
    oss << json;
    EXPECT_EQ(oss.str(), "\"\"");
  }
}

TEST(JsonStringViewDeathTest,
     StreamOutOperatorDropsUnsupportedControlCharacters) {
  for (const std::string& str : {
           std::string(1, '\0'),    // NUL
           std::string(1, '\a'),    // BEL
           std::string(1, '\e'),    // ESC
           std::string(1, '\x7f'),  // DEL
       }) {
    EXPECT_EQ(str.size(), 1);
    std::ostringstream oss;
    StringView view(str);
    JsonStringView json(view);
    EXPECT_EQ(json.view().size(), 1);
    LOG(INFO) << "json.view: " << json.view().ToHexEscapedString();
    EXPECT_DEBUG_DEATH(
        {
          oss << json;
          EXPECT_EQ(oss.str(), "\"\"");
        },
        "Unsupported JSON character");
  }
}

}  // namespace
}  // namespace alpaca
