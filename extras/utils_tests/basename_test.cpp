#include "utils/basename.h"

#include "gtest/gtest.h"
#include "mcucore/extrastest_tools/print_to_std_string.h"
#include "utils/inline_literal.h"

namespace alpaca {

namespace test {
namespace {

TEST(TasBasenameTest, NoSlash) {
  mcucore::test::PrintToStdString out;
  out.print(TAS_BASENAME("foo.bar.baz"));
  EXPECT_EQ(out.str(), "foo.bar.baz");
}

TEST(TasBasenameTest, LeadingSlash) {
  mcucore::test::PrintToStdString out;
  out.print(TAS_BASENAME("/bar.baz"));
  EXPECT_EQ(out.str(), "bar.baz");
}

TEST(TasBasenameTest, LeadingSlashes) {
  mcucore::test::PrintToStdString out;
  out.print(TAS_BASENAME("//bar.baz"));
  EXPECT_EQ(out.str(), "bar.baz");
}

TEST(TasBasenameTest, MiddleSlash) {
  mcucore::test::PrintToStdString out;
  out.print(TAS_BASENAME("foo/bar.baz"));
  EXPECT_EQ(out.str(), "bar.baz");
}

TEST(TasBasenameTest, LeadingAndMiddleSlashes) {
  mcucore::test::PrintToStdString out;
  out.print(TAS_BASENAME("//foo//bar/baz.cc"));
  EXPECT_EQ(out.str(), "baz.cc");
}

TEST(TasBasenameTest, TrailingSlash) {
  mcucore::test::PrintToStdString out;
  out.print(TAS_BASENAME("foo.bar.baz/"));
  EXPECT_EQ(out.str(), "");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
