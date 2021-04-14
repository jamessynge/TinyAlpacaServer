#ifndef TINY_ALPACA_SERVER_SRC_SERVER_SOCKET_AND_CONNECTION_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_SOCKET_AND_CONNECTION_H_

// Combines a ServerSocket that binds a hardware socket to a TCP port and a
// ServerConnection2 that listens to events from the socket.

#include "server_connection2.h"
#include "utils/platform.h"
#include "utils/server_socket.h"

namespace alpaca {

class ServerSocketAndConnection {
 public:
  ServerSocketAndConnection(uint16_t tcp_port,
                            RequestListener& request_listener);

  // Placement new operator.
  void* operator new(size_t size, void* ptr) { return ptr; }

  // Prepares the instance to receive TCP connections. Returns true if able to
  // do so, false otherwise.
  bool Initialize();

  // Performs network IO as appropriate.
  void PerformIO();

 private:
  ServerConnection2 server_connection_;
  ServerSocket server_socket_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_SOCKET_AND_CONNECTION_H_
