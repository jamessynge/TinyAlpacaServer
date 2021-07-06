#include "utils/moving_average.h"

#include <math.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "absl/random/random.h"
#include "absl/random/uniform_real_distribution.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace test {
namespace {

std::unique_ptr<absl::BitGen> MakeBitGen() {
  absl::StdSeedSeq time_based_seed({absl::ToUnixMicros(absl::Now())});
  return std::make_unique<absl::BitGen>(time_based_seed);
}

absl::BitGen& GetBitGen() {
  static std::unique_ptr<absl::BitGen> bit_gen(MakeBitGen());  // NOLINT
  return *bit_gen;
}

std::vector<double> MakeRandomData(const size_t elems, const double min_value,
                                   const double max_value) {
  EXPECT_LT(min_value, max_value);
  auto distribution =
      absl::uniform_real_distribution<double>(min_value, max_value);

  std::vector<double> data;
  while (data.size() < elems) {
    double r = distribution(GetBitGen());
    data.push_back(r);
  }
  return data;
}

std::vector<double> MakeFakeTemperatureData() {
  const double kMin = -20;
  const double kMax = 40;
  const size_t kElems = 1000;
  auto distribution = absl::uniform_real_distribution<double>(-2.0, 2.0);

  // We start by making a day's worth of data with a full swing of temperatures,
  // then add a 1 degree randomization.
  std::vector<double> temps;
  for (size_t ndx = 0; ndx < kElems; ++ndx) {
    auto x = (2 * 3.1415926 * ndx) / kElems;
    auto y = sin(x) * (kMax - kMin) + (kMin + kMax) / 2;
    temps.push_back(y + distribution(GetBitGen()));
  }
  return temps;
}

TEST(MovingAverageTest, Initial) {
  MovingAverage moving_average;
  EXPECT_FALSE(moving_average.has_average_value());
  EXPECT_EQ(moving_average.last_update_time(), 0);
}

TEST(MovingAverageTest, One) {
  MovingAverage moving_average;
  moving_average.RecordNewValue(123, 345, 567);
  EXPECT_TRUE(moving_average.has_average_value());
  EXPECT_EQ(moving_average.last_update_time(), 345);
  EXPECT_EQ(moving_average.average_value(), 123);
}

TEST(MovingAverageTest, Two) {
  MovingAverage moving_average;

  moving_average.RecordNewValue(123, 1, 10);
  EXPECT_TRUE(moving_average.has_average_value());
  EXPECT_EQ(moving_average.last_update_time(), 1);
  EXPECT_EQ(moving_average.average_value(), 123);

  moving_average.RecordNewValue(123, 2, 10);
  EXPECT_TRUE(moving_average.has_average_value());
  EXPECT_EQ(moving_average.last_update_time(), 2);
  EXPECT_EQ(moving_average.average_value(), 123);
}

TEST(MovingAverageTest, Many) {
  const int kAveragePeriod = 10;
  auto data = MakeFakeTemperatureData();
  for (int ndx = 0; ndx < data.size(); ++ndx) {
    LOG(INFO) << "data[" << ndx << "] " << data[ndx];
  }
  MovingAverage moving_average;
  double sum_of_deltas = 0;
  double sum_of_diffs = 0;
  for (int ndx = 0; ndx < data.size(); ++ndx) {
    moving_average.RecordNewValue(data[ndx], ndx, kAveragePeriod);
    EXPECT_TRUE(moving_average.has_average_value());
    double v = 0;
    int count = 0;
    int ndx2 = ndx;
    int limit = std::max(0, ndx - kAveragePeriod + 1);
    do {
      v += data[ndx2];
      ++count;
    } while (--ndx2 >= limit);
    v /= count;
    auto delta = moving_average.average_value() - v;
    sum_of_deltas += delta;
    auto diff = abs(delta);
    sum_of_diffs += diff;
    LOG(INFO) << "data[" << ndx << "]\t" << data[ndx] << " \t moving_average "
              << moving_average.average_value() << " \t diff " << diff;
    if (ndx > kAveragePeriod) {
      EXPECT_LT(diff, 2);
    }
  }
  EXPECT_EQ(moving_average.last_update_time(), data.size() - 1);
  EXPECT_LT(sum_of_diffs / data.size(), 0.5);
}

}  // namespace
}  // namespace test
}  // namespace alpaca
