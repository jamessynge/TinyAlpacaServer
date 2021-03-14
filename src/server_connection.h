#ifndef TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION_H_

// ServerConnection gathers everything we need to deal with a single TCP
// connection from a client. On Arduino, where we have no dynamic memory
// allocation and a fixed maximum number of TCP connections, we pre-allocate
// everything needed to handle one TCP connection.

#include "alpaca_request.h"
#include "request_decoder.h"
#include "request_listener.h"
#include "utils/platform.h"
#include "utils/server_connection_base.h"

namespace alpaca {

class ServerConnection : public ServerConnectionBase {
 public:
  ServerConnection(int sock_num, uint16_t tcp_port,
                   RequestListener& request_listener);

  // Methods from base class. These are public here to allow for testing.
  void OnConnect(EthernetClient& client) override;
  void OnCanRead(EthernetClient& client) override;
  void OnClientDone(EthernetClient& client) override;

  // Placement new operator.
  void* operator new(size_t size, void* ptr) { return ptr; }

 private:
  RequestListener& request_listener_;
  AlpacaRequest request_;
  RequestDecoder request_decoder_;
  uint8_t input_buffer_size_;
  char input_buffer_[32];
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION_H_
