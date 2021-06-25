#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_LED_CHANNEL_SWITCH_GROUP_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_LED_CHANNEL_SWITCH_GROUP_H_

#include <TinyAlpacaServer.h>

#include "cover_calibrator.h"

namespace astro_makers {

class LedChannelSwitchGroup : public alpaca::SwitchAdapter {
 public:
  LedChannelSwitchGroup(const alpaca::DeviceInfo& device_info,
                        CoverCalibrator& cover_calibrator);

  // Overridden methods.
  bool HandleGetSwitchDescription(const alpaca::AlpacaRequest& request,
                                  uint16_t switch_id, Print& out) override;
  bool HandleGetSwitchName(const alpaca::AlpacaRequest& request,
                           uint16_t switch_id, Print& out) override;
  bool HandleSetSwitchName(const alpaca::AlpacaRequest& request,
                           uint16_t switch_id, Print& out) override;
  uint16_t GetMaxSwitch() override;
  bool GetCanWrite(uint16_t switch_id) override;
  alpaca::StatusOr<bool> GetSwitch(uint16_t switch_id) override;
  alpaca::StatusOr<double> GetSwitchValue(uint16_t switch_id) override;
  double GetMinSwitchValue(uint16_t switch_id) override;
  double GetMaxSwitchValue(uint16_t switch_id) override;
  double GetSwitchStep(uint16_t switch_id) override;
  alpaca::Status SetSwitch(uint16_t switch_id, bool state) override;
  alpaca::Status SetSwitchValue(uint16_t switch_id, double value) override;

 private:
  CoverCalibrator& cover_calibrator_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_LED_CHANNEL_SWITCH_GROUP_H_
