#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_CLIENT_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_CLIENT_H_

// Represents a stream connected to a server.

#include "extras/host_arduino/ip_address.h"
#include "extras/host_arduino/stream.h"

class Client : public Stream {
 public:
  virtual int connect(IPAddress ip, uint16_t port) = 0;
  virtual int connect(const char *host, uint16_t port) = 0;
  using Stream::read;
  virtual int read(uint8_t *buf, size_t size) = 0;
  virtual void flush() = 0;
  virtual void stop() = 0;
  virtual uint8_t connected() = 0;
  virtual operator bool() = 0;
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_CLIENT_H_
