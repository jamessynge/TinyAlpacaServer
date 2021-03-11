#ifndef TINY_ALPACA_SERVER_EXTRAS_ETHERNET3_ETHERNET_CLIENT_H_
#define TINY_ALPACA_SERVER_EXTRAS_ETHERNET3_ETHERNET_CLIENT_H_

// Just enough of EthernetClient for Tiny Alpaca Server to compile on host,
// maybe to be a TCP server.

#include "extras/host_arduino/client.h"

class EthernetClient : public Client {
 public:
  explicit EthernetClient(uint8_t sock);

  int connect(IPAddress ip, uint16_t port) override { return 0; }
  int connect(const char *host, uint16_t port) override { return 0; }

  // Returns the status of the socket, from the Socket n Status Register.
  uint8_t status();

  // Write a byte to the stream, returns the number written.
  size_t write(uint8_t) override;

  // Write 'size' bytes from the buffer to the stream, returns the number
  // written. Note that Ethernet3 takes a blocking approach, looping until there
  // are 'size' bytes in the TX buffers available (with 'size' capped to the
  // maximum send size allowed).
  size_t write(const uint8_t *buf, size_t size) override;

  // Returns the number of bytes available for reading. It may not be possible
  // to read that many bytes in one call to read.
  int available() override;

  // Read one byte from the stream.
  int read() override;

  // Read up to 'size' bytes from the stream, returns the number read.
  int read(uint8_t *buf, size_t size) override;

  // Returns the next available byte/
  int peek() override;

  void flush() override;
  void stop() override;
  uint8_t connected() override;

  explicit operator bool() override { return connected() != 0; }

  uint8_t getSocketNumber();

 private:
  uint8_t sock_;
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_ETHERNET3_ETHERNET_CLIENT_H_
