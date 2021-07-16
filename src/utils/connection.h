#ifndef TINY_ALPACA_SERVER_SRC_UTILS_CONNECTION_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_CONNECTION_H_

// Connection is like Arduino's Client class, but doesn't include the connect
// method; i.e. a Connection is used to represent an existing (TCP) connection,
// not to create one. This is useful for implementing a server where we want to
// work with connections initiated by a remote client.
//
// Author: james.synge@gmail.com

#include "utils/platform_ethernet.h"

namespace alpaca {

// Extends Arduino's Stream with methods for closing a connection and checking
// whether it has been closed.
class Connection : public Stream {
 public:
  // Returns the number of readable bytes. If not connected, returns -1.
  int available() override = 0;

  // Close the connection (fully, not half-closed).
  virtual void close() = 0;

  // Returns true if the connection is either readable or writeable.
  virtual bool connected() const = 0;

  // Returns true if the peer (e.g. client of the server) half-closed its
  // connection for writing (e.g. by calling shutdown(fd, SHUT_WR), but is
  // waiting for us to write. This is apparently now an unlikely state because
  // clients won't typically close until they've read the full response. See
  // https://www.excentis.com/blog/tcp-half-close-cool-feature-now-broken.
  virtual bool peer_half_closed() const = 0;

  // Reads up to 'size' bytes into buf, stopping early if there are no more
  // bytes available to read from the connection. Returns the number of bytes
  // read. The default implementation uses `int Stream::read()` to read one byte
  // at a time.
  virtual size_t read(uint8_t *buf, size_t size);

  using Stream::read;

  // Returns the hardware socket number of this connection. This is exposed
  // primarily to support debugging.
  virtual uint8_t sock_num() const = 0;

  // Returns true if there is a write error or if the connection is broken.
  virtual bool hasWriteError();
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

// An abstract implementation of Connection that delegates to a Client instance
// provided by a subclass. To produce a concrete instance, some subclass will
// also need to implement close() and connected().
class WriteBufferedWrappedClientConnection : public Connection {
 public:
  WriteBufferedWrappedClientConnection(uint8_t *write_buffer,
                                       uint8_t write_buffer_limit);

  size_t write(uint8_t b) override;
  size_t write(const uint8_t *buf, size_t size) override;
  int available() override;
  int read() override;
  size_t read(uint8_t *buf, size_t size) override;
  int peek() override;
  void flush() override;

 protected:
  virtual Client &client() const = 0;
  uint8_t write_buffer_size() const { return write_buffer_size_; }

 private:
  void FlushIfFull();

  uint8_t *const write_buffer_;
  const uint8_t write_buffer_limit_;
  uint8_t write_buffer_size_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_CONNECTION_H_
