#include "literals.h"

#include "googletest/gtest.h"
#include "string_compare.h"
#include "string_view.h"

namespace alpaca {
namespace {

TEST(LiteralsTest, Basics) {
  const char kPUT[] = "PUT";
  EXPECT_EQ(Literals::PUT(), StringView(kPUT));
  EXPECT_NE(Literals::PUT().prog_data_for_tests(), kPUT);
  EXPECT_NE(Literals::PUT().prog_data_for_tests(), "PUT");

  auto literal1 = Literals::ClientId();
  EXPECT_EQ(literal1, StringView("ClientId"));
  auto literal2 = Literals::ClientId();
  EXPECT_EQ(literal2, StringView("ClientId"));
  EXPECT_EQ(literal1.prog_data_for_tests(), literal2.prog_data_for_tests());
}

// Generate a trivial test for each Literal in literals.inc.

#ifdef DEFINE_LITERAL
#undef DEFINE_LITERAL
#endif  // DEFINE_LITERAL

#define DEFINE_LITERAL(name, literal)               \
  TEST(GeneratedLiteralsTest, name##_IsAvailable) { \
    std::string expected(literal);                  \
    StringView view(expected);                      \
    EXPECT_EQ(Literals::name(), view);              \
  }
#include "literals.inc"
#undef DEFINE_LITERAL

}  // namespace
}  // namespace alpaca
