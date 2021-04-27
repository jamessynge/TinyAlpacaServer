#ifndef TINY_ALPACA_SERVER_SRC_UTILS_AVR_TIMER_COUNTER_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_AVR_TIMER_COUNTER_H_

// Routines for working with AVR ATmega2560 Timer/Counter units.
//
// TimerCounter[1345]Initialize16BitFastPwm sets the waveform generation mode to
// Fast PWM (mode 14), sets the clock prescaling as requested, and disables all
// outputs. The Input Capture Register is set to 0xFFFF (maximum uint16), so the
// smallest duty cycle is 1/65535 (or is it 65536?); if the system clock is
// 16MHz and prescaling is ClockPrescaling::kAsIs, the smallest pulse duration
// is approximately 62.5ns.
//
// TimerCounter[1345]SetCompareOutputMode is used to enable and disable output
// for specific channels of the counter.
//
// TimerCounter[1345]SetOutputCompareRegister sets the counter value (0 through
// 65535) at which the output is turned off (it is turned on during the wrap
// around from 65535 to 0).

#include "utils/platform.h"
#include "utils/printable_progmem_string.h"

namespace alpaca {

enum class ClockPrescaling : uint8_t {
  kAsIs = 0b001,
  kDivideBy8 = 0b010,
  kDivideBy64 = 0b011,
  kDivideBy256 = 0b100,
  kDivideBy1024 = 0b101,
};

size_t PrintValueTo(ClockPrescaling v, Print& out);
PrintableProgmemString ToPrintableProgmemString(ClockPrescaling v);

enum class FastPwmCompareOutputMode : uint8_t {
  kDisabled = 0b00,
  kNonInvertingMode = 0b10,
  kInvertingMode = 0b11,
};

size_t PrintValueTo(FastPwmCompareOutputMode v, Print& out);
PrintableProgmemString ToPrintableProgmemString(FastPwmCompareOutputMode v);

enum class TimerCounterChannel {
  A,
  B,
  C,
};

size_t PrintValueTo(TimerCounterChannel v, Print& out);
PrintableProgmemString ToPrintableProgmemString(TimerCounterChannel v);

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

// The following classes encapsulate access to the above TimerCounter* methods
// (except for the Initialize methods).

class TimerCounter1Pwm16Output {
 public:
  TimerCounter1Pwm16Output(TimerCounterChannel channel, uint8_t enabled_pin);
  explicit TimerCounter1Pwm16Output(TimerCounterChannel channel);

  bool is_enabled() const;
  void set_pulse_count(uint16_t value);
  uint16_t get_pulse_count() const;
  constexpr uint16_t max_count() const { return 0xFFFF; }

 private:
  const TimerCounterChannel channel_;
  const uint8_t enabled_pin_;
};

class TimerCounter3Pwm16Output {
 public:
  TimerCounter3Pwm16Output(TimerCounterChannel channel, uint8_t enabled_pin);
  explicit TimerCounter3Pwm16Output(TimerCounterChannel channel);

  bool is_enabled() const;
  void set_pulse_count(uint16_t value);
  uint16_t get_pulse_count() const;
  constexpr uint16_t max_count() const { return 0xFFFF; }

 private:
  const TimerCounterChannel channel_;
  const uint8_t enabled_pin_;
};

class TimerCounter4Pwm16Output {
 public:
  TimerCounter4Pwm16Output(TimerCounterChannel channel, uint8_t enabled_pin);
  explicit TimerCounter4Pwm16Output(TimerCounterChannel channel);

  bool is_enabled() const;
  void set_pulse_count(uint16_t value);
  uint16_t get_pulse_count() const;
  constexpr uint16_t max_count() const { return 0xFFFF; }

 private:
  const TimerCounterChannel channel_;
  const uint8_t enabled_pin_;
};

class TimerCounter5Pwm16Output {
 public:
  TimerCounter5Pwm16Output(TimerCounterChannel channel, uint8_t enabled_pin);
  explicit TimerCounter5Pwm16Output(TimerCounterChannel channel);

  bool is_enabled() const;
  void set_pulse_count(uint16_t value);
  uint16_t get_pulse_count() const;
  constexpr uint16_t max_count() const { return 0xFFFF; }

 private:
  const TimerCounterChannel channel_;
  const uint8_t enabled_pin_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_AVR_TIMER_COUNTER_H_
