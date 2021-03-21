// TODO(jamessynge): Describe why this file exists/what it provides.

#include "extras/host/ethernet3/ethernet_client.h"

#include "extras/host/ethernet3/ethernet_config.h"
#include "extras/host/ethernet3/host_sockets.h"
#include "extras/host/ethernet3/w5500.h"
// #include
// "experimental/users/jamessynge/tiny-alpaca-server/src/utils/platform.h"

EthernetClient::EthernetClient(uint8_t sock) : sock_(sock) {}

int EthernetClient::connect(IPAddress ip, uint16_t port) { return 0; }
int EthernetClient::connect(const char *host, uint16_t port) { return 0; }

// Returns the status of the socket, from the Socket n Status Register.
uint8_t EthernetClient::status() {
  return alpaca::HostSockets::SocketStatus(sock_);
}

// Write a byte to the stream, returns the number written.
size_t EthernetClient::write(uint8_t) { return 0; }

// Write 'size' bytes from the buffer to the stream, returns the number
// written. Note that Ethernet3 takes a blocking approach, looping until there
// are 'size' bytes in the TX buffers available (with 'size' capped to the
// maximum send size allowed).
size_t EthernetClient::write(const uint8_t *buf, size_t size) { return 0; }

// Returns the number of bytes available for reading. It may not be possible
// to read that many bytes in one call to read.
int EthernetClient::available() {
  return alpaca::HostSockets::AvailableBytes(sock_);
}

// Read one byte from the stream.
int EthernetClient::read() { return 0; }

// Read up to 'size' bytes from the stream, returns the number read.
int EthernetClient::read(uint8_t *buf, size_t size) { return 0; }

// Returns the next available byte/
int EthernetClient::peek() { return 0; }

void EthernetClient::flush() {}
void EthernetClient::stop() {}
uint8_t EthernetClient::connected() {
  return status() == SnSR::ESTABLISHED || status() == SnSR::CLOSE_WAIT;
}

EthernetClient::operator bool() { return connected() != 0; }

uint8_t EthernetClient::getSocketNumber() { return sock_; }
