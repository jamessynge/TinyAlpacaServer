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
    TAS_DCHECK_LT(c, 256) << FLASHSTR("c (") << c
                          << FLASHSTR(") should be in the range [0, 255]");
    *buf++ = c & 0xff;
    ++result;
    --size;
  }
  return result;
}

bool Connection::hasWriteError() {
  return getWriteError() != 0 || !connected();
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

WriteBufferedWrappedClientConnection::WriteBufferedWrappedClientConnection(
    uint8_t *write_buffer, uint8_t write_buffer_limit)
    : write_buffer_(write_buffer),
      write_buffer_limit_(write_buffer_limit),
      write_buffer_size_(0) {
  TAS_DCHECK(write_buffer != nullptr);
  TAS_DCHECK(write_buffer_limit > 0);
}
size_t WriteBufferedWrappedClientConnection::write(uint8_t b) {
  if (write_buffer_size_ >= write_buffer_limit_) {
    flush();
  }
  write_buffer_[write_buffer_size_++] = b;
  return 1;
}
size_t WriteBufferedWrappedClientConnection::write(const uint8_t *buf,
                                                   size_t size) {
  if (size == 0 || hasWriteError()) {
    return 0;
  }
  size_t room = write_buffer_limit_ - write_buffer_size_;
  if (size > room) {
    flush();
    if (hasWriteError()) {
      return 0;
    }
    auto wrote = client().write(buf, size);
    if (wrote < size || !client().connected() ||
        client().getWriteError() != 0) {
      setWriteError(1);
      return 0;
    }
    return wrote;
  }
  memcpy(write_buffer_ + write_buffer_size_, buf, size);
  write_buffer_size_ += size;
  return size;
}
int WriteBufferedWrappedClientConnection::available() {
  return client().available();
}
int WriteBufferedWrappedClientConnection::read() { return client().read(); }
size_t WriteBufferedWrappedClientConnection::read(uint8_t *buf, size_t size) {
  int result = client().read(buf, size);
  if (result >= 0) {
    return result;
  } else {
    return 0;
  }
}
int WriteBufferedWrappedClientConnection::peek() { return client().peek(); }
void WriteBufferedWrappedClientConnection::flush() {
  if (write_buffer_size_ > 0 && !hasWriteError()) {
    auto cursor = write_buffer_;
    auto remaining = write_buffer_size_;
    while (true) {
      auto wrote = client().write(cursor, remaining);
      TAS_DCHECK_LE(wrote, remaining);
      if (wrote <= 0 || !client().connected()) {
        setWriteError(1);
        return;
      }
      if (wrote == remaining) {
        write_buffer_size_ = 0;
        return;
      }
      cursor += wrote;
      remaining -= wrote;
    }
  }
}

}  // namespace alpaca
