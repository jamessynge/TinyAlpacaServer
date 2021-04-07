#include "tiny_alpaca_server.h"

#include "alpaca_response.h"
#include "literals.h"
#include "utils/any_printable.h"
#include "utils/logging.h"
#include "utils/platform_ethernet.h"
#include "utils/string_view.h"

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
    : TinyAlpacaRequestHandler(server_description, device_handlers),
      tcp_port_(tcp_port) {
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    new (GetServerConnection(ndx)) ServerConnection(*this);
  }
}

bool TinyAlpacaServer::Initialize() {
  if (!TinyAlpacaRequestHandler::Initialize()) {
    return false;
  }
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    // EthernetServer::begin() finds a closed hardware socket and puts it to use
    // listening for connections to a TCP port.
    EthernetServer listener(tcp_port_);
    listener.begin();
  }
  return true;
}

// Performs network IO.
void TinyAlpacaServer::PerformIO() {
  TinyAlpacaRequestHandler::MaintainDevices();

  // Find ServerConnections which have been disconnected.
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (conn && conn->has_socket()) {
      if (!SocketIsConnected(conn->sock_num())) {
        // Not connected. Call PerformIO which will detect the missing
        // connection and take the appropriate action.
        conn->PerformIO();
        TAS_DCHECK(!conn->has_socket()) << conn->sock_num();
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
  TAS_VLOG(1) << TASLIT("There aren't enough ServerConnections");
  return false;
}

}  // namespace alpaca
