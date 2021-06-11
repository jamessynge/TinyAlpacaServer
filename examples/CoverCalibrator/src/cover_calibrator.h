#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_

// Implements the Alpaca Cover Calibrator driver for the AstroMakers Cover
// Calibrator.
//
// Author: james.synge@gmail.com

#include <Arduino.h>
#include <TinyAlpacaServer.h>

#include "cover.h"

namespace astro_makers {

class CoverCalibrator : public alpaca::CoverCalibratorAdapter {
 public:
  CoverCalibrator();

  void Initialize() override;

  // Don't need to override MaintainDevice as we're using timer/counter
  // interrupts and PWM output.
  //  void MaintainDevice() override;

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
  // TODO(jamessynge): Need something like template specialization to select the
  // timer/counter number and channel given the kLedChannel1PwmPin macro (and
  // other such macros). Doing so could avoid linking in unused objects.
  alpaca::TimerCounter3Pwm16Output led1_;
  alpaca::TimerCounter3Pwm16Output led2_;
  alpaca::TimerCounter3Pwm16Output led3_;
  alpaca::TimerCounter4Pwm16Output led4_;
  Cover cover_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_
