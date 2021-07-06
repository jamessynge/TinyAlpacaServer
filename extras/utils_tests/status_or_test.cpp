#include "utils/status_or.h"

#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "utils/string_view.h"

namespace alpaca {
namespace test {
namespace {

template <typename T>
StatusOr<T> ReturnStatusWithoutMessage(uint32_t code) {
  return Status(code);
}

template <typename T>
StatusOr<T> ReturnStatusWithMessage(uint32_t code, Literal message) {
  return Status(code, message);
}

template <typename T>
StatusOr<T> ReturnValue(const T& value) {
  return value;
}

TEST(StatusOrTest, IntValue) {
  StatusOr<int> status_or(1);
  EXPECT_TRUE(status_or.ok());
  EXPECT_EQ(status_or.value(), 1);
  EXPECT_TRUE(status_or.status().ok());
  EXPECT_EQ(status_or.status().code(), 0);
  EXPECT_EQ(status_or.status().message(), Literal());

  status_or = Status(123, Literal("Oops"));
  EXPECT_FALSE(status_or.ok());
  EXPECT_FALSE(status_or.status().ok());
  EXPECT_EQ(status_or.status().code(), 123);
  EXPECT_EQ(status_or.status().message(), Literal("Oops"));

  status_or = ReturnValue<int>(123);
  EXPECT_TRUE(status_or.ok());
  EXPECT_EQ(status_or.value(), 123);
  EXPECT_TRUE(status_or.status().ok());
  EXPECT_EQ(status_or.status().code(), 0);
  EXPECT_EQ(status_or.status().message(), Literal());

  status_or = ReturnStatusWithoutMessage<int>(11508);
  EXPECT_FALSE(status_or.ok());
  EXPECT_FALSE(status_or.status().ok());
  EXPECT_EQ(status_or.status().code(), 11508);
  EXPECT_EQ(status_or.status().message(), Literal(""));
}

TEST(StatusOrTest, StringViewValue) {
  auto status_or = ReturnValue(StringView("abc"));
  EXPECT_TRUE(status_or.ok());
  EXPECT_EQ(status_or.value(), StringView("abc"));
  EXPECT_TRUE(status_or.status().ok());
  EXPECT_EQ(status_or.status().code(), 0);
  EXPECT_EQ(status_or.status().message(), Literal());

  status_or = Status(345);
  EXPECT_FALSE(status_or.ok());
  EXPECT_FALSE(status_or.status().ok());
  EXPECT_EQ(status_or.status().code(), 345);
  EXPECT_EQ(status_or.status().message(), Literal(""));

  status_or = StringView();
  EXPECT_TRUE(status_or.ok());
  EXPECT_EQ(status_or.value(), StringView(""));
  EXPECT_TRUE(status_or.status().ok());
  EXPECT_EQ(status_or.status().code(), 0);
  EXPECT_EQ(status_or.status().message(), Literal());

  status_or =
      ReturnStatusWithMessage<StringView>(90210, Literal("Beverly Hills"));
  EXPECT_FALSE(status_or.ok());
  EXPECT_FALSE(status_or.status().ok());
  EXPECT_EQ(status_or.status().code(), 90210);
  EXPECT_EQ(status_or.status().message(), Literal("Beverly Hills"));
}

}  // namespace
}  // namespace test
}  // namespace alpaca
