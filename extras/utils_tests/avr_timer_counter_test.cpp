#include "utils/avr_timer_counter.h"

#include <ostream>

#include "absl/time/time.h"
#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "utils/o_print_stream.h"

namespace alpaca {

bool operator==(const TC16ClockAndTicks& a, const TC16ClockAndTicks& b) {
  return a.clock_select == b.clock_select && a.clock_ticks == b.clock_ticks;
}

std::ostream& operator<<(std::ostream& out, const TC16ClockAndTicks& v) {
  return out << "{.clock_select = ClockPrescaling::"
             << PrintValueToStdString(v.clock_select)
             << ", .clock_ticks = " << v.clock_ticks
             << ", .seconds = " << v.ToSeconds()
             << ", .system_clock_cycles = " << v.ToSystemClockCycles() << "}";
}

namespace test {
namespace {

constexpr double kUnitNs = TC16ClockAndTicks::kNanoSecondsPerSystemClockCycle;

// The tolerance in our EXPECT_NEAR tests needs to vary based on the
// ClockPrescaler.
double SecondsTolerance(const TC16ClockAndTicks& ct) {
  double allowance_ns = (ToClockDivisor(ct.clock_select) + 0.5) * kUnitNs;
  return allowance_ns / 1000000000.0;
}

TC16ClockAndTicks MakeFromDuration(const absl::Duration d) {
  auto ns = absl::ToInt64Nanoseconds(d);
  // LOG_IF(INFO, d > absl::Milliseconds(26))
  //     << "d: "<<d<<", ns: " << ns;
  CHECK_GT(ns, 0);
  CHECK_LE(ns, UINT32_MAX);
  auto ct = TC16ClockAndTicks::FromNanoSeconds(ns);
  EXPECT_NE(ct.clock_select, ClockPrescaling::kDisabled)
      << "duration: " << d << ", ns: " << ns << ", ct: " << ct;
  EXPECT_NEAR(ct.ToSeconds(), absl::ToDoubleSeconds(d), SecondsTolerance(ct));

  EXPECT_NEAR(ct.ToSystemClockCycles(), (ns + kUnitNs / 2) / kUnitNs,
              ToClockDivisor(ct.clock_select));

  auto us = absl::ToInt64Microseconds(d + absl::Nanoseconds(500));
  if (us > 0) {
    auto ct2 = TC16ClockAndTicks::FromMicroSeconds(us);
    EXPECT_NE(ct2.clock_select, ClockPrescaling::kDisabled)
        << "duration: " << d << ", us: " << us << ", ct2: " << ct2;
    double s = us / 1000000.0;
    EXPECT_NEAR(ct2.ToSeconds(), s, SecondsTolerance(ct2))
        << "duration: " << d << ", us: " << us << ", ct2: " << ct2;
  }

  {
    auto s = absl::ToDoubleSeconds(d);
    auto ct2 = TC16ClockAndTicks::FromSeconds(s);
    EXPECT_NE(ct2.clock_select, ClockPrescaling::kDisabled)
        << "duration: " << d << ", s: " << s << ", ct2: " << ct2;
    EXPECT_NEAR(ct2.ToSeconds(), s, SecondsTolerance(ct2))
        << "duration: " << d << ", s: " << s << ", ct2: " << ct2;
  }

  auto double_events_per_second = 1.0 / absl::ToDoubleSeconds(d);
  if (0.2 < double_events_per_second && double_events_per_second <= F_CPU) {
    auto ct2 =
        TC16ClockAndTicks::FromDoubleEventsPerSecond(double_events_per_second);
    EXPECT_NE(ct2.clock_select, ClockPrescaling::kDisabled)
        << "duration: " << d
        << ", double_events_per_second: " << double_events_per_second
        << ", ct2: " << ct2;
    EXPECT_NEAR(ct2.ToSeconds(), absl::ToDoubleSeconds(d),
                SecondsTolerance(ct2))
        << "duration: " << d
        << ", double_events_per_second: " << double_events_per_second
        << ", ct2: " << ct2;
  }

  uint64_t integer_events_per_second = double_events_per_second + 0.5;
  if (1 <= integer_events_per_second && integer_events_per_second < 65536 &&
      integer_events_per_second <= F_CPU) {
    auto ct2 = TC16ClockAndTicks::FromIntegerEventsPerSecond(
        integer_events_per_second);
    EXPECT_NE(ct2.clock_select, ClockPrescaling::kDisabled)
        << "duration: " << d
        << ", integer_events_per_second: " << integer_events_per_second
        << ", ct2: " << ct2;
    // Duration d is not necessarily very near (i.e. within 1us) of the
    // requested frequency, so compute d2 from the requested frequency.
    auto d2 = absl::Seconds(1) / integer_events_per_second;
    EXPECT_NEAR(ct2.ToSeconds(), absl::ToDoubleSeconds(d2),
                SecondsTolerance(ct2))
        << "duration: " << d
        << ", integer_events_per_second: " << integer_events_per_second
        << ", ct2: " << ct2;
  }

  return ct;
}

TC16ClockAndTicks MakeFromNanoSeconds(uint32_t ns) {
  return MakeFromDuration(absl::Nanoseconds(ns));
}

TC16ClockAndTicks MakeFromSystemClockCycles(uint32_t system_clock_cycles) {
  auto ct = TC16ClockAndTicks::FromSystemClockCycles(system_clock_cycles);
  EXPECT_NE(ct.clock_select, ClockPrescaling::kDisabled);
  EXPECT_NEAR(system_clock_cycles, ct.ToSystemClockCycles(),
              ToClockDivisor(ct.clock_select));

  const double seconds = (system_clock_cycles) / static_cast<double>(F_CPU);
  const double step_size =
      (ToClockDivisor(ct.clock_select)) / static_cast<double>(F_CPU);
  EXPECT_NEAR(ct.ToSeconds(), seconds, step_size);

  auto ct2 = MakeFromDuration(absl::Seconds(1) * seconds);
  if (seconds < 0.001) {
    EXPECT_NEAR(ct.ToSystemClockCycles(), ct2.ToSystemClockCycles(), 10) << ct;
  } else {
    EXPECT_NEAR(ct.ToSystemClockCycles(), ct2.ToSystemClockCycles(),
                ct2.ToSystemClockCycles() / 1000)
        << ct;
  }

  return ct;
}

TEST(TC16ClockAndTicksTest, FromNanoSeconds_Unit) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy1,
                             .clock_ticks = 1};
  EXPECT_EQ(MakeFromNanoSeconds(kUnitNs), expected);
}

TEST(TC16ClockAndTicksTest, FromNanoSeconds_65535_Units) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy1,
                             .clock_ticks = 65535};
  EXPECT_EQ(MakeFromNanoSeconds(kUnitNs * 65535), expected);
}

