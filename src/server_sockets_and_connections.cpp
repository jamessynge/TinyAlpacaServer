#include "server_sockets_and_connections.h"

#include "utils/platform.h"

namespace alpaca {

ServerSocketsAndConnections::ServerSocketsAndConnections(
    uint16_t tcp_port, RequestListener& request_listener) {
  static_assert(0 < kNumSockets, "Too few server connections");
  static_assert(kNumSockets < MAX_SOCK_NUM, "Too many server connections");

  for (size_t ndx = 0; ndx < kNumSockets; ++ndx) {
    new (GetServerSocketAndConnection(ndx))
        ServerSocketAndConnection(tcp_port, request_listener);
  }
}

bool ServerSocketsAndConnections::Initialize() {
  TAS_VLOG(2) << FLASHSTR("ServerSocketsAndConnections::Initialize");
  uint8_t count = 0;
  for (size_t ndx = 0; ndx < kNumSockets; ++ndx) {
    if (GetServerSocketAndConnection(ndx)->Initialize()) {
      ++count;
    }
  }
  TAS_VLOG(2) << FLASHSTR("Initialized ") << count << FLASHSTR(" of ")
              << kNumSockets << FLASHSTR(" ServerSocketAndConnection objects");
  return count != kNumSockets;
}

void ServerSocketsAndConnections::PerformIO() {
  TAS_VLOG(6) << FLASHSTR("ServerSocketsAndConnections::PerformIO entry");
  for (size_t ndx = 0; ndx < kNumSockets; ++ndx) {
    GetServerSocketAndConnection(ndx)->PerformIO();
  }
  TAS_VLOG(6) << FLASHSTR("ServerSocketsAndConnections::PerformIO exit");
}

ServerSocketAndConnection*
ServerSocketsAndConnections::GetServerSocketAndConnection(size_t ndx) {
  return reinterpret_cast<ServerSocketAndConnection*>(sockets_storage_) + ndx;
}

}  // namespace alpaca
