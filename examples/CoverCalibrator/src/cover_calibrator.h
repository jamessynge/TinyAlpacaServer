#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

#include <Arduino.h>
#include <TinyAlpacaServer.h>

#include "src/cover.h"
#include "src/pwm_led.h"

// Include the AccelStepper library:
// #include <AccelStepper.h>

namespace astro_makers {

class CoverCalibrator : public alpaca::CoverCalibratorAdapter {
 public:
  CoverCalibrator();

  void Initialize() override;
  void Update() override;

  // Returns the current calibrator brightness.
  alpaca::StatusOr<int32_t> GetBrightness() override;

  // Returns the state of the calibration device, or kUnknown if not overridden
  // by a subclass.
  alpaca::StatusOr<alpaca::ECalibratorStatus> GetCalibratorState() override;

  // Returns the calibrator's maximum Brightness value.
  alpaca::StatusOr<int32_t> GetMaxBrightness() override;

  bool HandlePutCalibratorOff(const alpaca::AlpacaRequest& request,
                              Print& out) override;

  alpaca::Status SetBrightness(uint32_t brightness) override;

  // alpaca::StatusOr<ECoverStatus> GetCoverState() override;
  // bool HandlePutCloseCover(const alpaca::AlpacaRequest& request,
  //                          Print& out) override;
  // bool HandlePutHaltCover(const alpaca::AlpacaRequest& request,
  //                         Print& out) override;
  // bool HandlePutOpenCover(const alpaca::AlpacaRequest& request,
  //                         Print& out) override;

 private:
  void AdjustCurrentBrightness();

  // For fun, we implement a simple linear ramp from current brightness to the
  // target. We could use an easing (aka tweening) function if we wanted to be
  // even fancier.
  uint8_t current_brightness_{0};
  uint8_t brightness_target_{0};
  MillisT last_change_ms_{0};  // NOLINT

  PwmLed led1_;
  Cover cover_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_
