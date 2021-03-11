#ifndef TINY_ALPACA_SERVER_SRC_UTILS_CONNECTION_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_CONNECTION_H_

// Connection is like Arduino's Client class, but doesn't include the connect
// method; i.e. a Connection is used to represent an existing (TCP) connection,
// not to create one.

#include "utils/platform_ethernet.h"

namespace alpaca {
namespace internal {

class ClientConnection;

}  // namespace internal

class Connection : public Stream {
 public:
  // Create an instance of Connection which forwards method calls to an instance
  // of Client. 'client' must outlive the returned Connection (specifically, it
  // must exist as long as calls are being made to the methods).
  static internal::ClientConnection Wrap(Client &client);

  // Write a byte.
  int read() override = 0;
  virtual int read(uint8_t *buf, size_t size) = 0;
  virtual void flush() = 0;
  virtual void stop() = 0;
  virtual uint8_t connected() = 0;
};

namespace internal {

class ClientConnection : public Connection {
 public:
  explicit ClientConnection(Client &client);
  size_t write(uint8_t b) override;
  size_t write(const uint8_t *buf, size_t size) override;
  int available() override;
  int read() override;
  int read(uint8_t *buf, size_t size) override;
  int peek() override;
  void flush() override;
  void stop() override;
  uint8_t connected() override;

 private:
  Client &client_;
};

}  // namespace internal

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_CONNECTION_H_