TEST(TC16ClockAndTicksTest, FromNanoSeconds_65536_Units) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy8,
                             .clock_ticks = 8192};
  EXPECT_EQ(MakeFromNanoSeconds(kUnitNs * 65536), expected);
}

// Beyond this point we expect sufficient rounding in the nanoseconds value that
// we instead compare the derived cycle count to confirm that it is near the
// expected value.

TEST(TC16ClockAndTicksTest, FromNanoSeconds_524280_Units) {
  auto ct = MakeFromNanoSeconds(kUnitNs * 524280);  // 65535 * 8
  EXPECT_NEAR(ct.ToSystemClockCycles(), 524280, 10) << ct;
}

TEST(TC16ClockAndTicksTest, FromNanoSeconds_524281_Units) {
  auto ct = MakeFromNanoSeconds(kUnitNs * 524281);  // 65535 * 8 + 1
  EXPECT_NEAR(ct.ToSystemClockCycles(), 524281, 10) << ct;
}

TEST(TC16ClockAndTicksTest, FromNanoSeconds_4194240_Units) {
  auto ct = MakeFromNanoSeconds(kUnitNs * 4194240);  // 65535 * 64
  EXPECT_NEAR(ct.ToSystemClockCycles(), 4194240, 10) << ct;
}

TEST(TC16ClockAndTicksTest, FromNanoSeconds_4194241_Units) {
  auto ct = MakeFromNanoSeconds(kUnitNs * 4194241);  // 65535 * 64 + 1
  EXPECT_NEAR(ct.ToSystemClockCycles(), 4194241, 10) << ct;
}

TEST(TC16ClockAndTicksTest, FromNanoSeconds_16776960_Units) {
  auto ct = MakeFromNanoSeconds(kUnitNs * 16776960);  // 65535 * 256
  EXPECT_NEAR(ct.ToSystemClockCycles(), 16776960, 10) << ct;
}

TEST(TC16ClockAndTicksTest, FromNanoSeconds_16776961_Units) {
  auto ct = MakeFromNanoSeconds(kUnitNs * 16776961);  // 65535 * 256 + 1
  EXPECT_NEAR(ct.ToSystemClockCycles(), 16776961, 10) << ct;
}

