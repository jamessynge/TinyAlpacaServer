#include "server_connections.h"

#include "utils/platform.h"

namespace alpaca {

namespace {
bool SocketIsConnected(int sock_num) {
  EthernetClient client(sock_num);
  auto status = client.status();
  return status == SnSR::ESTABLISHED || status == SnSR::CLOSE_WAIT;
}
}  // namespace

ServerConnections::ServerConnections(RequestListener& request_listener,
                                     uint16_t tcp_port)
    : tcp_port_(tcp_port) {
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    new (GetServerConnection(ndx)) ServerConnection(request_listener);
  }
}

bool ServerConnections::Initialize() {
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    // EthernetServer::begin() finds a closed hardware socket and puts it to use
    // listening for connections to a TCP port.
    EthernetServer listener(tcp_port_);
    listener.begin();
  }
  return true;
}

void ServerConnections::PerformIO() {
  // Find ServerConnections which have been disconnected.
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (conn && conn->has_socket()) {
      if (!SocketIsConnected(conn->sock_num())) {
        // Not connected. Call PerformIO which will detect the missing
        // connection and take the appropriate action.
        conn->PerformIO();

        // The ServerConnection should NOT jump straight from disconnected to
        // connected again.
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

ServerConnection* ServerConnections::GetServerConnection(size_t ndx) {
  return reinterpret_cast<ServerConnection*>(connections_storage_) + ndx;
}

ServerConnection* ServerConnections::GetServerConnectionForSocket(
    int sock_num) {
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (conn && conn->sock_num() == sock_num) {
      return conn;
    }
  }
  return nullptr;
}

bool ServerConnections::AssignServerConnectionToSocket(int sock_num) {
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
