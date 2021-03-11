#include "device_api_handler_base.h"

#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "http_response_header.h"
#include "literals.h"
#include "utils/counting_bitbucket.h"
#include "utils/json_encoder.h"
#include "utils/platform.h"

namespace alpaca {

DeviceApiHandlerBase::DeviceApiHandlerBase(const DeviceInfo& device_info)
    : device_info_(device_info) {}
DeviceApiHandlerBase::~DeviceApiHandlerBase() {}

bool DeviceApiHandlerBase::HandleDeviceSetupRequest(
    const AlpacaRequest& request, Print& out) {
  return WriteAscomErrorResponse(request,
                                 ErrorCodes::ActionNotImplemented().code(),
                                 Literals::HttpMethodNotImplemented(), out);
}

bool DeviceApiHandlerBase::HandleDeviceApiRequest(const AlpacaRequest& request,
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
  return WriteHttpErrorResponse(EHttpStatusCode::kHttpInternalServerError,
                                Literals::HttpMethodNotImplemented(), out);
}

bool DeviceApiHandlerBase::HandleGetRequest(const AlpacaRequest& request,
                                            Print& out) {
  switch (request.device_method) {
    case EDeviceMethod::kConnected:
      return WriteBoolResponse(request, GetConnected(), out);

    case EDeviceMethod::kDescription:
      return WriteStringResponse(request, device_info_.description, out);

    case EDeviceMethod::kDriverInfo:
      return WriteStringResponse(request, device_info_.driver_info, out);

    case EDeviceMethod::kDriverVersion:
      return WriteStringResponse(request, device_info_.driver_version, out);

    case EDeviceMethod::kInterfaceVersion:
      return WriteIntegerResponse(request, device_info_.interface_version, out);

    case EDeviceMethod::kName:
      return WriteStringResponse(request, device_info_.name, out);

    case EDeviceMethod::kSupportedActions:
      return WriteLiteralArrayResponse(request, device_info_.supported_actions,
                                       out);

    default:
      // TODO(jamessynge): Write a NOT IMPLEMENTED error response.
      return WriteAscomErrorResponse(request,
                                     ErrorCodes::ActionNotImplemented().code(),
                                     Literals::HttpMethodNotImplemented(), out);
  }
}

}  // namespace alpaca
