#include "utils/status.h"

#include "googletest/gtest.h"

namespace alpaca {
namespace {

Status ReturnStatusWithoutMessage(uint32_t code) { return Status(code); }
Status ReturnStatusWithMessage(uint32_t code, Literal message) {
  return Status(code, message);
}

TEST(StatusTest, LocalOk) {
  Status status(0);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(status.code(), 0);
  EXPECT_EQ(status.message(), Literal());
}

TEST(StatusTest, LocalOkWithMessage) {
  Status status(0, Literal("Foo"));
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(status.code(), 0);
  EXPECT_EQ(status.message(), Literal());
}

TEST(StatusTest, LocalNotOk) {
  Status status(123);
  EXPECT_FALSE(status.ok());
  EXPECT_EQ(status.code(), 123);
  EXPECT_EQ(status.message(), Literal());
}

TEST(StatusTest, LocalNotOkWithMessage) {
  Status status(123, Literal("Bar"));
  EXPECT_FALSE(status.ok());
  EXPECT_EQ(status.code(), 123);
  EXPECT_EQ(status.message(), Literal("Bar"));
}

TEST(StatusTest, ReturnedOk) {
  auto status = ReturnStatusWithoutMessage(0);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(status.code(), 0);
  EXPECT_EQ(status.message(), Literal());
}

TEST(StatusTest, ReturnedNotOk) {
  auto status = ReturnStatusWithoutMessage(999999999);
  EXPECT_FALSE(status.ok());
  EXPECT_EQ(status.code(), 999999999);
  EXPECT_EQ(status.message(), Literal());
}

TEST(StatusTest, ReturnedNotOkWithMessage) {
  auto status = ReturnStatusWithMessage(11111, Literal("baz"));
  EXPECT_FALSE(status.ok());
  EXPECT_EQ(status.code(), 11111);
  EXPECT_EQ(status.message(), Literal("baz"));
}

}  // namespace
}  // namespace alpaca
