#include "utils/basename.h"

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gtest.h"
#include "utils/inline_literal.h"

namespace alpaca {

namespace test {
namespace {

TEST(TasBasenameTest, NoSlash) {
  PrintToStdString out;
  out.print(TAS_BASENAME("foo.bar.baz"));
  EXPECT_EQ(out.str(), "foo.bar.baz");
}

TEST(TasBasenameTest, LeadingSlash) {
  PrintToStdString out;
  out.print(TAS_BASENAME("/bar.baz"));
  EXPECT_EQ(out.str(), "bar.baz");
}

TEST(TasBasenameTest, LeadingSlashes) {
  PrintToStdString out;
  out.print(TAS_BASENAME("//bar.baz"));
  EXPECT_EQ(out.str(), "bar.baz");
}

TEST(TasBasenameTest, MiddleSlash) {
  PrintToStdString out;
  out.print(TAS_BASENAME("foo/bar.baz"));
  EXPECT_EQ(out.str(), "bar.baz");
}

TEST(TasBasenameTest, LeadingAndMiddleSlashes) {
  PrintToStdString out;
  out.print(TAS_BASENAME("//foo//bar/baz.cc"));
  EXPECT_EQ(out.str(), "baz.cc");
}

TEST(TasBasenameTest, TrailingSlash) {
  PrintToStdString out;
  out.print(TAS_BASENAME("foo.bar.baz/"));
  EXPECT_EQ(out.str(), "");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
