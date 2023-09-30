#include "cover.h"

#include <Arduino.h>
#include <McuCore.h>

#include "constants.h"

namespace {
using ::mcucore::TC16ClockAndTicks;

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
void StartTimer5(const TC16ClockAndTicks& ct) {
  MCU_VLOG(4) << MCU_PSD("StartTimer5 ct=") << ct
              << MCU_PSD(", ct.ClockSelectBits=") << ct.ClockSelectBits();

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

  MCU_VLOG(4) << mcucore::BaseHex << MCU_PSD("a=") << a << MCU_PSD(", b=") << b
              << mcucore::BaseDec << MCU_PSD(", top=") << top;

  noInterrupts();
  OCR5A = top;
  TCNT5 = 0;
  TCCR5A = a;
  TCCR5B = b;
  bitWrite(TIMSK5, TOIE5, 1);
  bitWrite(TIFR5, TOV5, 1);
  interrupts();

  if (MCU_VLOG_IS_ON(4)) {
    MCU_VLOG(4) << mcucore::BaseHex << MCU_PSD("StartTimer5 TCCR5A: ") << TCCR5A
                << MCU_PSD(", TCCR5B: ") << TCCR5B;
    MCU_VLOG(4) << MCU_PSD("StartTimer5 TCNT5: ") << TCNT5
                << MCU_PSD(", OCR5A: ") << OCR5A << MCU_PSD(", OCR5B: ")
                << OCR5B << MCU_PSD(", OCR5C: ") << OCR5C;

    // If something goes wrong right away, the interrupt handler will disable
    // the counter, so log the registers again after a millisecond.

    delay(1);
    MCU_VLOG(4) << mcucore::BaseHex << MCU_PSD("StartTimer5 TCCR5A: ") << TCCR5A
                << MCU_PSD(", TCCR5B: ") << TCCR5B;
    MCU_VLOG(4) << MCU_PSD("StartTimer5 TCNT5: ") << TCNT5
                << MCU_PSD(", OCR5A: ") << OCR5A << MCU_PSD(", OCR5B: ")
                << OCR5B << MCU_PSD(", OCR5C: ") << OCR5C;
  }
}

void StartTimer5(uint16_t interrupts_per_second) {
  StartTimer5(
      TC16ClockAndTicks::FromIntegerEventsPerSecond(interrupts_per_second));
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

Cover::Cover(uint8_t cover_present_pin, uint8_t stepper_enable_pin,
             uint8_t step_pin, uint8_t direction_pin, uint8_t open_limit_pin,
             uint8_t closed_limit_pin, uint32_t allowed_steps,
             uint32_t allowed_start_steps)
    : mcucore::EnableableByPin(cover_present_pin),
      stepper_enable_pin_(stepper_enable_pin),
      step_pin_(step_pin),
      direction_pin_(direction_pin),
      open_limit_pin_(open_limit_pin),
      closed_limit_pin_(closed_limit_pin),
      allowed_steps_(allowed_steps),
      allowed_start_steps_(allowed_start_steps),
      motor_status_(kNotMoving) {}

Cover::Cover()
    : Cover(kCoverPresentPin, kCoverMotorEnablePin, kCoverMotorStepPin,
            kCoverMotorDirectionPin, kCoverOpenLimitPin, kCoverCloseLimitPin,
            kMaximumSteps, kMaximumStartSteps) {}

void Cover::ResetHardware() {
  if (IsEnabled()) {
    pinMode(stepper_enable_pin_, OUTPUT);
    DisableStepper();
  }
}

void Cover::InitializeHardware() {
  // TODO(jamessynge): Consider whether to do this if motor is not present.

  ResetTimer5();

  pinMode(stepper_enable_pin_, OUTPUT);
  DisableStepper();

  pinMode(step_pin_, OUTPUT);
  digitalWrite(step_pin_, LOW);

  pinMode(direction_pin_, OUTPUT);
  digitalWrite(direction_pin_, LOW);  // Initial value doesn't really matter.

  pinMode(open_limit_pin_, kLimitSwitchPinMode);
  pinMode(closed_limit_pin_, kLimitSwitchPinMode);

#if defined(kMicrostepResolution1)
  pinMode(kMicrostepResolution1, OUTPUT);
  pinMode(kMicrostepResolution2, OUTPUT);
  pinMode(kMicrostepResolution3, OUTPUT);
  digitalWrite(kMicrostepResolution1, HIGH);
  digitalWrite(kMicrostepResolution2, HIGH);
  digitalWrite(kMicrostepResolution3, LOW);
#endif
}

ECoverStatus Cover::GetCoverStatus() const {
  if (!IsEnabled()) {
    return ECoverStatus::kNotPresent;
  } else if (IsMoving()) {
    return ECoverStatus::kMoving;
  }
  const bool is_open = IsOpen();
  const bool is_closed = IsClosed();
  if (is_open != is_closed) {
    if (is_open) {
      return ECoverStatus::kOpen;
    } else {
      return ECoverStatus::kClosed;
    }
  }
  if (is_open && is_closed) {
    MCU_VLOG(1) << MCU_PSD("GetCoverStatus -> ")
                << MCU_PSD("Error: both open and closed")
                << MCU_PSD("; motor_status=") << motor_status_
                << MCU_PSD(", step_count=") << step_count_;
    return ECoverStatus::kError;
  }
  MCU_VLOG(1) << MCU_PSD("GetCoverStatus -> ") << MCU_PSD("Unknown")
              << MCU_PSD("; motor_status=") << motor_status_
              << MCU_PSD(", step_count=") << step_count_;
  return ECoverStatus::kUnknown;
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
  } else if (handler != nullptr) {
    MCU_VLOG(3) << MCU_PSD("Open") << MCU_PSD(": another handler installed: ")
                << handler;
    return false;
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
  } else if (handler != nullptr) {
    MCU_VLOG(3) << MCU_PSD("Close") << MCU_PSD(": another handler installed: ")
                << handler;
    return false;
  }
  motor_status_ = kClosing;
  StartMoving(kDirectionClose);
  return true;
}

void Cover::EnableStepper() { digitalWrite(stepper_enable_pin_, LOW); }

void Cover::DisableStepper() { digitalWrite(stepper_enable_pin_, HIGH); }

void Cover::StartMoving(int direction_pin_value) {
  EnableStepper();
  ResetTimer5();
  step_count_ = 0;
  digitalWrite(direction_pin_, direction_pin_value);
  interrupt_handler = this;
  StartTimer5(kStepsPerSecond);
  delay(1);

  if (MCU_VLOG_IS_ON(2)) {
    noInterrupts();
    uint32_t step_count_copy = step_count_;
    interrupts();
    MCU_VLOG(3) << MCU_PSD("StartMoving done, handler=")
                << GetInterruptHandler() << MCU_PSD(", motor_status_=")
                << motor_status_ << MCU_PSD(", step_count=") << step_count_copy;
  }
}

void Cover::Halt() {
  DisableStepper();
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
    DisableStepper();
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
    DisableStepper();
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
    DisableStepper();
    return;
  }

  if (step_count_ > allowed_steps_) {
    if (motor_status_ == kClosing) {
      motor_status_ = kClosingFailed;
    } else {
      motor_status_ = kOpeningFailed;
    }
    interrupt_handler = nullptr;
    DisableStepper();
    return;
  }
}

}  // namespace astro_makers
