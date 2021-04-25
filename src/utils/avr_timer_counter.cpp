#include "utils/avr_timer_counter.h"

#include "utils/inline_literal.h"
#include "utils/logging.h"
#include "utils/print_misc.h"

namespace alpaca {

size_t PrintValueTo(ClockPrescaling v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("ClockPrescaling"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(FastPwmCompareOutputMode v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("FastPwmCompareOutputMode"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(TimerCounterChannel v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("TimerCounterChannel"),
                                 static_cast<uint32_t>(v), out);
}

////////////////////////////////////////////////////////////////////////////////

PrintableProgmemString ToPrintableProgmemString(ClockPrescaling v) {
  switch (v) {
    case ClockPrescaling::kAsIs:
      return TASLIT("AsIs");
    case ClockPrescaling::kDivideBy8:
      return TASLIT("DivideBy8");
    case ClockPrescaling::kDivideBy64:
      return TASLIT("DivideBy64");
    case ClockPrescaling::kDivideBy256:
      return TASLIT("DivideBy256");
    case ClockPrescaling::kDivideBy1024:
      return TASLIT("DivideBy1024");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(FastPwmCompareOutputMode v) {
  switch (v) {
    case FastPwmCompareOutputMode::kDisabled:
      return TASLIT("Disabled");
    case FastPwmCompareOutputMode::kNonInvertingMode:
      return TASLIT("NonInvertingMode");
    case FastPwmCompareOutputMode::kInvertingMode:
      return TASLIT("InvertingMode");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(TimerCounterChannel v) {
  switch (v) {
    case TimerCounterChannel::A:
      return TASLIT("A");
    case TimerCounterChannel::B:
      return TASLIT("B");
    case TimerCounterChannel::C:
      return TASLIT("C");
  }
  return PrintableProgmemString();
}

////////////////////////////////////////////////////////////////////////////////

namespace {
constexpr uint8_t kKeepAllExceptChannelA =
    static_cast<uint8_t>(~(0b11 << COM1A0));
constexpr uint8_t kKeepAllExceptChannelB =
    static_cast<uint8_t>(~(0b11 << COM1B0));
constexpr uint8_t kKeepAllExceptChannelC =
    static_cast<uint8_t>(~(0b11 << COM1C0));
}  // namespace

void TimerCounter1Initialize16BitFastPwm(ClockPrescaling prescaling) {
  noInterrupts();
  TCCR1A = 1 << WGM11;
  TCCR1B =
      (1 << WGM13) | (1 << WGM12) | static_cast<uint8_t>(prescaling) << CS10;
  ICR1 = UINT16_MAX;
  interrupts();
}

void TimerCounter1SetCompareOutputMode(TimerCounterChannel channel,
                                       FastPwmCompareOutputMode mode) {
  uint8_t keep_mask, set_mask;
  switch (channel) {
    case TimerCounterChannel::A:
      keep_mask = kKeepAllExceptChannelA;
      set_mask = static_cast<uint8_t>(mode) << COM1A0;
      break;
    case TimerCounterChannel::B:
      keep_mask = kKeepAllExceptChannelB;
      set_mask = static_cast<uint8_t>(mode) << COM1B0;
      break;
    case TimerCounterChannel::C:
      keep_mask = kKeepAllExceptChannelC;
      set_mask = static_cast<uint8_t>(mode) << COM1C0;
      break;
    default:
      TAS_DCHECK(false) << "Unknown channel " << channel;
  }
  noInterrupts();
  TCCR1A = (TCCR1A & keep_mask) | set_mask;
  interrupts();
}

void TimerCounter1SetOutputCompareRegister(TimerCounterChannel channel,
                                           uint16_t value) {
  switch (channel) {
    case TimerCounterChannel::A:
      OCR1A = value;
      break;
    case TimerCounterChannel::B:
      OCR1B = value;
      break;
    case TimerCounterChannel::C:
      OCR1C = value;
      break;
    default:
      TAS_DCHECK(false) << "Unknown channel " << channel;
  }
}

uint16_t TimerCounter1GetOutputCompareRegister(TimerCounterChannel channel) {
  switch (channel) {
    case TimerCounterChannel::A:
      return OCR1A;
    case TimerCounterChannel::B:
      return OCR1B;
    case TimerCounterChannel::C:
      return OCR1C;
    default:
      TAS_DCHECK(false) << "Unknown channel " << channel;
      return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

void TimerCounter3Initialize16BitFastPwm(ClockPrescaling prescaling) {
  noInterrupts();
  TCCR3A = 1 << WGM11;
  TCCR3B =
      (1 << WGM13) | (1 << WGM12) | static_cast<uint8_t>(prescaling) << CS10;
  ICR3 = UINT16_MAX;
  interrupts();
}

void TimerCounter3SetCompareOutputMode(TimerCounterChannel channel,
                                       FastPwmCompareOutputMode mode) {
  uint8_t keep_mask, set_mask;
  switch (channel) {
    case TimerCounterChannel::A:
      keep_mask = kKeepAllExceptChannelA;
      set_mask = static_cast<uint8_t>(mode) << COM3A0;
      break;
    case TimerCounterChannel::B:
      keep_mask = kKeepAllExceptChannelB;
      set_mask = static_cast<uint8_t>(mode) << COM3B0;
      break;
    case TimerCounterChannel::C:
      keep_mask = kKeepAllExceptChannelC;
      set_mask = static_cast<uint8_t>(mode) << COM3C0;
      break;
    default:
      TAS_DCHECK(false) << "Unknown channel " << channel;
  }
  noInterrupts();
  TCCR3A = (TCCR3A & keep_mask) | set_mask;
  interrupts();
}

void TimerCounter3SetOutputCompareRegister(TimerCounterChannel channel,
                                           uint16_t value) {
  switch (channel) {
    case TimerCounterChannel::A:
      OCR3A = value;
      break;
    case TimerCounterChannel::B:
      OCR3B = value;
      break;
    case TimerCounterChannel::C:
      OCR3C = value;
      break;
    default:
      TAS_DCHECK(false) << "Unknown channel " << channel;
  }
}

uint16_t TimerCounter3GetOutputCompareRegister(TimerCounterChannel channel) {
  switch (channel) {
    case TimerCounterChannel::A:
      return OCR3A;
    case TimerCounterChannel::B:
      return OCR3B;
    case TimerCounterChannel::C:
      return OCR3C;
    default:
      TAS_DCHECK(false) << "Unknown channel " << channel;
      return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

void TimerCounter4Initialize16BitFastPwm(ClockPrescaling prescaling) {
  noInterrupts();
  TCCR4A = 1 << WGM11;
  TCCR4B =
      (1 << WGM13) | (1 << WGM12) | static_cast<uint8_t>(prescaling) << CS10;
  ICR4 = UINT16_MAX;
  interrupts();
}

void TimerCounter4SetCompareOutputMode(TimerCounterChannel channel,
                                       FastPwmCompareOutputMode mode) {
  uint8_t keep_mask, set_mask;
  switch (channel) {
    case TimerCounterChannel::A:
      keep_mask = kKeepAllExceptChannelA;
      set_mask = static_cast<uint8_t>(mode) << COM4A0;
      break;
    case TimerCounterChannel::B:
      keep_mask = kKeepAllExceptChannelB;
      set_mask = static_cast<uint8_t>(mode) << COM4B0;
      break;
    case TimerCounterChannel::C:
      keep_mask = kKeepAllExceptChannelC;
      set_mask = static_cast<uint8_t>(mode) << COM4C0;
      break;
    default:
      TAS_DCHECK(false) << "Unknown channel " << channel;
  }
  noInterrupts();
  TCCR4A = (TCCR4A & keep_mask) | set_mask;
  interrupts();
}

void TimerCounter4SetOutputCompareRegister(TimerCounterChannel channel,
                                           uint16_t value) {
  switch (channel) {
    case TimerCounterChannel::A:
      OCR4A = value;
      break;
    case TimerCounterChannel::B:
      OCR4B = value;
      break;
    case TimerCounterChannel::C:
      OCR4C = value;
      break;
    default:
      TAS_DCHECK(false) << "Unknown channel " << channel;
  }
}

uint16_t TimerCounter4GetOutputCompareRegister(TimerCounterChannel channel) {
  switch (channel) {
    case TimerCounterChannel::A:
      return OCR4A;
    case TimerCounterChannel::B:
      return OCR4B;
    case TimerCounterChannel::C:
      return OCR4C;
    default:
      TAS_DCHECK(false) << "Unknown channel " << channel;
      return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

void TimerCounter5Initialize16BitFastPwm(ClockPrescaling prescaling) {
  noInterrupts();
  TCCR5A = 1 << WGM11;
  TCCR5B =
      (1 << WGM13) | (1 << WGM12) | static_cast<uint8_t>(prescaling) << CS10;
  ICR5 = UINT16_MAX;
  interrupts();
}

void TimerCounter5SetCompareOutputMode(TimerCounterChannel channel,
                                       FastPwmCompareOutputMode mode) {
  uint8_t keep_mask, set_mask;
  switch (channel) {
    case TimerCounterChannel::A:
      keep_mask = kKeepAllExceptChannelA;
      set_mask = static_cast<uint8_t>(mode) << COM5A0;
      break;
    case TimerCounterChannel::B:
      keep_mask = kKeepAllExceptChannelB;
      set_mask = static_cast<uint8_t>(mode) << COM5B0;
      break;
    case TimerCounterChannel::C:
      keep_mask = kKeepAllExceptChannelC;
      set_mask = static_cast<uint8_t>(mode) << COM5C0;
      break;
    default:
      TAS_DCHECK(false) << "Unknown channel " << channel;
  }
  noInterrupts();
  TCCR5A = (TCCR5A & keep_mask) | set_mask;
  interrupts();
}

void TimerCounter5SetOutputCompareRegister(TimerCounterChannel channel,
                                           uint16_t value) {
  switch (channel) {
    case TimerCounterChannel::A:
      OCR5A = value;
      break;
    case TimerCounterChannel::B:
      OCR5B = value;
      break;
    case TimerCounterChannel::C:
      OCR5C = value;
      break;
    default:
      TAS_DCHECK(false) << "Unknown channel " << channel;
  }
}

uint16_t TimerCounter5GetOutputCompareRegister(TimerCounterChannel channel) {
  switch (channel) {
    case TimerCounterChannel::A:
      return OCR5A;
    case TimerCounterChannel::B:
      return OCR5B;
    case TimerCounterChannel::C:
      return OCR5C;
    default:
      TAS_DCHECK(false) << "Unknown channel " << channel;
      return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

TimerCounter1Pwm16Output::TimerCounter1Pwm16Output(TimerCounterChannel channel,
                                                   uint8_t enabled_pin)
    : channel_(channel), enabled_pin_(enabled_pin) {}

bool TimerCounter1Pwm16Output::is_enabled() const {
  pinMode(enabled_pin_, INPUT_PULLUP);
  return digitalRead(enabled_pin_) == LOW;
}

void TimerCounter1Pwm16Output::set_pulse_count(uint16_t value) {
  TAS_DCHECK(is_enabled());
  if (value == 0) {
    TimerCounter1SetCompareOutputMode(channel_,
                                      FastPwmCompareOutputMode::kDisabled);
  } else {
    TimerCounter1SetCompareOutputMode(
        channel_, FastPwmCompareOutputMode::kNonInvertingMode);
    TimerCounter1SetOutputCompareRegister(channel_, value);
  }
}

uint16_t TimerCounter1Pwm16Output::get_pulse_count() const {
  TAS_DCHECK(is_enabled());
  return TimerCounter1GetOutputCompareRegister(channel_);
}

////////////////////////////////////////////////////////////////////////////////

TimerCounter5Pwm16Output::TimerCounter5Pwm16Output(TimerCounterChannel channel,
                                                   uint8_t enabled_pin)
    : channel_(channel), enabled_pin_(enabled_pin) {}

bool TimerCounter5Pwm16Output::is_enabled() const {
  pinMode(enabled_pin_, INPUT_PULLUP);
  return digitalRead(enabled_pin_) == LOW;
}

void TimerCounter5Pwm16Output::set_pulse_count(uint16_t value) {
  TAS_DCHECK(is_enabled());
  if (value == 0) {
    TimerCounter5SetCompareOutputMode(channel_,
                                      FastPwmCompareOutputMode::kDisabled);
  } else {
    TimerCounter5SetCompareOutputMode(
        channel_, FastPwmCompareOutputMode::kNonInvertingMode);
    TimerCounter5SetOutputCompareRegister(channel_, value);
  }
}

uint16_t TimerCounter5Pwm16Output::get_pulse_count() const {
  TAS_DCHECK(is_enabled());
  return TimerCounter5GetOutputCompareRegister(channel_);
}

}  // namespace alpaca
