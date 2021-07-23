#ifndef TINY_ALPACA_SERVER_SRC_UTILS_AVR_TIMER_COUNTER_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_AVR_TIMER_COUNTER_H_

// Routines for working with AVR ATmega2560 Timer/Counter units.
//
// TimerCounter[1345]Initialize16BitFastPwm sets the waveform generation mode to
// Fast PWM (mode 14), sets the clock prescaling as requested, and disables all
// of that timer/counter's output channels. The Input Capture Register is set to
// 0xFFFF (maximum uint16), so the smallest duty cycle is 1/65535 (or is it
// 65536?); if the system clock is 16MHz and prescaling is kDivideBy1, the
// smallest pulse duration is approximately 62.5ns.
//
// TimerCounter[1345]SetCompareOutputMode is used to enable and disable output
// for specific channels of the counter.
//
// TimerCounter[1345]SetOutputCompareRegister sets the counter value (0 through
// 65535) at which the output is turned off (it is turned on during the wrap
// around from 65535 to 0).
//
// NOTE: I wrote this code in this fashion with the idea of avoiding pulling in
// code for handling timer/counters that aren't in use. But, that now looks
// annoyingly hard to work with. A rethink is in order.
//
// Author: james.synge@gmail.com

#include "utils/platform.h"

namespace alpaca {

enum class ClockPrescaling : uint8_t {
  kDisabled = 0,
  kDivideBy1 = 0b001,
  kDivideBy8 = 0b010,
  kDivideBy64 = 0b011,
  kDivideBy256 = 0b100,
  kDivideBy1024 = 0b101,
};

size_t PrintValueTo(ClockPrescaling v, Print& out);
const __FlashStringHelper* ToFlashStringHelper(ClockPrescaling v);

enum class FastPwmCompareOutputMode : uint8_t {
  kDisabled = 0b00,
  kNonInvertingMode = 0b10,
  kInvertingMode = 0b11,
};

size_t PrintValueTo(FastPwmCompareOutputMode v, Print& out);
const __FlashStringHelper* ToFlashStringHelper(FastPwmCompareOutputMode v);

enum class TimerCounterChannel {
  A,
  B,
  C,
};

size_t PrintValueTo(TimerCounterChannel v, Print& out);
const __FlashStringHelper* ToFlashStringHelper(TimerCounterChannel v);

////////////////////////////////////////////////////////////////////////////////

// TC16ClockAndTicks encapsulates an ATmega2560's 16-bit timer/counter clock
// select and ticks values, representing some timer interval. The FromXyz
// methods act as factory methods for creating TC16ClockAndTicks instances from
// either a requested period or a requested frequency.
//
// Why? To make it easier to pre-compute the settings for a timer/counter, as
// when performing acceleration and deceleration of a stepper motor.
struct TC16ClockAndTicks {
  static constexpr double kNanoSecondsPerSystemClockCycle =
      1000000000.0 / F_CPU;  // 62.5 for 16MHz AVRs.

  // The maximum value of clock_ticks that a 16-bit timer/counter can represent
  // (i.e. UINT16_MAX). Note that there is also a minimum for the ATmega2560, et
  // al, of 3, which is not dealt with in this code.
  static constexpr uint16_t kMaxClockTicks = 65535;

  // The maximum number of system clock cycles to which a 16-bit timer/counter
  // can count.
  static constexpr uint32_t kMaxSystemClockCycles = kMaxClockTicks * 1024UL;

  // The maximum number of seconds to which a 16-bit timer/counter can count.
  static constexpr double kMaxSeconds =
      static_cast<double>(kMaxSystemClockCycles) / F_CPU;

  // Factory methods for converting a period (duration) into a
  // TC16ClockAndTicks.
  static TC16ClockAndTicks FromSystemClockCycles(uint32_t system_clock_cycles);
  static TC16ClockAndTicks FromNanoSeconds(uint32_t ns);
  static TC16ClockAndTicks FromMicroSeconds(uint32_t us);
  static TC16ClockAndTicks FromSeconds(double s);

  // Factory methods for converting a frequency (events per second) into a
  // TC16ClockAndTicks.
  static TC16ClockAndTicks FromIntegerEventsPerSecond(uint16_t events);
  static TC16ClockAndTicks FromDoubleEventsPerSecond(double events);

  // Returns the duration, in system clock cycles, represented by this instance.
  // This method is expected to be used primarily (or solely) for testing.
  uint32_t ToSystemClockCycles() const;

  // Returns the duration, in seconds, represented by this instance. This method
  // is expected to be used primarily (or solely) for testing.
  double ToSeconds() const;