TEST(TC16ClockAndTicksTest, FromNanoSeconds_67107840_Units) {
  auto ct = MakeFromNanoSeconds(kUnitNs * 67107840);  // 65535 * 1024
  EXPECT_NEAR(ct.ToSystemClockCycles(), 67107840, 10) << ct;
}

// FromSystemClockCycles

TEST(TC16ClockAndTicksTest, FromSystemClockCycles_1) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy1,
                             .clock_ticks = 1};
  EXPECT_EQ(MakeFromSystemClockCycles(1), expected);
}

TEST(TC16ClockAndTicksTest, FromSystemClockCycles_65535) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy1,
                             .clock_ticks = 65535};
  EXPECT_EQ(MakeFromSystemClockCycles(65535), expected);
}

TEST(TC16ClockAndTicksTest, FromSystemClockCycles_65536) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy8,
                             .clock_ticks = 8192};
  EXPECT_EQ(MakeFromSystemClockCycles(65536), expected);
}

TEST(TC16ClockAndTicksTest, FromSystemClockCycles_524280) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy8,
                             .clock_ticks = 65535};
  EXPECT_EQ(MakeFromSystemClockCycles(524280),  // 65535 * 8
            expected);
}

TEST(TC16ClockAndTicksTest, FromSystemClockCycles_524281) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy64,
                             .clock_ticks = 8191};
  EXPECT_EQ(MakeFromSystemClockCycles(524281),  // 65535 * 8 + 1
            expected);
}

TEST(TC16ClockAndTicksTest, FromSystemClockCycles_4194240) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy64,
                             .clock_ticks = 65535};
  EXPECT_EQ(MakeFromSystemClockCycles(4194240),  // 65535 * 64
            expected);
}

TEST(TC16ClockAndTicksTest, FromSystemClockCycles_4194241) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy256,
                             .clock_ticks = 16383};
  EXPECT_EQ(MakeFromSystemClockCycles(4194241),  // 65535 * 64 + 1
            expected);
}

TEST(TC16ClockAndTicksTest, FromSystemClockCycles_16776960) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy256,
                             .clock_ticks = 65535};
  EXPECT_EQ(MakeFromSystemClockCycles(16776960),  // 65535 * 256
            expected);
}

TEST(TC16ClockAndTicksTest, FromSystemClockCycles_16776961) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy1024,
                             .clock_ticks = 16383};
  EXPECT_EQ(MakeFromSystemClockCycles(16776961),  // 65535 * 256 + 1
            expected);
}

TEST(TC16ClockAndTicksTest, FromSystemClockCycles_67107840) {
  TC16ClockAndTicks expected{.clock_select = ClockPrescaling::kDivideBy1024,
                             .clock_ticks = 65535};
  EXPECT_EQ(MakeFromSystemClockCycles(67107840),  // 65535 * 1024
            expected);
}

// Verifies that ~all possible values that can be represented are in order.
TEST(TC16ClockAndTicksTest, CountUpSystemClockCycles) {
  TC16ClockAndTicks last_ct;
  uint32_t cycles = 1;
  while (true) {
    auto ct = MakeFromSystemClockCycles(cycles);
    ASSERT_NE(ct.clock_select, ClockPrescaling::kDisabled);
    ASSERT_GT(ct.ToSystemClockCycles(), 0);
    ASSERT_GT(ct.ToSeconds(), 0);
    if (cycles >= 3) {
      // ATmega2560 docs say 3 ticks is the minimum for some modes of interest,
      // so let's ensure we avoid those at higher clock prescaling values.
      ASSERT_GE(ct.clock_ticks, 3);
    }
    if (cycles > 1) {
      ASSERT_GT(ct.ToSystemClockCycles(), last_ct.ToSystemClockCycles());
      ASSERT_GT(ct.ToSeconds(), last_ct.ToSeconds());
      if (last_ct.clock_select == ct.clock_select) {
        ASSERT_GE(ct.clock_ticks, last_ct.clock_ticks);
      } else {
        ASSERT_GT(ct.clock_select, last_ct.clock_select);
        ASSERT_LT(ct.clock_ticks, last_ct.clock_ticks);
      }
    }
    last_ct = ct;

    // Increment by the clock divisor (i.e. 1, then 8 after a while, then 64,
    // etc.), which keeps the run time for this reasonable. It is also necessary
    // for the asserts above; i.e. if we increment by one cycle when the divisor
    // is 1024, then the resulting TC16ClockAndTicks is likely to be unchanged
    // relative to the previous value.
    cycles += ToClockDivisor(ct.clock_select);
    if (cycles >= TC16ClockAndTicks::kMaxSystemClockCycles) {
      break;
    }
  }
}

}  // namespace
}  // namespace test
}  // namespace alpaca
