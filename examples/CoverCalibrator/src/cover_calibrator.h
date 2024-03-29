#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_

// Implements the Alpaca Cover Calibrator driver for the AstroMakers Cover
// Calibrator.
//
// Author: james.synge@gmail.com

#include <Arduino.h>
#include <McuCore.h>
#include <TinyAlpacaServer.h>

#include "cover.h"

namespace astro_makers {

class CoverCalibrator : public alpaca::CoverCalibratorAdapter {
 public:
  explicit CoverCalibrator(alpaca::ServerContext& server_context,
                           const alpaca::DeviceDescription& device_description);

  // Disables the stepper motor driver, nothing else.
  void ResetHardware() override;

  // Initialize all the pins.
  void InitializeDevice() override;

  // Returns the current calibrator brightness.
  mcucore::StatusOr<int32_t> GetBrightness() override;

  // Returns the state of the calibration device, or kUnknown if not overridden
  // by a subclass.
  mcucore::StatusOr<alpaca::ECalibratorStatus> GetCalibratorState() override;

  // Returns the calibrator's maximum Brightness value.
  mcucore::StatusOr<int32_t> GetMaxBrightness() override;

  // Turns the calibrator lights off immediately.
  mcucore::Status SetCalibratorOff() override;

  // Sets the calibrator brightness.
  mcucore::Status SetCalibratorBrightness(uint32_t brightness) override;

  mcucore::StatusOr<alpaca::ECoverStatus> GetCoverState() override;

  mcucore::Status MoveCover(bool open) override;
  mcucore::Status HaltCoverMotion() override;

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
  mcucore::TimerCounter3Pwm16Output led1_;
  mcucore::TimerCounter3Pwm16Output led2_;
  mcucore::TimerCounter3Pwm16Output led3_;
  mcucore::TimerCounter4Pwm16Output led4_;

  Cover cover_;

  bool calibrator_on_;
  uint16_t brightness_;
  uint8_t enabled_led_channels_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_COVER_CALIBRATOR_H_
