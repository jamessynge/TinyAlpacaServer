#include "literals.h"

#include <McuCore.h>

#include <string>

#include "gtest/gtest.h"
#include "mcucore/extras/test_tools/print_value_to_std_string.h"
#include "mcucore/extras/test_tools/string_view_utils.h"

namespace alpaca {
namespace test {
namespace {

using ::mcucore::test::MakeStringView;

TEST(LiteralsTest, Basics) {
  const char kPUT[] = "PUT";
  EXPECT_EQ(ProgmemStringViews::PUT(), mcucore::StringView(kPUT));
  EXPECT_EQ(mcucore::PrintValueToStdString(ProgmemStringViews::GET()), "GET");

  auto literal1 = ProgmemStringViews::ClientID();
  EXPECT_EQ(literal1, mcucore::StringView("ClientID"));
  auto literal2 = ProgmemStringViews::ClientID();
  EXPECT_EQ(literal2, mcucore::StringView("ClientID"));
  EXPECT_TRUE(literal1.Identical(literal2));
}

// Generate a trivial test for each mcucore::ProgmemStringView in literals.inc.

#ifdef TAS_DEFINE_PROGMEM_LITERAL
#undef TAS_DEFINE_PROGMEM_LITERAL
#endif  // TAS_DEFINE_PROGMEM_LITERAL

#define TAS_DEFINE_PROGMEM_LITERAL(name, literal)        \
  TEST(GeneratedLiteralsTest, name##_IsAvailable) {      \
    std::string expected(literal);                       \
    mcucore::StringView view = MakeStringView(expected); \
    EXPECT_EQ(ProgmemStringViews::name(), view);         \
  }
#include "literals.inc"
#undef TAS_DEFINE_PROGMEM_LITERAL

}  // namespace
}  // namespace test
}  // namespace alpaca
