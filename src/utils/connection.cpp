#include "utils/connection.h"

#include "utils/logging.h"

namespace alpaca {

size_t Connection::read(uint8_t *buf, size_t size) {
  size_t result = 0;
  while (size > 0) {
    int c = read();
    if (c < 0) {
      break;
    }
    TAS_DCHECK_LT(c, 256) << "c (" << c << ") should be in the range [0, 255]";
    *buf++ = c & 0xff;
    ++result;
    --size;
  }
  return result;
}

size_t WrappedClientConnection::write(uint8_t b) { return client().write(b); }
size_t WrappedClientConnection::write(const uint8_t *buf, size_t size) {
  return client().write(buf, size);
}
int WrappedClientConnection::available() { return client().available(); }
int WrappedClientConnection::read() { return client().read(); }
size_t WrappedClientConnection::read(uint8_t *buf, size_t size) {
  int result = client().read(buf, size);
  if (result >= 0) {
    return result;
  } else {
    return 0;
  }
}
int WrappedClientConnection::peek() { return client().peek(); }
void WrappedClientConnection::flush() { return client().flush(); }

}  // namespace alpaca
