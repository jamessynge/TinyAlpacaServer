#include "server_connections.h"

#include "utils/o_print_stream.h"
#include "utils/platform.h"

namespace alpaca {

ServerConnections::ServerConnections(RequestListener& request_listener,
                                     uint16_t tcp_port)
    : tcp_port_(tcp_port) {
  static_assert(0 < kNumServerConnections, "Too few server connections");
  static_assert(kNumServerConnections < MAX_SOCK_NUM,
                "Too many server connections");

  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    new (GetServerConnection(ndx)) ServerConnection(request_listener);
  }

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
                << BaseHex << PlatformEthernet::SocketStatus(sock_num);
  }

  for (size_t ndx = 0; ndx < kNumServerConnections; ++ndx) {
    // EthernetServer::begin() finds a closed hardware socket and puts it to use
    // listening for connections to a TCP port.
    EthernetServer listener(tcp_port_);
    listener.begin();
  }

  TAS_VLOG(2) << TASLIT("ServerConnections::Initialize, _server_ports:");
  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    TAS_VLOG(2) << '[' << sock_num << TASLIT("] port ")
                << EthernetClass::_server_port[sock_num] << ", status 0x"
                << BaseHex << PlatformEthernet::SocketStatus(sock_num);
  }
  return true;
}

void ServerConnections::PerformIO() {
  TAS_VLOG(4) << TASLIT("ServerConnections::PerformIO entry");

  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    auto status = PlatformEthernet::SocketStatus(sock_num);
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
      if (!PlatformEthernet::SocketIsConnected(conn->sock_num())) {
        // Not connected. Call PerformIO which will detect the missing
        // connection and take the appropriate action.

        TAS_VLOG(2) << TASLIT("ServerConnections::PerformIO ServerConnection[")
                    << ndx << TASLIT("] no longer connected; sock_num was ")
                    << conn->sock_num();
        last_socket_status_[conn->sock_num()] =
            PlatformEthernet::SocketStatus(conn->sock_num());

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
  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    if (EthernetClass::_server_port[sock_num] != tcp_port_) {
      // Not related to this server.
      continue;
    } else if (PlatformEthernet::SocketIsConnected(sock_num)) {
      if (GetServerConnectionForSocket(sock_num) == nullptr) {
        // New connection.
        TAS_VLOG(2) << TASLIT("ServerConnections::PerformIO socket ")
                    << sock_num << TASLIT(" has a new connection ");
        AssignServerConnectionToSocket(sock_num);
      }
    } else if (PlatformEthernet::SocketIsClosed(sock_num)) {
      // Resume listening.
      if (!PlatformEthernet::InitializeTcpListenerSocket(sock_num, tcp_port_)) {
        TAS_DCHECK(false)
            << TASLIT("InitializeTcpListenerSocket failed for hardware socket ")
            << sock_num;
      }
    } else {
      uint8_t status = PlatformEthernet::SocketStatus(sock_num);
      switch (status) {
        case SnSR::CLOSING:
        case SnSR::LISTEN:
        case SnSR::TIME_WAIT:
        case SnSR::FIN_WAIT:
          break;
        // case SnSR::LAST_ACK:
        // case SnSR::SYNRECV:
        // case SnSR::UDP:
        // case SnSR::IPRAW:
        // case SnSR::MACRAW:
        // case SnSR::PPPOE:
        default:
          TAS_VLOG(2) << TASLIT("ServerConnections::PerformIO socket ")
                      << sock_num << TASLIT(" status is unexpected: 0x")
                      << BaseHex << status;
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

  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    auto status = PlatformEthernet::SocketStatus(sock_num);
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
