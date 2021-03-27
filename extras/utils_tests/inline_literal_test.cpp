#include "utils/inline_literal.h"

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace {

TEST(InlineLiteralTest, Basic) {
  // Not yet working. Sigh.
  TASLIT("a");
  // EXPECT_THAT(value, "a");
}

}  // namespace
}  // namespace alpaca
