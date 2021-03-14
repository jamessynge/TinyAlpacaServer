// TODO(jamessynge): Describe why this file exists/what it provides.

#include "tiny_alpaca_server.h"

#include "alpaca_response.h"
#include "literals.h"
#include "utils/any_string.h"
#include "utils/platform.h"

namespace alpaca {
namespace {}

TinyAlpacaServer::TinyAlpacaServer(
    uint16_t tcp_port, const ServerDescription& server_description,
    ArrayView<DeviceApiHandlerBasePtr> device_handlers)
    : server_transaction_id_(0),
      server_description_(server_description),
      device_handlers_(device_handlers) {
  InitializeServerConnections(tcp_port);
}

ServerConnection* TinyAlpacaServer::GetServerConnection(size_t ndx) {
  return reinterpret_cast<ServerConnection*>(connections_storage_) + ndx;
}

void TinyAlpacaServer::InitializeServerConnections(uint16_t tcp_port) {
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    // Use 'placement new' to initialize an array within this TinyAlpacaServer
    // with references to this instance.
    //    void* memory = GetServerConnection(ndx);
    //    new (memory) ServerConnection(/*sock_num=*/ndx, tcp_port, *this);
    new (GetServerConnection(ndx))
        ServerConnection(/*sock_num=*/ndx, tcp_port, *this);
  }
}

bool TinyAlpacaServer::begin() {
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (!conn->BeginListening()) {
      TAS_LOG(ERROR, "Unable to initialize ServerConnection #" << ndx);
      return false;
    }
  }
  // TODO(jamessynge): Add UDP listener too.
  return true;
}

// Performs network IO as appropriate.
void TinyAlpacaServer::loop() {}

void TinyAlpacaServer::OnStartDecoding(AlpacaRequest& request) {
  request.set_server_transaction_id(++server_transaction_id_);
}

// Called when a request has been successfully decoded. 'out' should be used
// to write a response to the client. Return true to continue decoding more
// requests from the client, false to disconnect.
bool TinyAlpacaServer::OnRequestDecoded(AlpacaRequest& request, Print& out) {
  switch (request.api) {
    case EAlpacaApi::kUnknown:
      break;

    case EAlpacaApi::kDeviceApi:
      // ABSL_FALLTHROUGH_INTENDED
    case EAlpacaApi::kDeviceSetup:
      for (DeviceApiHandlerBasePtr handler : device_handlers_) {
        if (request.device_type == handler->device_type() &&
            request.device_number == handler->device_number()) {
          return DispatchDeviceRequest(request, *handler, out);
        }
      }
      // Should this be an ASCOM error, or is an HTTP status OK?
      return WriteHttpErrorResponse(EHttpStatusCode::kHttpNotFound,
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

  return WriteHttpErrorResponse(EHttpStatusCode::kHttpInternalServerError,
                                Literals::ApiUnknown(), out);
}

// Called when decoding of a request has failed. 'out' should be used to write
// an error response to the client. The connection to the client will be
// closed after the response is returned.
void TinyAlpacaServer::OnRequestDecodingError(AlpacaRequest& request,
                                              EHttpStatusCode status,
                                              Print& out) {}

bool TinyAlpacaServer::DispatchDeviceRequest(AlpacaRequest& request,
                                             DeviceApiHandlerBase& handler,
                                             Print& out) {
  if (request.api == EAlpacaApi::kDeviceApi) {
    return handler.HandleDeviceApiRequest(request, out);
  } else if (request.api == EAlpacaApi::kDeviceSetup) {
    return handler.HandleDeviceApiRequest(request, out);
  } else {
    return WriteHttpErrorResponse(EHttpStatusCode::kHttpInternalServerError,
                                  Literals::HttpMethodNotImplemented(), out);
  }
}

bool TinyAlpacaServer::HandleManagementApiVersions(AlpacaRequest& request,
                                                   Print& out) {
  return false;
}

bool TinyAlpacaServer::HandleManagementDescription(AlpacaRequest& request,
                                                   Print& out) {
  return false;
}

bool TinyAlpacaServer::HandleManagementConfiguredDevices(AlpacaRequest& request,
                                                         Print& out) {
  return false;
}

bool TinyAlpacaServer::HandleServerSetup(AlpacaRequest& request, Print& out) {
  return false;
}

}  // namespace alpaca
