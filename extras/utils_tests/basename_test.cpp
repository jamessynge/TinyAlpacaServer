#include "utils/basename.h"

#include <string_view>

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {

namespace test {
namespace {

TEST(TasBasenameTest, NoSlash) {
  PrintToStdString out;
  out.print(TAS_BASENAME("foo.bar.baz"));
  EXPECT_EQ(out.str(), "foo.bar.baz");
}

TEST(TasBasenameTest, LeadingSlashOnly) {
  // The templates would have to be more complex to remove the leading slash
  // if it is the only slash.
  PrintToStdString out;
  out.print(TAS_BASENAME("/bar.baz"));
  EXPECT_EQ(out.str(), "/bar.baz");
}

TEST(TasBasenameTest, MiddleSlash) {
  PrintToStdString out;
  out.print(TAS_BASENAME("foo/bar.baz"));
  EXPECT_EQ(out.str(), "bar.baz");
}

TEST(TasBasenameTest, LeadingAndMiddleSlash) {
  PrintToStdString out;
  out.print(TAS_BASENAME("/foo/bar.baz"));
  EXPECT_EQ(out.str(), "bar.baz");
}

TEST(TasBasenameTest, TrailingSlash) {
  PrintToStdString out;
  out.print(TAS_BASENAME("foo.bar.baz/"));
  EXPECT_EQ(out.str(), "");
}

using ::alpaca::tas_basename::BasenameStorage;

TEST(BasenameStorageTest, RawProgmemString) {
  using Hello = BasenameStorage<'h', 'e', 'l', 'l', 'o'>;
  PrintToStdString out;
  EXPECT_EQ(out.print(Hello::FlashStringHelper()), 5);
  EXPECT_EQ(out.str(), "hello");
}

TEST(BasenameStorageTest, TASLIT16_String) {
  using Hello = BasenameStorage<_TAS_EXPAND_16(, "Hello!")>;
  PrintToStdString out;
  EXPECT_EQ(out.print(Hello::FlashStringHelper()), 6);
  EXPECT_EQ(out.str(), "Hello!");
}
}  // namespace
}  // namespace test
}  // namespace alpaca
