#include "led_channel_switch_group.h"

namespace astro_makers {

using ::alpaca::AlpacaRequest;
using ::alpaca::DeviceInfo;
using ::alpaca::ErrorCodes;
using ::alpaca::OkStatus;
using ::alpaca::PrintableCat;
using ::alpaca::Status;
using ::alpaca::StatusOr;
using ::alpaca::WriteResponse;

LedChannelSwitchGroup::LedChannelSwitchGroup(const DeviceInfo& device_info,
                                             CoverCalibrator& cover_calibrator)
    : SwitchAdapter(device_info), cover_calibrator_(cover_calibrator) {}

uint16_t LedChannelSwitchGroup::GetMaxSwitch() { return 4; }

bool LedChannelSwitchGroup::HandleGetSwitchDescription(
    const AlpacaRequest& request, uint16_t switch_id, Print& out) {
  TAS_DCHECK_LT(switch_id, GetMaxSwitch());
  return WriteResponse::PrintableStringResponse(
      request,
      PrintableCat(TASLIT("Enables Cover-Calibrator LED Channel #"), switch_id,
                   TASLIT(", if hardware is available")),
      out);
}

bool LedChannelSwitchGroup::HandleGetSwitchName(const AlpacaRequest& request,
                                                uint16_t switch_id,
                                                Print& out) {
  return WriteResponse::PrintableStringResponse(
      request, PrintableCat(TASLIT("Enable LED #"), switch_id), out);
}

bool LedChannelSwitchGroup::HandleSetSwitchName(const AlpacaRequest& request,
                                                uint16_t switch_id,
                                                Print& out) {
  return WriteResponse::AscomNotImplementedResponse(request, out);
}

bool LedChannelSwitchGroup::GetCanWrite(uint16_t switch_id) {
  return cover_calibrator_.GetLedChannelHardwareEnabled(switch_id);
}

StatusOr<bool> LedChannelSwitchGroup::GetSwitch(uint16_t switch_id) {
  return cover_calibrator_.GetLedChannelEnabled(switch_id);
}

StatusOr<double> LedChannelSwitchGroup::GetSwitchValue(uint16_t switch_id) {
  if (cover_calibrator_.GetLedChannelEnabled(switch_id)) {
    return 1.0;
  } else {
    return 0.0;
  }
}

double LedChannelSwitchGroup::GetMinSwitchValue(uint16_t switch_id) {
  return 0;
}

double LedChannelSwitchGroup::GetMaxSwitchValue(uint16_t switch_id) {
  return 1;
}

double LedChannelSwitchGroup::GetSwitchStep(uint16_t switch_id) { return 1; }

Status LedChannelSwitchGroup::SetSwitch(uint16_t switch_id, bool state) {
  if (!GetCanWrite(switch_id)) {
    return ErrorCodes::NotImplemented();
  } else if (cover_calibrator_.SetLedChannelEnabled(switch_id, state) !=
             state) {
    TAS_DCHECK(false) << TASLIT("Failed to set channel ") << switch_id
                      << TASLIT(" to state ") << state;
  }
  return OkStatus();
}

Status LedChannelSwitchGroup::SetSwitchValue(uint16_t switch_id, double value) {
  return SetSwitch(switch_id, value >= 0.5);
}

}  // namespace astro_makers
