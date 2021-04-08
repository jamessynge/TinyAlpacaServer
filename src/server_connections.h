#ifndef TINY_ALPACA_SERVER_SRC_SERVER_CONNECTIONS_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_CONNECTIONS_H_

// ServerConnections owns the set of ServerConnection objects used to implement
// the HTTP server feature of Tiny Alpaca Server.

#include "request_listener.h"
#include "server_connection.h"
#include "utils/platform.h"

namespace alpaca {

class ServerConnections {
 public:
  // request_listener is the object to be notified of decoding of HTTP requests
  // received on the tcp_port.
  ServerConnections(RequestListener& request_listener, uint16_t tcp_port);

  // Prepares the ServerConnection instances to receive TCP connections. Returns
  // true if able to do so, false otherwise.
  bool Initialize();

  // Performs network IO as appropriate.
  void PerformIO();

 private:
  // Not using all of the ports, need to reserve one for the Alpaca Discovery
  // protocols, one for DHCP renewal, and maybe one for outbound connections to
  // a time server.
  static constexpr size_t kNumServerConnections = 4;
  using ServerConnectionsArray = ServerConnection[kNumServerConnections];
  static constexpr size_t kServerConnectionsStorage =
      sizeof(ServerConnectionsArray);

  // Returns a pointer to the ServerConnection with index 'ndx', where 'ndx' is
  // in the range [0, kNumServerConnections-1].
  ServerConnection* GetServerConnection(size_t ndx);

  // Returns a pointer to the ServerConnection assigned to socket 'sock_num'.
  ServerConnection* GetServerConnectionForSocket(int sock_num);

  // Finds a ServerConnection that isn't in use and assigns it to the specified
  // socket.
  bool AssignServerConnectionToSocket(int sock_num);

  uint16_t tcp_port_;
  alignas(ServerConnection) uint8_t
      connections_storage_[kServerConnectionsStorage];
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_CONNECTIONS_H_
