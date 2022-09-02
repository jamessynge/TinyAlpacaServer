#include "device_types/switch/switch_adapter.h"

#include <McuCore.h>

#include "alpaca_response.h"
#include "constants.h"
#include "eeprom_ids.h"
#include "literals.h"

namespace alpaca {

using mcucore::TinyString;

SwitchAdapter::SwitchAdapter(ServerContext& server_context,
                             const DeviceDescription& device_description)
    : DeviceImplBase(server_context, device_description) {
  MCU_DCHECK_EQ(device_description.device_type, EDeviceType::kSwitch);
}

SwitchAdapter::~SwitchAdapter() {}

void SwitchAdapter::ValidateConfiguration() {
  MCU_CHECK_LE(0, GetMaxSwitch());
  MCU_CHECK_LE(GetMaxSwitch(), kMaxMaxSwitch);
  ValidateSwitchDeviceConfiguration();
}

bool SwitchAdapter::HandleGetRequest(const AlpacaRequest& request, Print& out) {
  MCU_DCHECK_EQ(request.api, EAlpacaApi::kDeviceApi);
  MCU_DCHECK_EQ(request.device_type, EDeviceType::kSwitch);
  MCU_DCHECK_EQ(request.device_number, device_description().device_number);

  // Except for 'maxswitch', all of the Switch-specific GET methods needs the id
  // parameter.
  bool handler_ret = true;
  switch (request.device_method) {
    case EDeviceMethod::kCanWrite:
    case EDeviceMethod::kGetSwitch:
    case EDeviceMethod::kGetSwitchDescription:
    case EDeviceMethod::kGetSwitchName:
    case EDeviceMethod::kGetSwitchValue:
    case EDeviceMethod::kMinSwitchValue:
    case EDeviceMethod::kMaxSwitchValue:
    case EDeviceMethod::kSwitchStep:
      if (!ValidateSwitchIdParameter(request, out, handler_ret)) {
        return handler_ret;
      }
      break;

    default:
      break;
  }

  switch (request.device_method) {
    case EDeviceMethod::kMaxSwitch:
      return WriteResponse::IntResponse(request, GetMaxSwitch(), out);

    case EDeviceMethod::kCanWrite:
      return WriteResponse::BoolResponse(request, GetCanWrite(request.id), out);

    case EDeviceMethod::kGetSwitch:
      return WriteResponse::StatusOrBoolResponse(request, GetSwitch(request.id),
                                                 out);

    case EDeviceMethod::kGetSwitchDescription:
      return HandleGetSwitchDescription(request, request.id, out);

    case EDeviceMethod::kGetSwitchName:
      return HandleGetSwitchName(request, request.id, out);

    case EDeviceMethod::kGetSwitchValue:
      return WriteResponse::StatusOrDoubleResponse(
          request, GetSwitchValue(request.id), out);

    case EDeviceMethod::kMinSwitchValue:
      return WriteResponse::StatusOrDoubleResponse(
          request, GetMinSwitchValue(request.id), out);

    case EDeviceMethod::kMaxSwitchValue:
      return WriteResponse::StatusOrDoubleResponse(
          request, GetMaxSwitchValue(request.id), out);

    case EDeviceMethod::kSwitchStep:
      return WriteResponse::StatusOrDoubleResponse(
          request, GetSwitchStep(request.id), out);

    default:
      return DeviceImplBase::HandleGetRequest(request, out);
  }
}

//////////////////////////////////////////////////////////////////////////////

// Handle a PUT 'request', write the HTTP response message to out.
bool SwitchAdapter::HandlePutRequest(const AlpacaRequest& request, Print& out) {
  MCU_DCHECK_EQ(request.api, EAlpacaApi::kDeviceApi);
  MCU_DCHECK_EQ(request.device_type, EDeviceType::kSwitch);
  MCU_DCHECK_EQ(request.device_number, device_description().device_number);

  // All of the Switch-specific PUT methods need the id parameter, but not the
  // common methods.
  bool handler_ret;
  switch (request.device_method) {
    case EDeviceMethod::kSetSwitch:
    case EDeviceMethod::kSetSwitchName:
    case EDeviceMethod::kSetSwitchValue:
      if (!ValidateSwitchIdParameter(request, out, handler_ret)) {
        return handler_ret;
      }
      break;

    default:
      break;
  }

  switch (request.device_method) {
    case EDeviceMethod::kSetSwitch:
      // Requires the state parameter.
      if (!request.have_state) {
        return WriteResponse::AscomParameterMissingErrorResponse(
            request, ProgmemStringViews::State(), out);
      }
      return WriteResponse::StatusResponse(
          request, SetSwitch(request.id, request.state), out);

    case EDeviceMethod::kSetSwitchName:
      if (!request.have_string_value || request.string_value.empty()) {
        return WriteResponse::AscomParameterMissingErrorResponse(
            request, ProgmemStringViews::Name(), out);
      }
      // TODO(jamessynge): Implement
      MCU_CHECK(false) << MCU_PSD("NOT IMPLEMENTED");
      return false;

    case EDeviceMethod::kSetSwitchValue:
      // Requires the value parameter.
      if (!request.have_value) {
        return WriteResponse::AscomParameterMissingErrorResponse(
            request, ProgmemStringViews::Value(), out);
      }
      if (request.value < GetMinSwitchValue(request.id) ||
          GetMaxSwitchValue(request.id) < request.value) {
        return WriteResponse::AscomParameterInvalidErrorResponse(
            request, ProgmemStringViews::Value(), out);
      }
      return WriteResponse::StatusResponse(
          request, SetSwitchValue(request.id, request.value), out);

      // TODO(jamessynge): Verify that the request has a valid name parameter.
      return HandleSetSwitchName(request, request.id, out);

    default:
      return DeviceImplBase::HandlePutRequest(request, out);
  }
}

bool SwitchAdapter::HandleGetSwitchName(const AlpacaRequest& request,
                                        uint16_t switch_id, Print& out) {
  mcucore::TinyString<kMaxNameLength> name_buffer;
  if (!ReadSwitchName(switch_id, name_buffer)) {
    name_buffer.Clear();
    GenerateSwitchName(switch_id, name_buffer);
  }
  return WriteResponse::PrintableStringResponse(
      request, mcucore::AnyPrintable(name_buffer), out);
}

bool SwitchAdapter::HandleSetSwitchName(const AlpacaRequest& request,
                                        uint16_t switch_id, Print& out) {
  MCU_CHECK(false) << MCU_PSD("NOT IMPLEMENTED");
  return false;
}

bool SwitchAdapter::ValidateSwitchIdParameter(const AlpacaRequest& request,
                                              Print& out, bool& handler_ret) {
  if (request.have_id) {
    MCU_DCHECK_LE(0, GetMaxSwitch());
    MCU_DCHECK_LE(GetMaxSwitch(), kMaxMaxSwitch);
    if (0 <= request.id && request.id < GetMaxSwitch()) {
      return true;
    }
    handler_ret = WriteResponse::AscomParameterInvalidErrorResponse(
        request, ProgmemStringViews::Id(), out);
    return false;
  } else {
    handler_ret = WriteResponse::AscomParameterMissingErrorResponse(
        request, ProgmemStringViews::Id(), out);
    return false;
  }
}

bool SwitchAdapter::ReadSwitchName(uint16_t switch_id,
                                   TinyString<kMaxNameLength>& name) {
  // Not overridden by subclass, so we have a much stricter limit on the number
  // of supported switches:
  MCU_CHECK_LE(GetMaxSwitch(), kMaxSwitchesForName);

  auto status_or_tlv = mcucore::EepromTlv::GetIfValid();
  MCU_DCHECK_OK(status_or_tlv);
  if (!status_or_tlv.ok()) {
    return false;
  }

  auto tlv = status_or_tlv.value();
  auto tag = MakeTag(kSwitch0NameId + switch_id);

  static_assert(sizeof(char) == sizeof(uint8_t));
  auto status_or_size = tlv.ReadEntry(
      tag, reinterpret_cast<uint8_t*>(name.data()), name.maximum_size());

  if (!status_or_size.ok()) {
    MCU_DCHECK(mcucore::IsNotFound(status_or_size.status()))
        << status_or_size.status();
    return false;
  }

  // Success!
  name.set_size(status_or_size.value());
  return true;
}

void SwitchAdapter::GenerateSwitchName(uint16_t switch_id,
                                       TinyString<kMaxNameLength>& name) {
  mcucore::PrintToBuffer p2b(name);
  p2b.print(MCU_FLASHSTR("Switch #"));
  p2b.print(switch_id);
  name.set_size(name.size() + p2b.data_size());
}

bool SwitchAdapter::WriteSwitchName(const AlpacaRequest& request,
                                    uint16_t switch_id,
                                    const mcucore::StringView& name,
                                    Print& out) {
  // Not overridden by subclass, so we have a much stricter limit on the number
  // of supported switches:
  MCU_CHECK_LE(GetMaxSwitch(), kMaxSwitchesForName);

  auto status_or_tlv = mcucore::EepromTlv::GetIfValid();
  MCU_DCHECK_OK(status_or_tlv);
  if (!status_or_tlv.ok()) {
    return WriteResponse::StatusResponse(request, status_or_tlv.status(), out);
  }

  auto tlv = status_or_tlv.value();
  auto tag = MakeTag(kSwitch0NameId + switch_id);

  auto status = tlv.WriteEntry(
      tag, reinterpret_cast<const uint8_t*>(name.data()), name.size());
  return WriteResponse::StatusResponse(request, status, out);
}

}  // namespace alpaca
