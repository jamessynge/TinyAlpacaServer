#include "cover.h"

#include <Arduino.h>

#include "constants.h"

namespace {

// The cover instance that is moving. Note that while multiple Cover instances
// can be created, only a single Timer/Counter is used on the ATmega2560 (number
// 5), therefore only a single Cover instance can be moving a cover at the same
// time. While it is unlikely that a single AstroMakers server will be serving
// multiple covers, this field allows for such a device.
astro_makers::InterruptHandler* interrupt_handler = nullptr;

// Get the value of interrupt_handler. This does not need to be used by code
// where it is known that interrupts are disabled, which is the situation when
// an ISR is called.
astro_makers::InterruptHandler* GetInterruptHandler() {
  astro_makers::InterruptHandler* copy;
  noInterrupts();
  copy = interrupt_handler;
  interrupts();
  return copy;
}

// Clear interrupt_handler if it is set to old_handler. This does not need to be
// used by code where it is known that interrupts are disabled, which is the
// situation when an ISR is called.
void RemoveInterruptHandler(astro_makers::InterruptHandler* old_handler) {
  noInterrupts();
  if (interrupt_handler == old_handler) {
    interrupt_handler = nullptr;
  }
  interrupts();
}

void ResetTimer5() {
  // Disable interrupts first.
  TIMSK5 = 0;

  // Clear any interrupts that have occurred; only relevant if this method is
  // called with interrupts disabled because if interrupts are enabled then
  // clearing TIMSK5 will disable any interrupts, so ICR5 would be zero at this
  // point.
  TIFR5 = 0;

  // Clear the registers that configure the behavior of the timer/counter.
  TCCR5A = 0;
  TCCR5B = 0;
  TCCR5C = 0;

  // Clear the 16-bit registers with the counter, match and capture values.
  TCNT5 = 0;
  OCR5A = 0;
  OCR5B = 0;
  OCR5C = 0;
  ICR5 = 0;
}

// ct represents 1/2 of the period between Timer/Counter 5 overflow interrupts.
void StartTimer5(const alpaca::TC16ClockAndTicks& ct) {
  // We use Waveform Generation Mode 9, i.e. Phase and Frequency Correct PWM
  // Mode; in this mode TCNT5 is incremented by one at each clock tick until it
  // reaches TOP (OCR5A) , then decremented down to BOTTOM (0), at which point
  // it starts incrementing up again. This dual slope operation means that the
  // period is 2 * TOP, i.e. 2 * OCR5A. Note that the counter is at TOP for only
  // one clock cycle (I don't know if that also means that the counter is at
  // BOTTOM for only one clock cycle, but that seems likely).
  uint8_t a = 1 << WGM50;
  uint8_t b = (1 << WGM53) | ct.ClockSelectBits();
  uint16_t top = ct.clock_ticks;

  noInterrupts();
  OCR5A = top;
  TCCR5B = b;
  TCCR5A = a;
  TCNT5 = 0;
  bitWrite(TIMSK5, TOIE5, 1);
  bitWrite(TIFR5, TOV5, 1);
  interrupts();
}

void StartTimer5(uint16_t interrupts_per_second) {
  StartTimer5(alpaca::TC16ClockAndTicks::FromIntegerEventsPerSecond(
      interrupts_per_second));
}

}  // namespace

ISR(TIMER5_OVF_vect) {
  if (interrupt_handler != nullptr) {
    interrupt_handler->HandleInterrupt();
  } else {
    ResetTimer5();
  }
}

