#include "device_types/switch/switch_adapter.h"

#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "literals.h"

namespace alpaca {

SwitchAdapter::SwitchAdapter(const DeviceInfo& device_info)
    : DeviceImplBase(device_info) {
  TAS_DCHECK_EQ(device_info.device_type, EDeviceType::kSwitch);
}

void SwitchAdapter::Initialize() {
  DeviceImplBase::Initialize();
  TAS_CHECK_LE(0, GetMaxSwitch());
  TAS_CHECK_LE(GetMaxSwitch(), kMaxMaxSwitch);
}

bool SwitchAdapter::HandleGetRequest(const AlpacaRequest& request, Print& out) {
  TAS_DCHECK_EQ(request.api, EAlpacaApi::kDeviceApi);
  TAS_DCHECK_EQ(request.device_type, EDeviceType::kSwitch);
  TAS_DCHECK_EQ(request.device_number, device_number());

  // All but one of the Switch-specific GET methods needs the id parameter, but
  // not the common methods.
  bool handler_ret;
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
  TAS_DCHECK_EQ(request.api, EAlpacaApi::kDeviceApi);
  TAS_DCHECK_EQ(request.device_type, EDeviceType::kSwitch);
  TAS_DCHECK_EQ(request.device_number, device_number());

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
            request, Literals::State(), out);
      }
      return WriteResponse::StatusResponse(
          request, PutSetSwitch(request.id, request.state), out);

    case EDeviceMethod::kSetSwitchValue:
      // Requires the value parameter.
      if (!request.have_value) {
        return WriteResponse::AscomParameterMissingErrorResponse(
            request, Literals::Value(), out);
      }
      if ((request.value < GetMinSwitchValue(request.id)) ||
          (GetMaxSwitchValue(request.id) < request.value)) {
        return WriteResponse::AscomParameterInvalidErrorResponse(
            request, Literals::Value(), out);
      }
      return WriteResponse::StatusResponse(
          request, PutSetSwitchValue(request.id, request.value), out);

    case EDeviceMethod::kSetSwitchName:
      // TODO(jamessynge): Implement this.
      TAS_FALLTHROUGH_INTENDED;

    default:
      return DeviceImplBase::HandlePutRequest(request, out);
  }
}

bool SwitchAdapter::ValidateSwitchIdParameter(const AlpacaRequest& request,
                                              Print& out, bool& handler_ret) {
  if (request.have_id) {
    TAS_DCHECK_LE(0, GetMaxSwitch());
    TAS_DCHECK_LE(GetMaxSwitch(), kMaxMaxSwitch);
    if (0 <= request.id && request.id < GetMaxSwitch()) {
      return true;
    }
    handler_ret = WriteResponse::AscomParameterInvalidErrorResponse(
        request, Literals::Id(), out);
  } else {
    handler_ret = WriteResponse::AscomParameterMissingErrorResponse(
        request, Literals::Id(), out);
  }
  return false;
}

}  // namespace alpaca
