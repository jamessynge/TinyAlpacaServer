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
  CoverCalibrator(const alpaca::DeviceInfo& device_info);

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

  // Extension API for the AstroMakers Cover Calibrator, which has 4 channels.

  // Sets the enabled value for the specified channel (0 through 3), and returns
  // the value that has been computed based on the jumper pin for that channel
  // AND the enabled argument. Returns false if the channel value is invalid.
  bool SetLedChannelEnabled(int channel, bool enabled);

  // Gets the enabled value for the specified channel (0 through 3) that has
  // been computed based on the jumper pin for that channel AND the last value
  // from SetLedChannelEnabled. Returns false if the channel value is invalid.
  bool GetLedChannelEnabled(int channel) const;

  // Gets the enabled value for the specified channel (0 through 3) based solely
  // on the jumper pin for that channel. Returns false if the channel value is
  // invalid.
  bool GetLedChannelHardwareEnabled(int channel) const;

 private:
  bool IsCalibratorHardwareEnabled() const;

  // TODO(jamessynge): Need something like template specialization to select the
  // timer/counter number and channel given the kLedChannel1PwmPin macro (and
  // other such macros). Doing so could avoid linking in unused objects.
  alpaca::TimerCounter3Pwm16Output led1_;
  alpaca::TimerCounter3Pwm16Output led2_;
  alpaca::TimerCounter3Pwm16Output led3_;
  alpaca::TimerCounter4Pwm16Output led4_;

  Cover cover_;

  uint16_t brightness_;
  uint8_t enabled_led_channels_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_
