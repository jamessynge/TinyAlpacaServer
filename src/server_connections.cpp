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
  TAS_VLOG(2) << TASLIT(
      "ServerConnections::Initialize, _server_ports at start:");
  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    TAS_VLOG(2) << '[' << sock_num << TASLIT("] = ")
                << EthernetClass::_server_port[sock_num];
  }

  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    // EthernetServer::begin() finds a closed hardware socket and puts it to use
    // listening for connections to a TCP port.
    EthernetServer listener(tcp_port_);
    listener.begin();
  }

  TAS_VLOG(2) << TASLIT("ServerConnections::Initialize, _server_ports:");
  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    TAS_VLOG(2) << '[' << sock_num << TASLIT("] = ")
                << EthernetClass::_server_port[sock_num];
  }
  return true;
}

void ServerConnections::PerformIO() {
  TAS_VLOG(4) << TASLIT("ServerConnections::PerformIO entry");
  // Find ServerConnections which have been disconnected.
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (conn && conn->has_socket()) {
      if (!SocketIsConnected(conn->sock_num())) {
        TAS_VLOG(2) << TASLIT("ServerConnections::PerformIO ServerConnection[")
                    << ndx << TASLIT("] no longer connected; sock_num was ")
                    << conn->sock_num();
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
        if (!AssignServerConnectionToSocket(sock_num)) {
          TAS_DCHECK(false)
              << TASLIT(
                     "Unable to assign a ServerConnection to hardware socket ")
              << sock_num
              << TASLIT(
                     ", we have more connected sockets than available "
                     "ServerConnections!");
        }
      }
    } else if (EthernetClass::_server_port[sock_num] == tcp_port_ &&
               PlatformEthernet::SocketIsClosed(sock_num)) {
      // Resume listening.
      if (!PlatformEthernet::InitializeTcpListenerSocket(sock_num, tcp_port_)) {
        TAS_DCHECK(false)
            << TASLIT("InitializeTcpListenerSocket for hardware socket ")
            << sock_num;
      }
    }
  }

  // For all the ServerConnections with TCP connections to clients, perform IO.
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (conn && conn->has_socket()) {
      conn->PerformIO();
    }
  }

  TAS_VLOG(4) << TASLIT("ServerConnections::PerformIO exit");
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
