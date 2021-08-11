#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_MULTI_SWITCH_ADAPTER_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_MULTI_SWITCH_ADAPTER_H_

// Forwards calls to the appropriate SwitchInterface device.
//
// Author: james.synge@gmail.com

#include "array_view.h"
#include "device_types/switch/switch_adapter.h"
#include "device_types/switch/switch_interface.h"
#include "mcucore_platform.h"
#include "status_or.h"

namespace alpaca {

class MultiSwitchAdapter : public SwitchAdapter {
 public:
  MultiSwitchAdapter(const DeviceInfo& device_info,
                     mcucore::ArrayView<SwitchInterface*> switches);
  // ~MultiSwitchAdapter() override {}

  // Overrides of the base class methods:
  bool HandleGetSwitchDescription(const AlpacaRequest& request,
                                  uint16_t switch_id, Print& out) override;
  bool HandleGetSwitchName(const AlpacaRequest& request, uint16_t switch_id,
                           Print& out) override;
  bool HandleSetSwitchName(const AlpacaRequest& request, uint16_t switch_id,
                           Print& out) override;
  uint16_t GetMaxSwitch() override;
  bool GetCanWrite(uint16_t switch_id) override;
  mcucore::StatusOr<bool> GetSwitch(uint16_t switch_id) override;
  mcucore::StatusOr<double> GetSwitchValue(uint16_t switch_id) override;
  double GetMinSwitchValue(uint16_t switch_id) override;
  double GetMaxSwitchValue(uint16_t switch_id) override;
  double GetSwitchStep(uint16_t switch_id) override;
  mcucore::Status SetSwitch(uint16_t switch_id, bool state) override;
  mcucore::Status SetSwitchValue(uint16_t switch_id, double value) override;

 private:
  SwitchInterface* GetSwitchInterface(uint16_t switch_id) const;

  mcucore::ArrayView<SwitchInterface*> switches_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_MULTI_SWITCH_ADAPTER_H_
