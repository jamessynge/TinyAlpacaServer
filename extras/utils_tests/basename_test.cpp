#include "utils/basename.h"

#include "experimental/users/jamessynge/arduino/hostuino/extras/test_tools/print_to_std_string.h"
#include "googletest/gtest.h"
#include "utils/inline_literal.h"

namespace alpaca {

namespace test {
namespace {

TEST(TasBasenameTest, NoSlash) {
  hostuino::PrintToStdString out;
  out.print(TAS_BASENAME("foo.bar.baz"));
  EXPECT_EQ(out.str(), "foo.bar.baz");
}

TEST(TasBasenameTest, LeadingSlash) {
  hostuino::PrintToStdString out;
  out.print(TAS_BASENAME("/bar.baz"));
  EXPECT_EQ(out.str(), "bar.baz");
}

TEST(TasBasenameTest, LeadingSlashes) {
  hostuino::PrintToStdString out;
  out.print(TAS_BASENAME("//bar.baz"));
  EXPECT_EQ(out.str(), "bar.baz");
}

TEST(TasBasenameTest, MiddleSlash) {
  hostuino::PrintToStdString out;
  out.print(TAS_BASENAME("foo/bar.baz"));
  EXPECT_EQ(out.str(), "bar.baz");
}

TEST(TasBasenameTest, LeadingAndMiddleSlashes) {
  hostuino::PrintToStdString out;
  out.print(TAS_BASENAME("//foo//bar/baz.cc"));
  EXPECT_EQ(out.str(), "baz.cc");
}

TEST(TasBasenameTest, TrailingSlash) {
  hostuino::PrintToStdString out;
  out.print(TAS_BASENAME("foo.bar.baz/"));
  EXPECT_EQ(out.str(), "");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
