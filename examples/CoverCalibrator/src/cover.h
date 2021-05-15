#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_H_

// Support for opening and closing the cover using the stepper motor and
// timer/counter interrupts.
//
// Author: james.synge@gmail.com

#include <AccelStepper.h>
#include <Arduino.h>
#include <TinyAlpacaServer.h>

namespace astro_makers {

// TODO(jamessynge): Move InterruptHandler into a utility library.
class InterruptHandler {
 public:
  virtual ~InterruptHandler() {}

  // Called to handle an interrupt.
  virtual void HandleInterrupt() = 0;
};

class Cover : InterruptHandler {
 public:
  enum MotorStatus : uint8_t {
    kNotMoving,
    kOpening,
    kClosing,
    kStartOpeningFailed,
    kStartClosingFailed,
    kOpeningFailed,
    kClosingFailed,
  };

  Cover(uint8_t step_pin, uint8_t direction_pin, uint8_t open_limit_pin,
        uint8_t closed_limit_pin, uint8_t cover_present_pin,
        uint32_t allowed_steps, uint32_t allowed_start_steps);

  Cover();

  alpaca::ECoverStatus GetCoverStatus() const;

  MotorStatus GetMotorStatus() const { return motor_status_; }

  // Returns true IFF the cover present pin indicates that the cover motor
  // feature is installed/usable.
  bool IsPresent() const;

  // Returns true IFF the cover has been successfully commanded to move, and has
  // not completed the movement.
  bool IsMoving() const;

  // Returns true IFF the open limit switch is closed and "is present".
  bool IsOpen() const;

  // Returns true IFF the closed limit switch is closed and "is present".
  bool IsClosed() const;

  // Move the motor until it the cover is open. Returns true IFF the cover was
  // not open and the cover "is present".
  bool Open();

  // Move the motor until it the cover is closed. Returns true IFF the cover was
  // not closed and the cover "is present".
  bool Close();

  // Stop any movement.
  void Halt();

  // Returns true if it should be possible to move the cover.
  bool CanMove() const;

 private:
  void HandleInterrupt() override;

  // bool StartMoving(MotorStatus motor_status,

  // Pulse to step the motor.
  const uint8_t step_pin_;

  // HIGH means close, LOW means the open, though that could be a separate
  // parameter.
  const uint8_t direction_pin_;

  // If LOW, the Open Limit switch has been closed (grounded).
  const uint8_t open_limit_pin_;

  // If LOW, the Closed Limit switch has been closed (grounded).
  const uint8_t closed_limit_pin_;

  // If LOW, the jumper is installed indicating that a cover motor is available.
  const uint8_t cover_present_pin_;

  // Maximum number of steps we can take during a single movement. This helps to
  // prevent burning out the motor if something goes wrong.
  const uint32_t allowed_steps_;

  // When starting to move, at this number of steps we check to make sure that
  // the limit switch that we're moving away from is not closed. This helps to
  // detect when the torque isn't sufficient to start the movement.
  const uint32_t allowed_start_steps_;

  //////////////////////////////////////////////////////////////////////////////
  // These two fields are specified as volatile to *allow* for an ISR to access
  // and update it.

  volatile MotorStatus motor_status_;

  // Number of steps taken during the current or most recent movement.
  volatile uint32_t step_count_ = 0;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_H_
