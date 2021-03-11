#ifndef TINY_ALPACA_SERVER_EXTRAS_FUTURES_SOCKET_H_
#define TINY_ALPACA_SERVER_EXTRAS_FUTURES_SOCKET_H_

// Socket and ServerSocket provide a simple API for working with the sockets
// provided by internet offload chips such as the WIZ5500 found on RobotDyn's
// Mega ETH board.
//
// THIS IS SPECULATIVE, NOT IMPLEMENTED OR USED.

#include "extras/futures/socket_listener.h"
#include "utils/platform_ethernet.h"

namespace alpaca {

class ClientSocket {
 public:
  ClientSocket(int sock_num, SocketListener& listener);
  virtual ~ClientSocket();

  // Connect to the specified ip:port, returning true if successful.
  bool connect(IPAddress ip, uint16_t port);

  // To be called from the Arduino loop() function, notifies the listener when
  // there is data to read, or when the peer closes their side of the socket.
  void loop();

 private:
  const int sock_num_;
  bool connected_;
  SocketListener& listener_;
};

class ServerSocket {
 public:
  ServerSocket(int sock_num, uint16_t tcp_port, SocketListener& listener);
  virtual ~ServerSocket();

  // Start listening for connections to 'tcp_port', returns true if successfully
  // configures the underlying socket. If the socket is already being used for
  // another purpose (e.g. there is an existing connection), that is replaced
  // with this new use (i.e. an existing connection is closed or broken).
  bool start_listening();

  // To be called from the Arduino loop() function, notifies the
  // Handles changes in state of the socket (i.e. a new connection from a
  // client, available data to read, room to write, client disconnect). The
  // current implementation will make at most one of the On<Event> calls per
  // call to PerformIO. This method is expected to be called from the loop()
  // function of an Arduino sketch.
  void loop();

 private:
  const int sock_num_;
  bool connected_;
  SocketListener& listener_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_FUTURES_SOCKET_H_