  // The clock prescaler bits for the TCCRnB register.
  uint8_t ClockSelectBits() const { return static_cast<uint8_t>(clock_select); }

  // Print the fields to out, e.g. "{.cs=DivideBy1, .ticks=123}"
  size_t printTo(Print& out) const;

  ClockPrescaling clock_select;
  uint16_t clock_ticks;
};

// Returns the divisor applied to the system clock for the specified prescaling
// of the system clock.
uint16_t ToClockDivisor(ClockPrescaling prescaling);

////////////////////////////////////////////////////////////////////////////////

void TimerCounter1Initialize16BitFastPwm(ClockPrescaling prescaling);
void TimerCounter1SetCompareOutputMode(TimerCounterChannel channel,
                                       FastPwmCompareOutputMode mode);
void TimerCounter1SetOutputCompareRegister(TimerCounterChannel channel,
                                           uint16_t value);
uint16_t TimerCounter1GetOutputCompareRegister(TimerCounterChannel channel);

void TimerCounter3Initialize16BitFastPwm(ClockPrescaling prescaling);
void TimerCounter3SetCompareOutputMode(TimerCounterChannel channel,
                                       FastPwmCompareOutputMode mode);
void TimerCounter3SetOutputCompareRegister(TimerCounterChannel channel,
                                           uint16_t value);
uint16_t TimerCounter3GetOutputCompareRegister(TimerCounterChannel channel);

void TimerCounter4Initialize16BitFastPwm(ClockPrescaling prescaling);
void TimerCounter4SetCompareOutputMode(TimerCounterChannel channel,
                                       FastPwmCompareOutputMode mode);
void TimerCounter4SetOutputCompareRegister(TimerCounterChannel channel,
                                           uint16_t value);
uint16_t TimerCounter4GetOutputCompareRegister(TimerCounterChannel channel);

void TimerCounter5Initialize16BitFastPwm(ClockPrescaling prescaling);
void TimerCounter5SetCompareOutputMode(TimerCounterChannel channel,
                                       FastPwmCompareOutputMode mode);
void TimerCounter5SetOutputCompareRegister(TimerCounterChannel channel,
                                           uint16_t value);
uint16_t TimerCounter5GetOutputCompareRegister(TimerCounterChannel channel);

////////////////////////////////////////////////////////////////////////////////

// The following classes encapsulate access to the above TimerCounter* methods
// (except for the Initialize methods).

class EnableableByPin {
 public:
  static constexpr uint8_t kNoEnabledPin = 255;

  explicit EnableableByPin(uint8_t enabled_pin);
  EnableableByPin();  // For when the use of the pin is itself disabled.
  bool IsEnabled() const;
  int ReadPin() const;
  uint8_t enabled_pin() const { return enabled_pin_; }

 private:
  const uint8_t enabled_pin_;
};

class TimerCounter1Pwm16Output : public EnableableByPin {
 public:
  TimerCounter1Pwm16Output(TimerCounterChannel channel, uint8_t enabled_pin);
  explicit TimerCounter1Pwm16Output(TimerCounterChannel channel);

  void set_pulse_count(uint16_t value);
  uint16_t get_pulse_count() const;
  constexpr uint16_t max_count() const { return 0xFFFF; }

 private:
  const TimerCounterChannel channel_;
};

class TimerCounter3Pwm16Output : public EnableableByPin {
 public:
  TimerCounter3Pwm16Output(TimerCounterChannel channel, uint8_t enabled_pin);
  explicit TimerCounter3Pwm16Output(TimerCounterChannel channel);

  void set_pulse_count(uint16_t value);
  uint16_t get_pulse_count() const;
  constexpr uint16_t max_count() const { return 0xFFFF; }

 private:
  const TimerCounterChannel channel_;
};

class TimerCounter4Pwm16Output : public EnableableByPin {
 public:
  TimerCounter4Pwm16Output(TimerCounterChannel channel, uint8_t enabled_pin);
  explicit TimerCounter4Pwm16Output(TimerCounterChannel channel);

  void set_pulse_count(uint16_t value);
  uint16_t get_pulse_count() const;
  constexpr uint16_t max_count() const { return 0xFFFF; }

 private:
  const TimerCounterChannel channel_;
};

class TimerCounter5Pwm16Output : public EnableableByPin {
 public:
  TimerCounter5Pwm16Output(TimerCounterChannel channel, uint8_t enabled_pin);
  explicit TimerCounter5Pwm16Output(TimerCounterChannel channel);

  void set_pulse_count(uint16_t value);
  uint16_t get_pulse_count() const;
  constexpr uint16_t max_count() const { return 0xFFFF; }

 private:
  const TimerCounterChannel channel_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_AVR_TIMER_COUNTER_H_
