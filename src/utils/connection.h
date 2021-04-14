#ifndef TINY_ALPACA_SERVER_SRC_UTILS_CONNECTION_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_CONNECTION_H_

// Connection is like Arduino's Client class, but doesn't include the connect
// method; i.e. a Connection is used to represent an existing (TCP) connection,
// not to create one. This is useful for implementing a server where we want to
// work with connections initiated by a remote client.

#include "utils/platform_ethernet.h"

namespace alpaca {

// Extends Arduino's Stream with methods for closing a connection and checking
// whether it has been closed.
class Connection : public Stream {
 public:
  // Close the connection (fully, not half-closed).
  virtual void close() = 0;

  // Returns true if the connection is either readable or writeable.
  virtual bool connected() const = 0;

  // Reads up to 'size' bytes into buf, stopping early if there are no more
  // bytes available to read from the connection. Returns the number of bytes
  // read. The default implementation uses `int Stream::read()` to read one byte
  // at a time.
  virtual size_t read(uint8_t *buf, size_t size);

  using Stream::read;

  // Returns the hardware socket number of this connection. This is exposed
  // primarily to support debugging.
  virtual uint8_t sock_num() const = 0;
};

// An abstract implementation of Connection that delegates to a Client instance
// provided by a subclass. To produce a concrete instance, some subclass will
// also need to implement close() and connected().
class WrappedClientConnection : public Connection {
 public:
  size_t write(uint8_t b) override;
  size_t write(const uint8_t *buf, size_t size) override;
  int available() override;
  int read() override;
  size_t read(uint8_t *buf, size_t size) override;
  int peek() override;
  void flush() override;

 protected:
  virtual Client &client() const = 0;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_CONNECTION_H_
