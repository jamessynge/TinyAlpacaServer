#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ACCEL_STEPPER_ACCEL_STEPPER_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ACCEL_STEPPER_ACCEL_STEPPER_H_

// Stub of AccelStepper for host compilation.

#include "extras/host/arduino/arduino.h"

class AccelStepper {
 public:
  // Stepper Driver, 2 driver pins required.
  static constexpr uint8_t DRIVER = 1;  // NOLINT

  AccelStepper(uint8_t interface, uint8_t pin1, uint8_t pin2, uint8_t pin3 = 4,
               uint8_t pin4 = 5, bool enable = true) {}

  long currentPosition() { return position_; }  // NOLINT
  void setMaxSpeed(float speed) {}
  void setSpeed(float speed) {
    if (speed < 0) {
      speed_ = -1;
    } else if (speed > 0) {
      speed_ = 1;
    } else {
      speed_ = 0;
    }
  }
  bool runSpeed() {
    position_ += speed_;
    return true;
  }

 private:
  long position_{1};  // NOLINT
  int8_t speed_;
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ACCEL_STEPPER_ACCEL_STEPPER_H_
