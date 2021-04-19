#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_H_

// Support for opening and closing the cover.

#include <Arduino.h>
#include <TinyAlpacaServer.h>

#ifdef ARDUINO
#include <AccelStepper.h>
#else
// Stub of AccelStepper for host compilation.
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
#endif

namespace astro_makers {

class Cover {
 public:
  Cover(uint8_t step_pin, uint8_t direction_pin, uint8_t open_limit_pin,
        uint8_t closed_limit_pin, uint8_t cover_present_pin);

  alpaca::ECoverStatus GetStatus() const;

  bool IsPresent() const;
  bool IsMoving() const { return moving_; }
  bool IsOpen() const;
  bool IsClosed() const;

  void Open();
  void Close();
  void Halt();

  void PerformIO();

 private:
  // This is isolated into a separate routine so that we can call it from an
  // ISR.
  void MoveStepper();

  AccelStepper stepper_;

  // If LOW, the Open Limit switch has been closed (grounded).
  const uint8_t open_limit_pin_;

  // If LOW, the Closed Limit switch has been closed (grounded).
  const uint8_t closed_limit_pin_;

  // If LOW, the jumper is installed indicating that a cover motor is available.
  const uint8_t cover_present_pin_;

  //////////////////////////////////////////////////////////////////////////////
  // These two fields are specified as volatile to *allow* for an ISR to access
  // and update it.

  // moving_ is to true when a command is recieved to open or close, and is set
  // to false when that operation is achieved or when commanded to halt.
  volatile bool moving_{false};

  // Direction of movement.
  volatile bool closing_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_H_
