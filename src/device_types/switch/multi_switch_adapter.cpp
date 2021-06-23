#include "device_types/switch/multi_switch_adapter.h"

#include "device_info.h"

// TODO(jamessynge): Describe why this file exists/what it provides.

#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "literals.h"

namespace alpaca {

MultiSwitchAdapter::MultiSwitchAdapter(const DeviceInfo& device_info,
                                       ArrayView<SwitchInterface*> switches)
    : SwitchAdapter(device_info), switches_(switches) {}

SwitchInterface* MultiSwitchAdapter::GetSwitchInterface(
    uint16_t switch_id) const {
  TAS_CHECK_LT(switch_id, switches_.size());
  return switches_[switch_id];
}

bool MultiSwitchAdapter::HandleGetSwitchDescription(
    const AlpacaRequest& request, uint16_t switch_id, Print& out) {
  return GetSwitchInterface(switch_id)->HandleGetSwitchDescription(request,
                                                                   out);
}

bool MultiSwitchAdapter::HandleGetSwitchName(const AlpacaRequest& request,
                                             uint16_t switch_id, Print& out) {
  return GetSwitchInterface(switch_id)->HandleGetSwitchName(request, out);
}

bool MultiSwitchAdapter::HandleSetSwitchName(const AlpacaRequest& request,
                                             uint16_t switch_id, Print& out) {
  return GetSwitchInterface(switch_id)->HandleSetSwitchName(request, out);
}

uint16_t MultiSwitchAdapter::GetMaxSwitch() { return switches_.size(); }

bool MultiSwitchAdapter::GetCanWrite(uint16_t switch_id) {
  return GetSwitchInterface(switch_id)->GetCanWrite();
}

StatusOr<bool> MultiSwitchAdapter::GetSwitch(uint16_t switch_id) {
  return GetSwitchInterface(switch_id)->GetSwitch();
}

StatusOr<double> MultiSwitchAdapter::GetSwitchValue(uint16_t switch_id) {
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

Status MultiSwitchAdapter::SetSwitch(uint16_t switch_id, bool state) {
  return GetSwitchInterface(switch_id)->SetSwitch(state);
}

Status MultiSwitchAdapter::SetSwitchValue(uint16_t switch_id, double value) {
  return GetSwitchInterface(switch_id)->SetSwitchValue(value);
}

}  // namespace alpaca
