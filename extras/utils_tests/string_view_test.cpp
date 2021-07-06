#include "utils/string_view.h"

// Author: james.synge@gmail.com

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "absl/strings/numbers.h"
#include "config.h"
#include "extras/test_tools/string_view_utils.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "logging.h"

namespace alpaca {
namespace test {
namespace {

using ::testing::IsEmpty;

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
    StringView view = MakeStringView(empty);
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

  EXPECT_FALSE(view.match_and_consume(StringView("123")));
  EXPECT_FALSE(view.match_and_consume(StringView("23456789")));
  EXPECT_FALSE(view.match_and_consume(StringView("678")));

  EXPECT_EQ(view, std::string("2345678"));

  EXPECT_TRUE(view.match_and_consume(StringView("23")));
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
  EXPECT_NE(view1.data(), view2.data());
  EXPECT_EQ(view1.size(), 0);
  EXPECT_EQ(view2.size(), 0);

  // Case: both strings have different pointers, but point to underlying strings
  // with the same value.
  view1 = StringView("123");
  constexpr char kConstStr[] = "123";
  view2 = StringView(kConstStr);

  EXPECT_EQ(view1, view2);
  EXPECT_NE(view1.data(), view2.data());
  EXPECT_EQ(view1.size(), 3);
  EXPECT_EQ(view2.size(), 3);

  // Case: both strings have the same pointer but different lengths.
  view1 = StringView("1234");
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
  StringView view = MakeStringView(buffer);

  EXPECT_EQ(view, buffer);
  EXPECT_EQ(view, StringView("abcdefghijkl"));
  EXPECT_TRUE(view.contains(StringView("abcdefghijkl")));
  EXPECT_TRUE(view.contains(StringView("abc")));
  EXPECT_TRUE(view.contains(StringView("def")));
  EXPECT_TRUE(view.contains(StringView("jkl")));

  view.remove_prefix(3);
  view.remove_suffix(3);

  EXPECT_FALSE(view.contains(StringView("abcdefghijkl")));
  EXPECT_FALSE(view.contains(StringView("abc")));
  EXPECT_TRUE(view.contains(StringView("defghi")));
  EXPECT_TRUE(view.contains(StringView("def")));
  EXPECT_TRUE(view.contains(StringView("ghi")));
  EXPECT_FALSE(view.contains(StringView("ghijkl")));
  EXPECT_FALSE(view.contains(StringView("jkl")));
}

TEST(StringViewTest, EndsWithChar) {
  StringView view("12345", 4);
  EXPECT_FALSE(view.ends_with('1'));
  EXPECT_FALSE(view.ends_with('2'));
  EXPECT_FALSE(view.ends_with('3'));
  EXPECT_TRUE(view.ends_with('4'));
  EXPECT_FALSE(view.ends_with('5'));
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
      {"0000004294967295", UINT32_MAX},
  });
  for (const auto& test_case : test_cases) {
    StringView view = MakeStringView(test_case.first);
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
    StringView view = MakeStringView(not_a_uint32);
    uint32_t out = tester;
    EXPECT_FALSE(view.to_uint32(out));
    EXPECT_EQ(out, tester);
    ++tester;
  }
}

TEST(StringViewTest, ToInt32) {
  std::vector<std::pair<std::string, int32_t>> test_cases({
      {"0", 0},
      {"-0", 0},
      {"1", 1},
      {"-1", -1},
      {"10", 10},
      {"-10", -10},
      {"99", 99},
      {"-99", -99},
      {"0000000000123456789", 123456789},
      {"-2147483647", -2147483647},
      {"-000000002147483647", -2147483647},
      {"-2147483648", -2147483648},
      {"-000000002147483648", INT32_MIN},
      {"2147483647", 2147483647},
      {"0000000002147483647", INT32_MAX},
  });
  for (const auto& test_case : test_cases) {
    StringView view = MakeStringView(test_case.first);
    int32_t out = 0;
    EXPECT_TRUE(view.to_int32(out));
    EXPECT_EQ(out, test_case.second);

    out = ~out;  // Flip all the bits, try again.
    EXPECT_TRUE(view.to_int32(out));
    EXPECT_EQ(out, test_case.second);
  }
}

TEST(StringViewTest, ToInt32Fails) {
  uint32_t tester = 1734594785;  // Vaguely random, value doesn't really matter.
  for (const std::string not_an_int32 : {
           "+1",          // Explicitly positive number.
           "--1",         // Double negative number
           "-",           // Sign only.
           "+",           // Sign only.
           "",            // Empty string
           "123,456",     // Number with non-digit.
           "123.456",     // Number with non-digit.
           "2147483648",  // One too large.
       }) {
    StringView view = MakeStringView(not_an_int32);
    int32_t out = tester;
    EXPECT_FALSE(view.to_int32(out));
    EXPECT_EQ(out, tester);
    ++tester;
  }
  int32_t min_int = -2147483648;
  EXPECT_EQ(min_int, INT_MIN);
  min_int = 0x80000000;
  EXPECT_EQ(min_int, INT_MIN);
}

