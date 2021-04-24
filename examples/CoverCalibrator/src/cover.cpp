#include "cover.h"

// TODO(jamessynge): Decide whether to use an ISR for updating the position.
// We certainly don't want to block the CPU for the duration of opening or
// closing the cover. The alternative used now is to call PerformIO frequently,
// i.e. from loop(), which requires that other methods called from loop are
// non-blocking, and don't run for very long in total.

#include <Arduino.h>

#include "device_types/cover_calibrator/cover_calibrator_constants.h"

namespace astro_makers {

#define kCoverPresentPinMode INPUT_PULLUP
#define kCoverIsPresent LOW

#define kLimitSwitchPinMode INPUT_PULLUP
#define kLimitSwitchClosed LOW

// WARNING: The AccelStepper library suggests that 1K steps/second is a
// reasonable max speed for most systems. Maximum speed is set to 20K
// steps/second, and speed to 10K, based on Alan's choices in his demo script.
// TBD whether to lower them.
#define kMaxStepperSpeed 20000.0
#define kStepperSpeed 10000.0

using ::alpaca::ECoverStatus;

Cover::Cover(uint8_t step_pin, uint8_t direction_pin, uint8_t open_limit_pin,
             uint8_t closed_limit_pin, uint8_t cover_present_pin)
    : stepper_(AccelStepper::DRIVER, step_pin, direction_pin),
      open_limit_pin_(open_limit_pin),
      closed_limit_pin_(closed_limit_pin),
      cover_present_pin_(cover_present_pin) {
  pinMode(open_limit_pin, kLimitSwitchPinMode);
  pinMode(closed_limit_pin, kLimitSwitchPinMode);
  pinMode(cover_present_pin, kCoverPresentPinMode);
}

ECoverStatus Cover::GetStatus() const {
  if (digitalRead(cover_present_pin_) == LOW) {
    return ECoverStatus::kNotPresent;
  } else if (moving_) {
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
  return digitalRead(cover_present_pin_) == kCoverIsPresent;
}

bool Cover::IsOpen() const {
  return digitalRead(open_limit_pin_) == kLimitSwitchClosed && IsPresent();
}

bool Cover::IsClosed() const {
  return digitalRead(closed_limit_pin_) == kLimitSwitchClosed && IsPresent();
}

bool Cover::Open() {
  if (!IsPresent() || IsOpen()) {
    return false;
  } else if (!moving_ || closing_) {
    closing_ = false;
    moving_ = true;
    stepper_.setMaxSpeed(kMaxStepperSpeed);
    stepper_.setSpeed(kStepperSpeed);
    MoveStepper();
  }
  return true;
}

bool Cover::Close() {
  if (!IsPresent() || IsClosed()) {
    return false;
  } else if (!moving_ || !closing_) {
    closing_ = true;
    moving_ = true;
    stepper_.setMaxSpeed(kMaxStepperSpeed);
    stepper_.setSpeed(-kStepperSpeed);
    MoveStepper();
  }
  return true;
}

void Cover::Halt() { moving_ = false; }

bool Cover::MoveStepper() {
  if (moving_) {
    if ((closing_ && IsClosed()) || (!closing_ && IsOpen())) {
      moving_ = false;
      return false;
    }
    return stepper_.runSpeed();
  }
  return false;
}

}  // namespace astro_makers
