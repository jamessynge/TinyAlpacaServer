#include "literals.h"

#include <string>

#include "gtest/gtest.h"
#include "literal.h"
#include "mcucore/extrastest_tools/string_view_utils.h"
#include "string_compare.h"
#include "string_view.h"

namespace alpaca {
namespace test {
namespace {

using ::mcucore::test::MakeStringView;

TEST(LiteralsTest, Basics) {
  const char kPUT[] = "PUT";
  EXPECT_EQ(Literals::PUT(), mcucore::StringView(kPUT));
  EXPECT_NE(Literals::PUT().prog_data_for_tests(), kPUT);
  EXPECT_NE(Literals::PUT().prog_data_for_tests(), "PUT");

  auto literal1 = Literals::ClientID();
  EXPECT_EQ(literal1, mcucore::StringView("ClientID"));
  auto literal2 = Literals::ClientID();
  EXPECT_EQ(literal2, mcucore::StringView("ClientID"));
  EXPECT_EQ(literal1.prog_data_for_tests(), literal2.prog_data_for_tests());
}

// Generate a trivial test for each mcucore::Literal in literals.inc.

#ifdef TAS_DEFINE_BUILTIN_LITERAL
#undef TAS_DEFINE_BUILTIN_LITERAL
#endif  // TAS_DEFINE_BUILTIN_LITERAL

#define TAS_DEFINE_BUILTIN_LITERAL(name, literal)        \
  TEST(GeneratedLiteralsTest, name##_IsAvailable) {      \
    std::string expected(literal);                       \
    mcucore::StringView view = MakeStringView(expected); \
    EXPECT_EQ(Literals::name(), view);                   \
  }
#include "literals.inc"
#undef TAS_DEFINE_BUILTIN_LITERAL

}  // namespace
}  // namespace test
}  // namespace alpaca
