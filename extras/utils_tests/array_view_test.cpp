#include "utils/array_view.h"

#include <stdint.h>

#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace test {
namespace {

constexpr double kZeroDoubles[] = {};
constexpr float kOneFloat[] = {1};
constexpr int16_t kThreeShorts[] = {10, 20, 30};

TEST(ArrayViewTest, EmptyArrayView) {
  ArrayView<const double> array(kZeroDoubles, 0);
  EXPECT_EQ(array.data(), nullptr);
  EXPECT_EQ(array.size(), 0);
  int count = 0;
  for (const auto& value : array) {
    ++count;
    EXPECT_TRUE(false) << "How is there a value (" << value << ")?";
  }
  EXPECT_EQ(count, 0);
}

TEST(ArrayViewTest, Size1ArrayView) {
  ArrayView<const float> array(kOneFloat);
  EXPECT_EQ(array.data(), kOneFloat);
  EXPECT_EQ(array.size(), 1);
  EXPECT_THAT(array, testing::ElementsAre(1.0));
}

TEST(ArrayViewTest, Size3ArrayView) {
  ArrayView<const int16_t> array(kThreeShorts);
  EXPECT_EQ(array.data(), kThreeShorts);
  EXPECT_EQ(array.size(), 3);
  EXPECT_THAT(array, testing::ElementsAre(10, 20, 30));
}

TEST(ArrayViewTest, MakeArrayView) {
  double underlying_array[] = {3.1416, 1.1414, 2.7183};
  auto array = MakeArrayView(underlying_array);
  EXPECT_EQ(array.data(), underlying_array);
  EXPECT_EQ(array.size(), 3);
  EXPECT_THAT(array, testing::UnorderedElementsAre(1.1414, 2.7183, 3.1416));
}

TEST(ArrayViewTest, MakeArrayViewFromConst) {
  const uint32_t underlying_array[] = {0, 1, 2};
  auto array = MakeArrayView(underlying_array);
  EXPECT_EQ(array.data(), underlying_array);
  EXPECT_EQ(array.size(), 3);
  EXPECT_THAT(array, testing::UnorderedElementsAre(0, 1, 2));
}

}  // namespace
}  // namespace test
}  // namespace alpaca
