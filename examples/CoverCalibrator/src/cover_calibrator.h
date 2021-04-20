#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_

// Implements the Alpaca Cover Calibrator driver for the AstroMakers Cover
// Calibrator.
//
// Author: james.synge@gmail.com

#include <Arduino.h>
#include <TinyAlpacaServer.h>

#include "src/cover.h"
#include "src/ramp_pwm_led.h"

// Include the AccelStepper library:
// #include <AccelStepper.h>

namespace astro_makers {

class CoverCalibrator : public alpaca::CoverCalibratorAdapter {
 public:
  CoverCalibrator();

  void Initialize() override;
  void MaintainDevice() override;

  // Returns the current calibrator brightness.
  alpaca::StatusOr<int32_t> GetBrightness() override;

  // Returns the state of the calibration device, or kUnknown if not overridden
  // by a subclass.
  alpaca::StatusOr<alpaca::ECalibratorStatus> GetCalibratorState() override;

  // Returns the calibrator's maximum Brightness value.
  alpaca::StatusOr<int32_t> GetMaxBrightness() override;

  // Turns the calibrator lights off immediately.
  alpaca::Status SetCalibratorOff() override;

  // Sets the calibrator brightness.
  alpaca::Status SetCalibratorBrightness(uint32_t brightness) override;

  alpaca::StatusOr<alpaca::ECoverStatus> GetCoverState() override;

  alpaca::Status MoveCover(bool open) override;
  alpaca::Status HaltCoverMotion() override;

 private:
  RampPwmLed white_led_;
  Cover cover_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_
