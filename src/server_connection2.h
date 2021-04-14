#ifndef TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION2_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION2_H_

// ServerConnection gathers everything we need to deal with a single TCP
// connection from a client. On Arduino, where we have no dynamic memory
// allocation and a fixed maximum number of TCP connections, we pre-allocate
// everything needed to handle one TCP connection.

#include "alpaca_request.h"
#include "request_decoder.h"
#include "request_listener.h"
#include "utils/platform.h"
#include "utils/socket_listener.h"

namespace alpaca {

class ServerConnection2 : public ServerSocketListener {
 public:
  // enum class State : uint8_t {
  //   kClosed,
  //   kBetweenRequests,
  //   kDecodingRequest,
  //   kDecodedRequest,
  //   kDecodeFailed
  // };
  explicit ServerConnection2(RequestListener& request_listener);

  // Placement new operator. Used to allow us to have a compile time
  // configuration of the number of simultaneous connections that we want to
  // support.
  void* operator new(size_t size, void* ptr) { return ptr; }

  int sock_num() const { return sock_num_; }
  bool has_socket() const { return sock_num_ < MAX_SOCK_NUM; }

  // Methods from ServerSocketListener.
  void OnConnect(Connection& connection) override;
  void OnCanRead(Connection& connection) override;
  void OnHalfClosed(Connection& connection) override;
  void OnDisconnect() override;

 private:
  RequestListener& request_listener_;
  AlpacaRequest request_;
  RequestDecoder request_decoder_;
  uint8_t sock_num_;
  bool between_requests_;
  // State state_;
  uint8_t input_buffer_size_;
  char input_buffer_[32];
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_CONNECTION2_H_
