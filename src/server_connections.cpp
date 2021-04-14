#include "server_connections.h"

#include "utils/o_print_stream.h"
#include "utils/platform.h"

namespace alpaca {

namespace {
uint8_t SocketStatus(int sock_num) {
  EthernetClient client(sock_num);
  return client.status();
}
bool SocketIsConnected(int sock_num) {
  auto status = SocketStatus(sock_num);
  return status == SnSR::ESTABLISHED || status == SnSR::CLOSE_WAIT;
}
}  // namespace

ServerConnections::ServerConnections(RequestListener& request_listener,
                                     uint16_t tcp_port)
    : tcp_port_(tcp_port) {
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    new (GetServerConnection(ndx)) ServerConnection(request_listener);
  }
  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    last_socket_status_[sock_num] = SnSR::CLOSED;
  }
}

bool ServerConnections::Initialize() {
  TAS_VLOG(2) << TASLIT(
      "ServerConnections::Initialize, _server_ports at start:");
  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    TAS_VLOG(2) << '[' << sock_num << TASLIT("] port ")
                << EthernetClass::_server_port[sock_num] << ", status 0x"
                << BaseHex << SocketStatus(sock_num);
  }

  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    // EthernetServer::begin() finds a closed hardware socket and puts it to use
    // listening for connections to a TCP port.
    EthernetServer listener(tcp_port_);
    listener.begin();
    // if (!PlatformEthernet::InitializeTcpListenerSocket(ndx + 1, tcp_port_)) {
    //   TAS_DCHECK(false)
    //       << TASLIT("InitializeTcpListenerSocket failed for hardware socket
    //       ")
    //       << ndx + 1;
    // }
  }

  TAS_VLOG(2) << TASLIT("ServerConnections::Initialize, _server_ports:");
  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    TAS_VLOG(2) << '[' << sock_num << TASLIT("] port ")
                << EthernetClass::_server_port[sock_num] << ", status 0x"
                << BaseHex << SocketStatus(sock_num);
  }
  return true;
}

void ServerConnections::PerformIO() {
  TAS_VLOG(4) << TASLIT("ServerConnections::PerformIO entry");

  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    auto status = SocketStatus(sock_num);
    if (last_socket_status_[sock_num] != status) {
      TAS_VLOG(2) << '[' << sock_num << TASLIT("] status changed from 0x")
                  << BaseHex << last_socket_status_[sock_num] << " to 0x"
                  << status;
      last_socket_status_[sock_num] = status;
    }
  }

  // Find ServerConnections which have been disconnected.
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (conn && conn->has_socket()) {
      if (!SocketIsConnected(conn->sock_num())) {
        // Not connected. Call PerformIO which will detect the missing
        // connection and take the appropriate action.

        TAS_VLOG(2) << TASLIT("ServerConnections::PerformIO ServerConnection[")
                    << ndx << TASLIT("] no longer connected; sock_num was ")
                    << conn->sock_num();
        last_socket_status_[conn->sock_num()] = SocketStatus(conn->sock_num());

        conn->PerformIO();

        // The ServerConnection should NOT jump straight from disconnected to
        // connected again.
        TAS_DCHECK(!conn->has_socket()) << conn->sock_num();
      }
    }
  }

  // Find new TCP connections and assign ServerConnections to them, and find
  // recently closed sockets that should resume listening.
  TAS_VLOG(7) << TASLIT("Finding sockets whose connection state has changed");
  int num_sockets_configured = 0;
  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    if (EthernetClass::_server_port[sock_num] != tcp_port_) {
      // Not related to this server.
      continue;
    }
    ++num_sockets_configured;
    if (SocketIsConnected(sock_num)) {
      if (GetServerConnectionForSocket(sock_num) == nullptr) {
        // New connection.
        TAS_VLOG(2) << TASLIT("ServerConnections::PerformIO socket ")
                    << sock_num << TASLIT(" has a new connection ");
        AssignServerConnectionToSocket(sock_num);
      }
    } else if (PlatformEthernet::SocketIsClosed(sock_num)) {
      // Resume listening.

      // THIS ISN'T REACHED BECAUSE EthernetClient::stop resets
      // _server_port[sock_num] to zero when closing the connection.

      if (!PlatformEthernet::InitializeTcpListenerSocket(sock_num, tcp_port_)) {
        TAS_DCHECK(false)
            << TASLIT("InitializeTcpListenerSocket failed for hardware socket ")
            << sock_num;
      }
    } else {
      uint8_t status = SocketStatus(sock_num);
      switch (status) {
        case SnSR::LISTEN:
          break;
        case SnSR::SYNRECV:
        case SnSR::CLOSING:
        case SnSR::FIN_WAIT:
        case SnSR::LAST_ACK:
        case SnSR::TIME_WAIT:
          TAS_VLOG(2) << TASLIT("ServerConnections::PerformIO socket ")
                      << sock_num << TASLIT(" status is transient: 0x")
                      << BaseHex << status;
          break;
        default:
          TAS_VLOG(2) << TASLIT("ServerConnections::PerformIO socket ")
                      << sock_num << TASLIT(" status is unexpected: 0x")
                      << BaseHex << status;
      }
    }
  }

  while (num_sockets_configured++ < kNumServerConnections) {
    // EthernetServer::begin() finds a closed hardware socket and puts it to use
    // listening for connections to a TCP port.
    EthernetServer listener(tcp_port_);
    listener.begin();
    TAS_VLOG(2) << TASLIT("Started another listener.");
  }

  // For all the ServerConnections with TCP connections to clients, perform IO.
  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (conn && conn->has_socket()) {
      conn->PerformIO();
    }
  }

  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    auto status = SocketStatus(sock_num);
    if (last_socket_status_[sock_num] != status) {
      TAS_VLOG(2) << '[' << sock_num << TASLIT("] status changed from 0x")
                  << BaseHex << last_socket_status_[sock_num] << " to 0x"
                  << status;
      last_socket_status_[sock_num] = status;
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
  TAS_VLOG(1) << TASLIT("No unused ServerConnection for socket ") << sock_num;
  return false;
}

}  // namespace alpaca
