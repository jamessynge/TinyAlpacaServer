#ifndef TINY_ALPACA_SERVER_SRC_UTILS_SERVER_CONNECTION_BASE_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_SERVER_CONNECTION_BASE_H_

// ServerConnection is a base class for the server-side handling of a TCP
// connection; it works with EthernetServer, etc. to handle listening on a port,
// accepting a new connection, reading and writing data and closing the
// connection.

#include "utils/connection.h"
#include "utils/logging.h"
#include "utils/platform.h"
#include "utils/platform_ethernet.h"

namespace alpaca {

class ServerConnectionBase {
 public:
  ServerConnectionBase() : sock_num_(-1) {}
  virtual ~ServerConnectionBase();

  // Sets a socket number that has been accepted. Returns true if this
  // connection instance not already in use, in which case sock_num_ has been
  // set to the socket number. If already in use, returns false; this is a
  // programming error, so a DCHECK is will trigger.
  bool set_sock_num(uint8_t sock_num);

  // Reads and decodes requests from, and writes responses to, the socket. Only
  // called when the socket is connected. Calls the subclasses's On<EventName>
  // as appropriate. The current implementation will make at most one of the
  // On<EventName> calls per call to PerformIO. This method is expected to be
  // called from the loop() function of an Arduino sketch.
  void PerformIO();

  // Accessors.
  int sock_num() const { return sock_num_; }
  bool is_connected() const { return sock_num_ >= 0; }

 protected:
  // Called when a new connection from a client is received.
  virtual void OnConnect(EthernetClient& client) = 0;

  // Called when there may be data to read from the client.
  virtual void OnCanRead(EthernetClient& client) = 0;

  // Called when there is no more data to come from the client (i.e. it has
  // half closed the socket). Note that while compliant routers, firewalls,
  // etc., *should* support the TCP connection staying in this half-closed state
  // for a long time (i.e. so that we can stream a response slowly), in practice
  // those middle boxes may interpret a FIN from one peer in the connection as a
  // sign that they connection will soon close, and will wait very little time
  // before effectively breaking the connection.
  virtual void OnClientDone(EthernetClient& client) = 0;

  // Need to figure out if we need OnCanWrite, and if so how to track when
  // writing failed because the TX buffers were full.
  //
  // // Called when there is room to write a new connection is received.
  // virtual void OnCanWrite(EthernetClient& client) = 0;

  // Called when we discover that the connection has been broken.
  // TODO(jamessynge): To avoid calling OnDisconnect unnecessarily, need to come
  // up with a way to detect whether we closed the connection, or the client
  // did. This could be done by creating a sub-class of EthernetClient that
  // records when stop has been called.
  virtual void OnDisconnect() = 0;

 private:
  // If negative, not connected. Starts negative, gets set by set_sock_num, then
  // gets set back to negative when the connection is closed or broken.
  int sock_num_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_SERVER_CONNECTION_BASE_H_
