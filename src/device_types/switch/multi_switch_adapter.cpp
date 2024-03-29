#include "device_types/switch/multi_switch_adapter.h"

#include "device_description.h"
#include "literals.h"

namespace alpaca {

MultiSwitchAdapter::MultiSwitchAdapter(
    ServerContext& server_context, const DeviceDescription& device_description,
    mcucore::ArrayView<SwitchInterface*> switches)
    : SwitchAdapter(server_context, device_description), switches_(switches) {}

SwitchInterface* MultiSwitchAdapter::GetSwitchInterface(
    uint16_t switch_id) const {
  MCU_CHECK_LT(switch_id, switches_.size());
  return switches_[switch_id];
}

// bool MultiSwitchAdapter::HandleGetSwitchDescription(
//     const AlpacaRequest& request, uint16_t switch_id, Print& out) {
//   return GetSwitchInterface(switch_id)->HandleGetSwitchDescription(request,
//                                                                    out);
// }

uint16_t MultiSwitchAdapter::GetMaxSwitch() { return switches_.size(); }

bool MultiSwitchAdapter::GetCanWrite(uint16_t switch_id) {
  return GetSwitchInterface(switch_id)->GetCanWrite();
}

mcucore::StatusOr<bool> MultiSwitchAdapter::GetSwitch(uint16_t switch_id) {
  return GetSwitchInterface(switch_id)->GetSwitch();
}

mcucore::StatusOr<double> MultiSwitchAdapter::GetSwitchValue(
    uint16_t switch_id) {
  return GetSwitchInterface(switch_id)->GetSwitchValue();
}

double MultiSwitchAdapter::GetMinSwitchValue(uint16_t switch_id) {
  return GetSwitchInterface(switch_id)->GetMinSwitchValue();
}

double MultiSwitchAdapter::GetMaxSwitchValue(uint16_t switch_id) {
  return GetSwitchInterface(switch_id)->GetMaxSwitchValue();
}

double MultiSwitchAdapter::GetSwitchStep(uint16_t switch_id) {
  return GetSwitchInterface(switch_id)->GetSwitchStep();
}

mcucore::Status MultiSwitchAdapter::SetSwitch(uint16_t switch_id, bool state) {
  return GetSwitchInterface(switch_id)->SetSwitch(state);
}

mcucore::Status MultiSwitchAdapter::SetSwitchValue(uint16_t switch_id,
                                                   double value) {
  return GetSwitchInterface(switch_id)->SetSwitchValue(value);
}

}  // namespace alpaca
