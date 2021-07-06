#include "utils/inline_literal.h"

#include <string_view>

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace test {
namespace {
using progmem_data::ProgmemStringStorage;

TEST(InlineLiteralTest, RawProgmemString) {
  auto printable =
      ProgmemStringStorage<'h', 'e', 'l', 'l', 'o'>::MakePrintable();
  EXPECT_EQ(printable.size(), 5);

  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 5);
  EXPECT_EQ(out.str(), "hello");
}

TEST(InlineLiteralTest, TASLIT16_String) {
  auto printable = ProgmemStringStorage<TASLIT16(, "Hello!")>::MakePrintable();
  EXPECT_EQ(printable.size(), 16);

  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 16);
  EXPECT_EQ(out.str(), std::string_view("Hello!\0\0\0\0\0\0\0\0\0\0", 16));
}

TEST(InlineLiteralTest, Basic) {
  auto printable = TASLIT("Hey There!");
  EXPECT_EQ(printable.size(), 10);

  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 10);
  EXPECT_EQ(out.str(), "Hey There!");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
