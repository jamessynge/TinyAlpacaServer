// TODO(jamessynge): Describe why this file exists/what it provides.

#include "tiny_alpaca_server.h"

#include "alpaca_response.h"
#include "literals.h"
#include "utils/logging.h"
#include "utils/platform.h"
#include "utils/platform_ethernet.h"

namespace alpaca {
namespace {
bool SocketIsConnected(int sock_num) {
  EthernetClient client(sock_num);
  auto status = client.status();
  return status == SnSR::ESTABLISHED || status == SnSR::CLOSE_WAIT;
}
}  // namespace

TinyAlpacaServer::TinyAlpacaServer(
    uint16_t tcp_port, const ServerDescription& server_description,
    ArrayView<DeviceApiHandlerBasePtr> device_handlers)
    : server_transaction_id_(0),
      tcp_port_(tcp_port),
      server_description_(server_description),
      device_handlers_(device_handlers) {
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    new (GetServerConnection(ndx)) ServerConnection(*this);
  }
}

bool TinyAlpacaServer::begin() {
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    // EthernetServer::begin() finds a closed hardware socket and puts it to use
    // listening for connections to a TCP port.
    EthernetServer listener(tcp_port_);
    listener.begin();
  }
  // TODO(jamessynge): Add UDP listener too.
  return true;
}

// Performs network IO.
void TinyAlpacaServer::loop() {
  // Find ServerConnections which have been disconnected.
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (conn && conn->has_socket()) {
      if (!SocketIsConnected(conn->sock_num())) {
        // Not connected. Call PerformIO which will detect the missing
        // connection and take the appropriate action.
        conn->PerformIO();
        TAS_DCHECK(!conn->has_socket(), conn->sock_num());
      }
    }
  }

  // Find new TCP connections and assign ServerConnections to them, and find
  // recently closed sockets that should resume listening.
  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    if (SocketIsConnected(sock_num)) {
      if (GetServerConnectionForSocket(sock_num) == nullptr) {
        // New connection.
        AssignServerConnectionToSocket(sock_num);
      }
    } else if (EthernetClass::_server_port[sock_num] == tcp_port_ &&
               PlatformEthernet::SocketIsClosed(sock_num)) {
      // Resume listening.
      PlatformEthernet::InitializeTcpListenerSocket(sock_num, tcp_port_);
    }
  }

  // For all the ServerConnections with TCP connections to clients, perform IO.
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (conn && conn->has_socket()) {
      conn->PerformIO();
    }
  }
}

ServerConnection* TinyAlpacaServer::GetServerConnection(size_t ndx) {
  return reinterpret_cast<ServerConnection*>(connections_storage_) + ndx;
}

ServerConnection* TinyAlpacaServer::GetServerConnectionForSocket(int sock_num) {
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (conn && conn->sock_num() == sock_num) {
      return conn;
    }
  }
  return nullptr;
}
bool TinyAlpacaServer::AssignServerConnectionToSocket(int sock_num) {
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (conn && !conn->has_socket() && conn->set_sock_num(sock_num)) {
      return true;
    }
  }
  TAS_LOG(WARNING, "There aren't enough ServerConnections");
  return false;
}

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
    return WriteResponse::HttpErrorResponse(
        EHttpStatusCode::kHttpInternalServerError,
        Literals::HttpMethodNotImplemented(), out);
  }
}

bool TinyAlpacaServer::HandleManagementApiVersions(AlpacaRequest& request,
                                                   Print& out) {
  return WriteResponse::AscomNotImplementedErrorResponse(request, out);
}

bool TinyAlpacaServer::HandleManagementDescription(AlpacaRequest& request,
                                                   Print& out) {
  return WriteResponse::AscomNotImplementedErrorResponse(request, out);
}

bool TinyAlpacaServer::HandleManagementConfiguredDevices(AlpacaRequest& request,
                                                         Print& out) {
  return WriteResponse::AscomNotImplementedErrorResponse(request, out);
}

bool TinyAlpacaServer::HandleServerSetup(AlpacaRequest& request, Print& out) {
  return WriteResponse::AscomNotImplementedErrorResponse(request, out);
}

}  // namespace alpaca
