#ifndef TINY_ALPACA_SERVER_SRC_SERVER_SOCKETS_AND_CONNECTIONS_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_SOCKETS_AND_CONNECTIONS_H_

// ServerSocketsAndConnections owns the set of ServerConnection objects used to
// implement the HTTP server feature of Tiny Alpaca Server.

#include "request_listener.h"
#include "server_socket_and_connection.h"
#include "utils/platform.h"

namespace alpaca {

class ServerSocketsAndConnections {
 public:
  // request_listener is the object to be notified of decoding of HTTP requests
  // received on the tcp_port.
  ServerSocketsAndConnections(uint16_t tcp_port,
                              RequestListener& request_listener);

  // Prepares the ServerSocketAndConnection instances to receive TCP
  // connections. Returns true if able to do so, false otherwise.
  bool Initialize();

  // Performs network IO as appropriate.
  void PerformIO();

 private:
  // Not using all of the ports, need to reserve one for the Alpaca Discovery
  // protocols, one for DHCP renewal, and maybe one for outbound connections to
  // a time server.
  static constexpr size_t kNumSockets = TAS_NUM_SERVER_CONNECTIONS;

  using ServerSocketAndConnectionArray = ServerSocketAndConnection[kNumSockets];
  static constexpr size_t kServerSocketAndConnectionStorage =
      sizeof(ServerSocketAndConnectionArray);

  // Returns a pointer to the ServerSocketAndConnection with index 'ndx', where
  // 'ndx' is in the range [0, kNumSockets-1].
  ServerSocketAndConnection* GetServerSocketAndConnection(size_t ndx);

  alignas(ServerSocketAndConnection) uint8_t
      sockets_storage_[kServerSocketAndConnectionStorage];
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_SOCKETS_AND_CONNECTIONS_H_
