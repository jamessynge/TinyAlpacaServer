#include "led_channel_switch_group.h"

#include <McuCore.h>
#include <TinyAlpacaServer.h>

#include "cover_calibrator.h"

namespace astro_makers {

using ::alpaca::AlpacaRequest;
using ::alpaca::ErrorCodes;
using ::alpaca::WriteResponse;
using ::mcucore::StatusOr;

LedChannelSwitchGroup::LedChannelSwitchGroup(
    alpaca::ServerContext& server_context,
    const alpaca::DeviceDescription& device_description,
    CoverCalibrator& cover_calibrator)
    : SwitchAdapter(server_context, device_description),
      cover_calibrator_(cover_calibrator) {}

uint16_t LedChannelSwitchGroup::GetMaxSwitch() { return 4; }

bool LedChannelSwitchGroup::HandleGetSwitchDescription(
    const AlpacaRequest& request, uint16_t switch_id, Print& out) {
  // Not checking switch_id for validity, that should have been done by the
  // caller.
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
  MCU_VLOG(2) << MCU_PSD("Set switch#") << switch_id << " to " << state;

  if (!GetCanWrite(switch_id)) {
    MCU_VLOG(2) << MCU_PSD("Can NOT write switch #") << switch_id;
    return ErrorCodes::NotImplemented();
  } else if (cover_calibrator_.SetLedChannelEnabled(switch_id, state) !=
             state) {
    MCU_VLOG(1) << MCU_PSD("Failed to set channel ") << switch_id
                << MCU_PSD(" to state ") << state;
  } else {
    MCU_VLOG(4) << MCU_PSD("Switch #") << switch_id << MCU_PSD(" now set to ")
                << state;
  }
  return mcucore::OkStatus();
}

mcucore::Status LedChannelSwitchGroup::SetSwitchValue(uint16_t switch_id,
                                                      double value) {
  return SetSwitch(switch_id, value >= 0.5);
}

}  // namespace astro_makers
