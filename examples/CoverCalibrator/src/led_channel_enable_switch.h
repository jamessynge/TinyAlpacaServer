#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_LED_CHANNEL_ENABLE_SWITCH_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_LED_CHANNEL_ENABLE_SWITCH_H_

// LedChannelEnableSwitch provides remote control over whether an LED channel of
// a CoverCalibrator device is enabled.

#include <McuCore.h>
#include <TinyAlpacaServer.h>

#include "cover_calibrator.h"

namespace astro_makers {

class LedChannelEnableSwitch : public alpaca::ToggleSwitchBase {
 public:
  LedChannelEnableSwitch(CoverCalibrator& cover_calibrator, uint8_t channel);

  // Overridden methods.
  bool HandleGetSwitchDescription(const alpaca::AlpacaRequest& request,
                                  Print& out) override;
  bool HandleGetSwitchName(const alpaca::AlpacaRequest& request,
                           Print& out) override;
  bool HandleSetSwitchName(const alpaca::AlpacaRequest& request,
                           Print& out) override;
  bool GetCanWrite() override;
  mcucore::StatusOr<bool> GetSwitch() override;
  mcucore::Status SetSwitch(bool state) override;

 private:
  CoverCalibrator& cover_calibrator_;
  const uint8_t channel_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_LED_CHANNEL_ENABLE_SWITCH_H_
