#include "led_channel_switch_group.h"

#include <McuCore.h>

namespace astro_makers {

using ::alpaca::AlpacaRequest;
using ::alpaca::DeviceInfo;
using ::alpaca::ErrorCodes;
using ::alpaca::WriteResponse;
using ::mcucore::AnyPrintable;
using ::mcucore::StatusOr;

LedChannelSwitchGroup::LedChannelSwitchGroup(const DeviceInfo& device_info,
                                             CoverCalibrator& cover_calibrator)
    : SwitchAdapter(device_info), cover_calibrator_(cover_calibrator) {}

uint16_t LedChannelSwitchGroup::GetMaxSwitch() { return 4; }

bool LedChannelSwitchGroup::HandleGetSwitchDescription(
    const AlpacaRequest& request, uint16_t switch_id, Print& out) {
  MCU_DCHECK_LT(switch_id, GetMaxSwitch());
  return WriteResponse::PrintableStringResponse(
      request,
      mcucore::PrintableCat(
          MCU_FLASHSTR("Enables Cover-Calibrator LED Channel #"),
          mcucore::AnyPrintable(switch_id),
          MCU_FLASHSTR(", if hardware is available")),
      out);
}

bool LedChannelSwitchGroup::HandleGetSwitchName(const AlpacaRequest& request,
                                                uint16_t switch_id,
                                                Print& out) {
  return WriteResponse::PrintableStringResponse(
      request, mcucore::PrintableCat(MCU_FLASHSTR("Enable LED #"), switch_id),
      out);
}

bool LedChannelSwitchGroup::HandleSetSwitchName(const AlpacaRequest& request,
                                                uint16_t switch_id,
                                                Print& out) {
  return WriteResponse::AscomMethodNotImplementedResponse(request, out);
}

bool LedChannelSwitchGroup::GetCanWrite(uint16_t switch_id) {
  return cover_calibrator_.GetLedChannelHardwareEnabled(switch_id);
}

mcucore::StatusOr<bool> LedChannelSwitchGroup::GetSwitch(uint16_t switch_id) {
  return cover_calibrator_.GetLedChannelEnabled(switch_id);
}

mcucore::StatusOr<double> LedChannelSwitchGroup::GetSwitchValue(
    uint16_t switch_id) {
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

mcucore::Status LedChannelSwitchGroup::SetSwitch(uint16_t switch_id,
                                                 bool state) {
  MCU_VLOG(2) << MCU_FLASHSTR("Set switch#") << switch_id << " to " << state;

  if (!GetCanWrite(switch_id)) {
    MCU_VLOG(2) << MCU_FLASHSTR("Can NOT write switch #") << switch_id;
    return ErrorCodes::NotImplemented();
  } else if (cover_calibrator_.SetLedChannelEnabled(switch_id, state) !=
             state) {
    MCU_VLOG(1) << MCU_FLASHSTR("Failed to set channel ") << switch_id
                << MCU_FLASHSTR(" to state ") << state;
  } else {
    MCU_VLOG(4) << MCU_FLASHSTR("Switch #") << switch_id
                << MCU_FLASHSTR(" now set to ") << state;
  }
  return mcucore::OkStatus();
}

mcucore::Status LedChannelSwitchGroup::SetSwitchValue(uint16_t switch_id,
                                                      double value) {
  return SetSwitch(switch_id, value >= 0.5);
}

}  // namespace astro_makers
