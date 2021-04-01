#ifndef TINY_ALPACA_SERVER_EXTRAS_FUTURES_SOCKET_LISTENER_H_
#define TINY_ALPACA_SERVER_EXTRAS_FUTURES_SOCKET_LISTENER_H_

// An exploration of how we might expose networking socket events to a listener.

#include "utils/platform.h"
#include "utils/platform_ethernet.h"

namespace alpaca {

class SocketListener {
 public:
  virtual ~SocketListener();

  // Called when there may be data to read from the peer.
  virtual void OnCanRead(EthernetClient& client) = 0;

  // Called when there is no more data to come from the client (i.e. it has
  // half closed the socket). Note that compliant routers, firewalls, etc.,
  // should support the TCP connection staying in this half-closed state for a
  // long time (i.e. so that we can stream a response slowly), those middle
  // boxes may interpret a FIN from one peer in the connection as a sign that
  // they connection will soon close, and will wait very little time before
  // effectively breaking the connection.
  virtual void OnHalfClosed(EthernetClient& client) = 0;

  // Need to figure out if we need OnCanWrite, and if so how to track when
  // writing failed because the TX buffers were full.
  //
  // // Called when there is room to write a new connection is received.
  // virtual void OnCanWrite(EthernetClient& client) = 0;
};

class ServerSocketListener : public SocketListener {
 public:
  // Called when a new connection from a client is received.
  virtual void OnConnect(EthernetClient& client) = 0;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_FUTURES_SOCKET_LISTENER_H_
