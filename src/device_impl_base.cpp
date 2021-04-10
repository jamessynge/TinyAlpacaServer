#include "device_impl_base.h"

#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "http_response_header.h"
#include "literals.h"
#include "utils/counting_bitbucket.h"
#include "utils/json_encoder.h"

namespace alpaca {

bool DeviceImplBase::HandleDeviceSetupRequest(const AlpacaRequest& request,
                                              Print& out) {
  return WriteResponse::AscomErrorResponse(
      request, ErrorCodes::ActionNotImplemented().code(),
      Literals::HttpMethodNotImplemented(), out);
}

bool DeviceImplBase::HandleDeviceApiRequest(const AlpacaRequest& request,
                                            Print& out) {
  switch (request.http_method) {
    case EHttpMethod::GET:
    case EHttpMethod::HEAD:
      return HandleGetRequest(request, out);

    case EHttpMethod::PUT:
      return HandlePutRequest(request, out);

    case EHttpMethod::kUnknown:
      break;
  }

  // We shouldn't get here because we shouldn't have decoded an http_method not
  // explicitly listed above. So returning kHttpInternalServerError rather than
  // kHttpMethodNotImplemented, but using the HttpMethodNotImplemented reason
  // phrase.
  return WriteResponse::HttpErrorResponse(
      EHttpStatusCode::kHttpInternalServerError,
      Literals::HttpMethodNotImplemented(), out);
}

bool DeviceImplBase::HandleGetRequest(const AlpacaRequest& request,
                                      Print& out) {
  switch (request.device_method) {
    case EDeviceMethod::kConnected:
      return WriteResponse::StatusOrBoolResponse(request, GetConnected(), out);

    case EDeviceMethod::kDescription:
      return WriteResponse::AnyPrintableStringResponse(
          request, device_info_.description, out);

    case EDeviceMethod::kDriverInfo:
      return WriteResponse::AnyPrintableStringResponse(
          request, device_info_.driver_info, out);

    case EDeviceMethod::kDriverVersion:
      return WriteResponse::AnyPrintableStringResponse(
          request, device_info_.driver_version, out);

    case EDeviceMethod::kInterfaceVersion:
      return WriteResponse::IntResponse(request, device_info_.interface_version,
                                        out);

    case EDeviceMethod::kName:
      return WriteResponse::AnyPrintableStringResponse(request,
                                                       device_info_.name, out);

    case EDeviceMethod::kSupportedActions:
      return WriteResponse::LiteralArrayResponse(
          request, device_info_.supported_actions, out);

    default:
      return WriteResponse::AscomErrorResponse(
          request, ErrorCodes::ActionNotImplemented(), out);
  }
}

bool DeviceImplBase::HandlePutRequest(const AlpacaRequest& request,
                                      Print& out) {
  return WriteResponse::HttpErrorResponse(
      EHttpStatusCode::kHttpInternalServerError,
      Literals::HttpMethodNotImplemented(), out);
}

}  // namespace alpaca
