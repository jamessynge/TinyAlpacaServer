#ifndef TINY_ALPACA_SERVER_SRC_UTILS_SOCKET_LISTENER_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_SOCKET_LISTENER_H_

// The APIs for notifying listeners of key socket events.
//
// Author: james.synge@gmail.com

#include "utils/connection.h"
#include "utils/platform.h"

namespace alpaca {

// There is no OnCanWrite because (for now) we don't need it and don't have the
// ability to buffer data and resume writing later when there is room for more.
class SocketListener {
 public:
#if !TAS_EMBEDDED_TARGET
  virtual ~SocketListener() {}
#endif

  // Called when there *may* be data to read from the peer. Currently this is
  // also called by ServerSocket for existing connections.
  virtual void OnCanRead(Connection& connection) = 0;

  // Called when there is no more data to come from the client (i.e. it has half
  // closed its socket), but this end of the connection may still write. This
  // may not be called between OnConnect and OnDisconnect.
  //
  // Note that while compliant routers, firewalls, etc., should support the TCP
  // connection staying in this half-closed state for a long time (i.e. so that
  // we can stream a response slowly), NAT devices may interpret a FIN from one
  // peer in the connection as a sign that they connection will soon close, and
  // will wait very little time before effectively breaking the connection by
  // losing track of the address and port mapping.
  virtual void OnHalfClosed(Connection& connection) = 0;

  // Called when we discover that the connection has been broken by the other
  // party (e.g. when a RST packet is received and the socket status changes to
  // CLOSED without this application having initiated the close).
  virtual void OnDisconnect() = 0;
};

class ServerSocketListener : public SocketListener {
 public:
  // Called when a new connection from a client is received.
  virtual void OnConnect(Connection& connection) = 0;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_SOCKET_LISTENER_H_
