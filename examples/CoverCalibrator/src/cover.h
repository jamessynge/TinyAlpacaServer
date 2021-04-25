#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_H_

// Support for opening and closing the cover.
//
// Author: james.synge@gmail.com

#include <AccelStepper.h>
#include <Arduino.h>
#include <TinyAlpacaServer.h>

namespace astro_makers {

class Cover {
 public:
  Cover(uint8_t step_pin, uint8_t direction_pin, uint8_t open_limit_pin,
        uint8_t closed_limit_pin, uint8_t cover_present_pin);

  alpaca::ECoverStatus GetStatus() const;

  // Returns true IFF the cover present pin indicates that the cover motor
  // feature is installed/usable.
  bool IsPresent() const;

  // Returns true IFF the cover has been successfully commanded to move, and has
  // not completed the movement.
  bool IsMoving() const { return moving_; }

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

  // Checks if the stepper needs to move at the current time. If yes, then it
  // moves it and returns true. If not, returns false. This is isolated into a
  // separate routine so that we can call it from an ISR, or (indirectly) from
  // loop().
  bool MoveStepper();

 private:
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