TEST(StringViewTest, ToDouble) {
  std::vector<std::pair<std::string, double>> test_cases({
      {"0", 0},
      {"-0", 0},
      {"0.", 0},
      {"-0.", 0},
      {".0", 0},
      {"-.0", 0},
      {"1", 1},
      {"-1", -1},
      {"1.", 1},
      {"-1.", -1},
      {"10", 10},
      {"-10", -10},
      {"99", 99},
      {"-99", -99},
      {".1", .1},
      {".01", .01},
      {".22", .22},
      {"9.1", 9.1},
      {"9000.01", 9000.01},
      {"2222.22", 2222.22},
      {"987654321.123456789", 987654321.123456789},
      {".99999999999", .99999999999},
      {"0000000000123456789", 123456789},
      {"-2147483647", -2147483647},
      {"-000000002147483647", -2147483647},
      {"-2147483648", -2147483648},
      {"-000000002147483648", INT32_MIN},
      {"2147483647", 2147483647},
      {"0000000002147483647", INT32_MAX},
      {"4294967295", 4294967295},
      {"0000004294967295", UINT32_MAX},
      {"429496729500", UINT32_MAX * 100.0},
  });
  for (const auto& test_case : test_cases) {
    StringView view = MakeStringView(test_case.first);
    double out = 0;
    EXPECT_TRUE(view.to_double(out)) << "\nInput string: " << test_case.first;
    EXPECT_DOUBLE_EQ(out, test_case.second)
        << "\nInput string: " << test_case.first;

    // Make sure that the value of out has no impact on decoding by changing it
    // to a very different value, then decoding again.
    out = -out * 3.14159265359;
    EXPECT_TRUE(view.to_double(out)) << "\nInput string: " << test_case.first;
    EXPECT_DOUBLE_EQ(out, test_case.second)
        << "\nInput string: " << test_case.first;
  }
}

TEST(StringViewTest, ToDoubleFails) {
  uint32_t tester = 1734594785;  // Vaguely random, value doesn't really matter.
  for (const std::string not_a_double : {
           "",         // Empty string
           ".",        // Decimal point only.
           "+1",       // Explicitly positive number.
           "--1",      // Double negative number
           "-",        // Sign only.
           "+",        // Sign only.
           "-.",       // Sign and decimal point only.
           "+.",       // Sign and decimal point only.
           "123,456",  // Number with non-digit.
           "1.2.3",    // Too many decimal points.
       }) {
    StringView view = MakeStringView(not_a_double);
    double out = tester;
    EXPECT_FALSE(view.to_double(out)) << "\nInput string: " << not_a_double;
    EXPECT_EQ(out, tester) << "\nInput string: " << not_a_double;
    ++tester;
  }
}

#ifdef NDEBUG
// Really slow if not optimized.
#if TAS_ENABLED_VLOG_LEVEL < 1
// to_uint32 and to_int32 are a bit noisy at level 5, which would slow this down
// too much, so skip if logging.
#if 0
// On my workstation, this runs at about 20 million values per second, or almost
// 4 minutes for each of these two test cases, which is really not worth the
// trouble.

TEST(StringViewTest, ToUint32All) {
  char buffer[32];
  uint32_t value = 0;
  uint32_t announce = 0;
  do {
    int size = absl::numbers_internal::FastIntToBuffer(value, buffer) - buffer;
    ASSERT_LT(size, 32);
    StringView view(buffer, size);
    uint32_t out = 0;
    ASSERT_TRUE(view.to_uint32(out));
    ASSERT_EQ(out, value);
    if (++announce >= 10000000) {
      announce = 0;
      LOG(INFO) << "value: " << value;
    }
  } while (value++ < UINT32_MAX);
}

TEST(StringViewTest, ToInt32All) {
  char buffer[32];
  int32_t value = INT32_MIN;
  uint32_t announce = 0;
  do {
    int size = absl::numbers_internal::FastIntToBuffer(value, buffer) - buffer;
    ASSERT_LT(size, 32);
    StringView view(buffer, size);
    int32_t out = 0;
    ASSERT_TRUE(view.to_int32(out));
    ASSERT_EQ(out, value);
    if (++announce >= 10000000) {
      announce = 0;
      LOG(INFO) << "value: " << value;
    }
  } while (value++ < INT32_MAX);
}

#endif  // 0
#endif  // TAS_ENABLED_VLOG_LEVEL < 1
#endif  // NDEBUG

TEST(StringViewTest, StreamOutOperator) {
  std::ostringstream oss;
  const std::string s("abc'\"\t\r\e");
  const StringView view = MakeStringView(s);
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

}  // namespace
}  // namespace test
}  // namespace alpaca
