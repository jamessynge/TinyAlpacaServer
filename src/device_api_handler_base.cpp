#include "device_api_handler_base.h"

#include "alpaca_response.h"
#include "constants.h"
#include "counting_bitbucket.h"
#include "http_response_header.h"
#include "json_encoder.h"
#include "literal.h"
#include "platform.h"

namespace alpaca {

DeviceApiHandlerBase::DeviceApiHandlerBase(const DeviceInfo& device_info)
    : device_info_(device_info) {}
DeviceApiHandlerBase::~DeviceApiHandlerBase() {}

void DeviceApiHandlerBase::HandleGetRequest(const AlpacaRequest& request,
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
      return;
  }
}

}  // namespace alpaca
