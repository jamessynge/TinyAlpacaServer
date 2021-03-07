#ifndef TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION_H_

// ServerConnection gathers everything we need to deal with a single TCP
// connection from a client. On Arduino, where we have no dynamic memory
// allocation and a fixed maximum number of TCP connections, we pre-allocate
// everything needed to handle one TCP connection.

#include "utils/platform.h"

namespace alpaca {

class ServerConnection {
 public:
  ServerConnection(uint16_t tcp_port, int sock_num);

 private:
  const uint16_t tcp_port_;
  const int sock_num_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION_H_
