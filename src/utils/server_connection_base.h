#ifndef TINY_ALPACA_SERVER_SRC_UTILS_SERVER_CONNECTION_BASE_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_SERVER_CONNECTION_BASE_H_

// ServerConnection is a base class for the server-side handling of a TCP
// connection; it works with EthernetClass, etc. to handle listening on a port,
// accepting a new connection, reading and writing data and closing the
// connection.

#include "utils/platform.h"
#include "utils/platform_ethernet.h"

namespace alpaca {

class ServerConnectionBase {
 public:
  ServerConnectionBase(int sock_num, uint16_t tcp_port);
  virtual ~ServerConnectionBase();

  // Call once at server startup to configure socket 'sock_num' to listen for
  // new connections to the TCP port number 'tcp_port'.
  void BeginListening();

  // Handles changes in state of the socket (i.e. a new connection from a
  // client, available data to read, room to write, client disconnect). The
  // current implementation will make at most one of the On<Event> calls per
  // call to PerformIO. This method is expected to be called from the loop()
  // function of an Arduino sketch.
  void PerformIO();

 protected:
  // Called when a new connection from a client is received.
  virtual void OnConnect(EthernetClient& client) = 0;

  // Called when there may be data to read from the client.
  virtual void OnCanRead(EthernetClient& client) = 0;

  // Called when there is no more data to come from the client (i.e. it has
  // half closed the socket). Note that compliant routers, firewalls, etc.,
  // should support the TCP connection staying in this half-closed state for a
  // long time (i.e. so that we can stream a response slowly), those middle
  // boxes may interpret a FIN from one peer in the connection as a sign that
  // they connection will soon close, and will wait very little time before
  // effectively breaking the connection.
  virtual void OnClientDone(EthernetClient& client) = 0;

  // Need to figure out if we need OnCanWrite, and if so how to track when
  // writing failed because the TX buffers were full.
  //
  // // Called when there is room to write a new connection is received.
  // virtual void OnCanWrite(EthernetClient& client) = 0;

  // // Called when we discover that the connection has been broken.
  // // TODO(jamessynge): To avoid calling OnDisconnect unnecessarily, need to
  // come
  // // up with a way to detect whether we closed the connection, or the client
  // // did. This could be done by creating a sub-class of EthernetClient that
  // // records when stop has been called.
  // virtual void OnDisconnect(int sock_num) = 0;

 private:
  void DoListen();

  const int sock_num_;
  const uint16_t tcp_port_;

  // At the last PerformIO call, was the socket connected (this includes
  // CLOSE_WAIT with data available to read).
  bool was_connected_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_SERVER_CONNECTION_BASE_H_
