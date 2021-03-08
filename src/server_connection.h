#ifndef TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION_H_

// ServerConnection gathers everything we need to deal with a single TCP
// connection from a client. On Arduino, where we have no dynamic memory
// allocation and a fixed maximum number of TCP connections, we pre-allocate
// everything needed to handle one TCP connection.

#include "alpaca_request.h"
#include "utils/platform.h"
#include "utils/server_connection_base.h"

namespace alpaca {

class ServerConnection : public ServerConnectionBase {
 public:
  ServerConnection(int sock_num, uint16_t tcp_port);

  // Methods from base class. These are public here to allow for testing.
  void OnConnect(EthernetClient& client) override;
  void OnCanRead(EthernetClient& client) override;
  void OnClientDone(EthernetClient& client) override;

 private:
  const int sock_num_;
  const uint16_t tcp_port_;
  AlpacaRequest request_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION_H_
