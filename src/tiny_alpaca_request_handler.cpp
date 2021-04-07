#include "tiny_alpaca_request_handler.h"

#include "alpaca_response.h"
#include "literals.h"
#include "utils/any_printable.h"
#include "utils/logging.h"
#include "utils/platform_ethernet.h"
#include "utils/string_view.h"

namespace alpaca {

TinyAlpacaRequestHandler::TinyAlpacaRequestHandler(
    const ServerDescription& server_description,
    ArrayView<DeviceApiHandlerBasePtr> device_handlers)
    : server_transaction_id_(0),
      server_description_(server_description),
      device_handlers_(device_handlers) {
}

bool TinyAlpacaRequestHandler::Initialize() {
  for (DeviceApiHandlerBasePtr handler : device_handlers_) {
    handler->Initialize();
  }
  return true;
}

void TinyAlpacaRequestHandler::MaintainDevices() {
  // Give devices a chance to perform work.
  for (DeviceApiHandlerBasePtr handler : device_handlers_) {
    handler->Update();
  }
}

void TinyAlpacaRequestHandler::OnStartDecoding(AlpacaRequest& request) {
  request.set_server_transaction_id(++server_transaction_id_);
}

bool TinyAlpacaRequestHandler::OnRequestDecoded(AlpacaRequest& request, Print& out) {
  TAS_VLOG(3) << TASLIT("OnRequestDecoded: api=") << request.api;
  switch (request.api) {
    case EAlpacaApi::kUnknown:
      break;

    case EAlpacaApi::kDeviceApi:
      // ABSL_FALLTHROUGH_INTENDED
    case EAlpacaApi::kDeviceSetup:
      TAS_VLOG(3) << TASLIT("OnRequestDecoded: device_type=")
                  << request.device_type << TASLIT(", device_number=")
                  << request.device_number;
      for (DeviceApiHandlerBasePtr handler : device_handlers_) {
        if (request.device_type == handler->device_type() &&
            request.device_number == handler->device_number()) {
          return DispatchDeviceRequest(request, *handler, out);
        }
      }
      TAS_VLOG(3) << TASLIT("OnRequestDecoded: found no Device API Handler");
      // Should this be an ASCOM error, or is an HTTP status OK?
      return WriteResponse::HttpErrorResponse(EHttpStatusCode::kHttpNotFound,
                                              Literals::NoSuchDevice(), out);

    case EAlpacaApi::kManagementApiVersions:
      return HandleManagementApiVersions(request, out);

    case EAlpacaApi::kManagementDescription:
      return HandleManagementDescription(request, out);

    case EAlpacaApi::kManagementConfiguredDevices:
      return HandleManagementConfiguredDevices(request, out);

    case EAlpacaApi::kServerSetup:
      return HandleServerSetup(request, out);
  }

  return WriteResponse::HttpErrorResponse(
      EHttpStatusCode::kHttpInternalServerError, Literals::ApiUnknown(), out);
}

// Called when decoding of a request has failed. 'out' should be used to write
// an error response to the client. The connection to the client will be
// closed after the response is returned.
void TinyAlpacaRequestHandler::OnRequestDecodingError(AlpacaRequest& request,
                                              EHttpStatusCode status,
                                              Print& out) {
  TAS_VLOG(3) << TASLIT("OnRequestDecodingError: status=") << status;
  WriteResponse::HttpErrorResponse(status, AnyPrintable(), out);
}

bool TinyAlpacaRequestHandler::DispatchDeviceRequest(AlpacaRequest& request,
                                             DeviceApiHandlerBase& handler,
                                             Print& out) {
  if (request.api == EAlpacaApi::kDeviceApi) {
    TAS_VLOG(3) << TASLIT("DispatchDeviceRequest: device_method=")
                << request.device_method;
    return handler.HandleDeviceApiRequest(request, out);
  } else if (request.api == EAlpacaApi::kDeviceSetup) {
    return handler.HandleDeviceApiRequest(request, out);
  } else {
    return WriteResponse::HttpErrorResponse(
        EHttpStatusCode::kHttpInternalServerError,
        Literals::HttpMethodNotImplemented(), out);
  }
}

bool TinyAlpacaRequestHandler::HandleManagementApiVersions(AlpacaRequest& request,
                                                   Print& out) {
  TAS_VLOG(3) << TASLIT("HandleManagementApiVersions");
  return WriteResponse::AscomNotImplementedErrorResponse(request, out);
}

bool TinyAlpacaRequestHandler::HandleManagementDescription(AlpacaRequest& request,
                                                   Print& out) {
  TAS_VLOG(3) << TASLIT("HandleManagementDescription");
  return WriteResponse::AscomNotImplementedErrorResponse(request, out);
}

bool TinyAlpacaRequestHandler::HandleManagementConfiguredDevices(AlpacaRequest& request,
                                                         Print& out) {
  TAS_VLOG(3) << TASLIT("HandleManagementConfiguredDevices");
  return WriteResponse::AscomNotImplementedErrorResponse(request, out);
}

bool TinyAlpacaRequestHandler::HandleServerSetup(AlpacaRequest& request, Print& out) {
  TAS_VLOG(3) << TASLIT("HandleServerSetup");
  return WriteResponse::AscomNotImplementedErrorResponse(request, out);
}

}  // namespace alpaca
