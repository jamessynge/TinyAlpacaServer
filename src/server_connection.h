#ifndef TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION_H_

// ServerConnection gathers everything we need to deal with a single TCP
// connection from a client, without actually including any of the networking
// classes that need to deal with the platform's networking API. On Arduino,
// where we have no dynamic memory allocation and a fixed maximum number of TCP
// connections, we pre-allocate everything needed to handle one TCP connection.
//
// Author: james.synge@gmail.com

#include <McuCore.h>
#include <McuNet.h>

#include "alpaca_request.h"
#include "config.h"
#include "request_decoder.h"
#include "request_listener.h"

namespace alpaca {

class ServerConnection : public mcunet::ServerSocketListener {
 public:
  explicit ServerConnection(RequestListener& request_listener);

  // The sock_num is set when OnConnect is called, and cleared when either the
  // instance calls close on a connection, or when OnDisconnect is called.
  int sock_num() const { return sock_num_; }
  bool has_socket() const { return sock_num_ < MAX_SOCK_NUM; }

  // Methods from ServerSocketListener.
  void OnConnect(mcunet::Connection& connection) override;
  void OnCanRead(mcunet::Connection& connection) override;
  void OnDisconnect() override;

 private:
  RequestListener& request_listener_;
  AlpacaRequest request_;
  RequestDecoder request_decoder_;
  uint8_t sock_num_;
  bool between_requests_;
  uint8_t input_buffer_size_;
  char input_buffer_[SERVER_CONNECTION_INPUT_BUFFER_SIZE];
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION_H_
