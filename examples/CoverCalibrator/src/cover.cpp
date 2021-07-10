#include "cover.h"

#include <Arduino.h>

#include "constants.h"

namespace {

// The cover instance that is moving. Note that while multiple Cover instances
// can be created, only a single Timer/Counter is used on the ATmega2560 (number
// 5), therefore only a single Cover instance can be moving a cover at the same
// time. While it is unlikely that a single AstroMakers server will be serving
// multiple covers, this field allows for such a server.
astro_makers::InterruptHandler* interrupt_handler = nullptr;

// Get the value of interrupt_handler.
astro_makers::InterruptHandler* GetInterruptHandler() {
  astro_makers::InterruptHandler* copy;
  noInterrupts();
  copy = interrupt_handler;
  interrupts();
  return copy;
}

// Clear interrupt_handler if it is set to old_handler.
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
  TAS_VLOG(4) << TAS_FLASHSTR("StartTimer5 ct=") << ct
              << TAS_FLASHSTR(", ct.ClockSelectBits=") << ct.ClockSelectBits();

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

  TAS_VLOG(4) << alpaca::BaseHex << TAS_FLASHSTR("a=") << a
              << TAS_FLASHSTR(", b=") << b << alpaca::BaseDec
              << TAS_FLASHSTR(", top=") << top;

  noInterrupts();
  OCR5A = top;
  TCNT5 = 0;
  TCCR5A = a;
  TCCR5B = b;
  bitWrite(TIMSK5, TOIE5, 1);
  bitWrite(TIFR5, TOV5, 1);
  interrupts();

  if (TAS_VLOG_IS_ON(4)) {
    TAS_VLOG(4) << alpaca::BaseHex << TAS_FLASHSTR("StartTimer5 TCCR5A: ")
                << TCCR5A << TAS_FLASHSTR(", TCCR5B: ") << TCCR5B;
    TAS_VLOG(4) << TAS_FLASHSTR("StartTimer5 TCNT5: ") << TCNT5
                << TAS_FLASHSTR(", OCR5A: ") << OCR5A
                << TAS_FLASHSTR(", OCR5B: ") << OCR5B
                << TAS_FLASHSTR(", OCR5C: ") << OCR5C;

    // If something goes wrong right away, the interrupt handler will disable
    // the counter, so log the registers again after a millisecond.

    delay(1);
    TAS_VLOG(4) << alpaca::BaseHex << TAS_FLASHSTR("StartTimer5 TCCR5A: ")
                << TCCR5A << TAS_FLASHSTR(", TCCR5B: ") << TCCR5B;
    TAS_VLOG(4) << TAS_FLASHSTR("StartTimer5 TCNT5: ") << TCNT5
                << TAS_FLASHSTR(", OCR5A: ") << OCR5A
                << TAS_FLASHSTR(", OCR5B: ") << OCR5B
                << TAS_FLASHSTR(", OCR5C: ") << OCR5C;
  }
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
    : alpaca::EnableableByPin(cover_present_pin),
      step_pin_(step_pin),
      direction_pin_(direction_pin),
      open_limit_pin_(open_limit_pin),
      closed_limit_pin_(closed_limit_pin),
      allowed_steps_(allowed_steps),
      allowed_start_steps_(allowed_start_steps),
      motor_status_(kNotMoving) {}

Cover::Cover()
    : Cover(kCoverMotorStepPin, kCoverMotorDirectionPin, kCoverOpenLimitPin,
            kCoverCloseLimitPin, kCoverEnabledPin, kMaximumSteps,
            kMaximumStartSteps) {}

void Cover::Initialize() {
  pinMode(step_pin_, OUTPUT);
  digitalWrite(step_pin_, LOW);

  pinMode(direction_pin_, OUTPUT);
  digitalWrite(direction_pin_, LOW);  // Initial value doesn't really matter.

  pinMode(open_limit_pin_, kLimitSwitchPinMode);
  pinMode(closed_limit_pin_, kLimitSwitchPinMode);

  ResetTimer5();
}

ECoverStatus Cover::GetCoverStatus() const {
  if (!IsEnabled()) {
    return ECoverStatus::kNotPresent;
  } else if (IsMoving()) {
    return ECoverStatus::kMoving;
  } else if (IsClosed()) {
    TAS_DCHECK(!IsOpen());
    return ECoverStatus::kClosed;
  } else if (IsOpen()) {
    return ECoverStatus::kOpen;
  } else {
    TAS_VLOG(1) << TAS_FLASHSTR("GetCoverStatus -> Unknown; motor_status=")
                << motor_status_ << TAS_FLASHSTR(", step_count=")
                << step_count_;
    return ECoverStatus::kUnknown;
  }
}

bool Cover::IsMoving() const {
  return motor_status_ == kOpening || motor_status_ == kClosing;
}

bool Cover::IsOpen() const {
  return digitalRead(open_limit_pin_) == kLimitSwitchClosed && IsEnabled();
}

bool Cover::IsClosed() const {
  return digitalRead(closed_limit_pin_) == kLimitSwitchClosed && IsEnabled();
}

bool Cover::CanMove() const {
  if (IsEnabled()) {
    InterruptHandler* handler = GetInterruptHandler();
    return (handler == nullptr || handler == this);
  }
  return false;
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
  } else {
    TAS_DCHECK_EQ(handler, nullptr);  // CanMove should prevent this failing.
  }
  motor_status_ = kOpening;
  StartMoving(kDirectionOpen);
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
  } else {
    TAS_DCHECK_EQ(handler, nullptr);
  }
  motor_status_ = kClosing;
  StartMoving(kDirectionClose);
  return true;
}

void Cover::StartMoving(int direction_pin_value) {
  ResetTimer5();
  step_count_ = 0;
  digitalWrite(direction_pin_, direction_pin_value);
  interrupt_handler = this;
  StartTimer5(kStepsPerSecond);
  delay(1);

  if (TAS_VLOG_IS_ON(1)) {
    noInterrupts();
    uint32_t step_count_copy = step_count_;
    interrupts();
    TAS_VLOG(3) << TAS_FLASHSTR("StartMoving done, handler=")
                << GetInterruptHandler() << TAS_FLASHSTR(", motor_status_=")
                << motor_status_ << TAS_FLASHSTR(", step_count=")
                << step_count_copy;
  }
}

void Cover::Halt() {
  // Don't change the value of motor_status_ unless it appears we were moving.
  if (IsMoving()) {
    motor_status_ = kNotMoving;
  }
  RemoveInterruptHandler(this);
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
  // stays closed for at least one ms or three interrupts).
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
