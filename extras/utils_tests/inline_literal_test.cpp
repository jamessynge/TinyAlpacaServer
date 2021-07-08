#include "utils/inline_literal.h"

#include <string_view>

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace test {
namespace {
using ::alpaca::progmem_data::ProgmemStringStorage;

TEST(InlineLiteralTest, RawProgmemString) {
  using Type = ProgmemStringStorage<'H', 'E', 'L', 'L', 'O'>;
  auto printable = alpaca::progmem_data::MakePrintable<Type>();
  EXPECT_EQ(printable.size(), 5);
  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 5);
  EXPECT_EQ(out.str(), "HELLO");
}

TEST(InlineLiteralTest, TASLIT16_String) {
  using Type = ProgmemStringStorage<TASLIT16(, "Hello!")>;
  auto printable = alpaca::progmem_data::MakePrintable<Type>();
  EXPECT_EQ(printable.size(), 16);
  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 16);
  EXPECT_EQ(out.str(), std::string_view("Hello!\0\0\0\0\0\0\0\0\0\0", 16));
}

TEST(InlineLiteralTest, TasLit) {
  auto printable = TASLIT("Hey There!");
  EXPECT_EQ(printable.size(), 10);
  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 10);
  EXPECT_EQ(out.str(), "Hey There!");
}

TEST(InlineLiteralTest, TasFlashstr) {
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR("Echo, echo, echo, echo, echo")), 28);
  EXPECT_EQ(out.str(), "Echo, echo, echo, echo, echo");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
