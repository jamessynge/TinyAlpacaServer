#ifndef TINY_ALPACA_SERVER_SRC_SERVER_SOCKET_AND_CONNECTION_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_SOCKET_AND_CONNECTION_H_

// Combines a ServerSocket that binds a hardware socket to a TCP port and a
// ServerConnection that listens to events from the socket.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "experimental/users/jamessynge/arduino/mcunet/src/server_socket.h"
#include "request_listener.h"
#include "server_connection.h"

namespace alpaca {

class ServerSocketAndConnection {
 public:
  ServerSocketAndConnection(uint16_t tcp_port,
                            RequestListener& request_listener);

  // Placement new operator. Used to allow us to have a compile time
  // configuration of the number of simultaneous connections that we want to
  // support, i.e. the number of hardware sockets dedicated to serving tcp_port.
  void* operator new(size_t size, void* ptr) { return ptr; }

  // Prepares the instance to receive TCP connections. Returns true if able to
  // do so, false otherwise.
  bool Initialize();

  // Performs network IO as appropriate.
  void PerformIO();

 private:
  ServerConnection server_connection_;
  mcunet::ServerSocket server_socket_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_SOCKET_AND_CONNECTION_H_