namespace astro_makers {
using ::alpaca::ECoverStatus;

Cover::Cover(uint8_t step_pin, uint8_t direction_pin, uint8_t open_limit_pin,
             uint8_t closed_limit_pin, uint8_t cover_present_pin,
             uint32_t allowed_steps, uint32_t allowed_start_steps)
    : step_pin_(step_pin),
      direction_pin_(direction_pin),
      open_limit_pin_(open_limit_pin),
      closed_limit_pin_(closed_limit_pin),
      cover_present_pin_(cover_present_pin),
      allowed_steps_(allowed_steps),
      allowed_start_steps_(allowed_start_steps),
      motor_status_(kNotMoving) {
  pinMode(open_limit_pin, kLimitSwitchPinMode);
  pinMode(closed_limit_pin, kLimitSwitchPinMode);
  pinMode(cover_present_pin, kCoverPresentPinMode);
}

Cover::Cover()
    : Cover(kCoverMotorStepPin, kCoverMotorDirectionPin, kCoverOpenLimitPin,
            kCoverCloseLimitPin, kCoverEnabledPin, kMaximumSteps,
            kMaximumStartSteps) {}

ECoverStatus Cover::GetCoverStatus() const {
  if (digitalRead(cover_present_pin_) == LOW) {
    return ECoverStatus::kNotPresent;
  } else if (IsMoving()) {
    return ECoverStatus::kMoving;
  } else if (IsClosed()) {
    TAS_DCHECK(!IsOpen());
    return ECoverStatus::kClosed;
  } else if (IsOpen()) {
    return ECoverStatus::kOpen;
  } else {
    return ECoverStatus::kUnknown;
  }
}

bool Cover::IsPresent() const {
  return cover_present_pin_ == 0 ||
         digitalRead(cover_present_pin_) == kCoverIsPresent;
}

bool Cover::IsMoving() const {
  return motor_status_ == kOpening || motor_status_ == kClosing;
}

bool Cover::IsOpen() const {
  return digitalRead(open_limit_pin_) == kLimitSwitchClosed && IsPresent();
}

bool Cover::IsClosed() const {
  return digitalRead(closed_limit_pin_) == kLimitSwitchClosed && IsPresent();
}

bool Cover::CanMove() const {
  InterruptHandler* handler = GetInterruptHandler();
  return IsPresent() && (handler == nullptr || handler == this);
}

bool Cover::Open() {
  if (!CanMove()) {
    return false;
  }
  if (IsOpen()) {
    return false;
  }
  InterruptHandler* handler = GetInterruptHandler();
  if (handler == this) {
    // Are we moving in the requested direction?
    if (motor_status_ == kOpening) {
      // Already moving.
      return true;
    }
    ResetTimer5();
  } else {
    TAS_DCHECK_EQ(handler, nullptr);
  }
  digitalWrite(direction_pin_, kDirectionOpen);
  handler = this;
  StartTimer5(kStepsPerSecond);
  return true;
}

bool Cover::Close() {
  if (!CanMove()) {
    return false;
  }
  if (IsClosed()) {
    return false;
  }
  InterruptHandler* handler = GetInterruptHandler();
  if (handler == this) {
    // Are we moving in the requested direction?
    if (motor_status_ == kClosing) {
      // Already moving.
      return true;
    }
    ResetTimer5();
  } else {
    TAS_DCHECK_EQ(handler, nullptr);
  }
  digitalWrite(direction_pin_, kDirectionClose);
  handler = this;
  StartTimer5(kStepsPerSecond);
  return true;
}

void Cover::Halt() {
  if (IsMoving()) {
    motor_status_ = kNotMoving;
    RemoveInterruptHandler(this);
  }
}

void Cover::HandleInterrupt() {
  uint8_t start_pin;
  uint8_t limit_pin;
  if (motor_status_ == kClosing) {
    start_pin = open_limit_pin_;
    limit_pin = closed_limit_pin_;
  } else if (motor_status_ == kOpening) {
    start_pin = closed_limit_pin_;
    limit_pin = open_limit_pin_;
  } else {
    interrupt_handler = nullptr;
    return;
  }

  // On the assumption that our steps are small and rapid, we check the limit
  // switches after stepping. Note that writing or reading by direct access to
  // the port is much faster, should performance become an issue.

  digitalWrite(step_pin_, HIGH);
  delayMicroseconds(1);
  digitalWrite(step_pin_, LOW);
  ++step_count_;

  // Note that we don't debounce the limit pin (e.g. we don't check that it
  // stays closed for at least 1ms).
  if (digitalRead(limit_pin) == kLimitSwitchClosed) {
    // Reached the end.
    motor_status_ = kNotMoving;
    interrupt_handler = nullptr;
    return;
  }

  // We only check once for this situation.
  if (step_count_ == allowed_start_steps_ &&
      digitalRead(start_pin) == kLimitSwitchClosed) {
    // The other limit switch is still closed, so we must have failed to move
    // far enough.
    if (motor_status_ == kClosing) {
      motor_status_ = kStartClosingFailed;
    } else {
      motor_status_ = kStartOpeningFailed;
    }
    interrupt_handler = nullptr;
    return;
  }

  if (step_count_ > allowed_steps_) {
    if (motor_status_ == kClosing) {
      motor_status_ = kClosingFailed;
    } else {
      motor_status_ = kOpeningFailed;
    }
    interrupt_handler = nullptr;
    return;
  }
}

}  // namespace astro_makers
