// TODO(jamessynge): Describe why this file exists/what it provides.

#include "utils/connection.h"

#include "utils/platform.h"

namespace alpaca {
namespace internal {
ClientConnection::ClientConnection(Client &client) : client_(client) {}

size_t ClientConnection::write(uint8_t b) { return client_.write(b); }
size_t ClientConnection::write(const uint8_t *buf, size_t size) {
  return client_.write(buf, size);
}
int ClientConnection::available() { return client_.available(); }
int ClientConnection::read() { return client_.read(); }
int ClientConnection::read(uint8_t *buf, size_t size) {
  return client_.read(buf, size);
}
int ClientConnection::peek() { return client_.peek(); }
void ClientConnection::flush() { return client_.flush(); }
void ClientConnection::stop() { return client_.stop(); }
uint8_t ClientConnection::connected() { return client_.connected(); }

}  // namespace internal

}  // namespace alpaca
