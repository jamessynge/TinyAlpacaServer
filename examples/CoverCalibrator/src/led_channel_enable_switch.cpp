#include "led_channel_enable_switch.h"

namespace astro_makers {

using ::alpaca::AlpacaRequest;
using ::alpaca::ErrorCodes;
using ::alpaca::WriteResponse;
using ::mcucore::OkStatus;
using ::mcucore::PrintableCat;
using ::mcucore::Status;
using ::mcucore::StatusOr;

LedChannelEnableSwitch::LedChannelEnableSwitch(
    CoverCalibrator& cover_calibrator, uint8_t channel)
    : cover_calibrator_(cover_calibrator), channel_(channel) {}

bool LedChannelEnableSwitch::HandleGetSwitchDescription(
    const AlpacaRequest& request, Print& out) {
  return WriteResponse::PrintableStringResponse(
      request,
      PrintableCat(MCU_PSD("Enables CoverCalibrator LED Channel #"), channel_,
                   MCU_PSD(", if hardware available")),
      out);
}

bool LedChannelEnableSwitch::HandleGetSwitchName(const AlpacaRequest& request,
                                                 Print& out) {
  return WriteResponse::PrintableStringResponse(
      request, PrintableCat(MCU_PSD("Enable LED "), channel_), out);
}

bool LedChannelEnableSwitch::HandleSetSwitchName(const AlpacaRequest& request,
                                                 Print& out) {
  return WriteResponse::AscomNotImplementedResponse(request, out);
}

bool LedChannelEnableSwitch::GetCanWrite() {
  return cover_calibrator_.GetLedChannelHardwareEnabled(channel_);
}

StatusOr<bool> LedChannelEnableSwitch::GetSwitch() {
  return cover_calibrator_.GetLedChannelEnabled(channel_);
}

Status LedChannelEnableSwitch::SetSwitch(bool state) {
  if (!GetCanWrite()) {
    return ErrorCodes::NotImplemented();
  } else if (cover_calibrator_.SetLedChannelEnabled(channel_, state) != state) {
    MCU_DCHECK(false) << MCU_PSD("Failed to set channel ") << channel_ << " to "
                      << state;
  }
  return OkStatus();
}

}  // namespace astro_makers
