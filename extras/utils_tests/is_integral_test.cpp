#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "utils/traits/type_traits.h"

namespace alpaca {
namespace {

struct SomeStructType {};
enum SomeEnumType : int {};

TEST(IsIntegralTest, IsIntegral) {
  EXPECT_EQ(is_integral<bool>::value, true);
  EXPECT_EQ(is_integral<const int>::value, true);
  EXPECT_EQ(is_integral<short>::value, true);  // NOLINT
  EXPECT_EQ(is_integral<const int32_t>::value, true);

  EXPECT_EQ(is_integral<double>::value, false);
  EXPECT_EQ(is_integral<SomeStructType>::value, false);
  EXPECT_EQ(is_integral<SomeEnumType>::value, false);
}

// TEST(IsIntegralTest, IsIntegralV) {
//   EXPECT_EQ(is_integral_v<bool>, true);
//   EXPECT_EQ(is_integral_v<const int>, true);
//   EXPECT_EQ(is_integral_v<short>, true);  // NOLINT
//   EXPECT_EQ(is_integral_v<const int32_t>, true);

//   EXPECT_EQ(is_integral_v<double>, false);
//   EXPECT_EQ(is_integral_v<SomeStructType>, false);
//   EXPECT_EQ(is_integral_v<SomeEnumType>, false);
// }

}  // namespace
}  // namespace alpaca
