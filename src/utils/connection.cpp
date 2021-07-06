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
    TAS_DCHECK_LT(c, 256) << TAS_FLASHSTR("c (") << c
                          << TAS_FLASHSTR(") should be in the range [0, 255]");
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
                                                   const size_t size) {
  // NOTE: Avoiding checking for hasWriteError, and leaving that up to
  // a caller. Also avoiding optimizing for long strings, just appending
  // to the write buffer multiple times if necessary, with flushes in
  // between.
  FlushIfFull();
  size_t remaining = size;
  while (remaining > 0) {
    size_t room = write_buffer_limit_ - write_buffer_size_;
    TAS_DCHECK_GT(room, 0);
    if (room > remaining) {
      room = remaining;
    }
    memcpy(write_buffer_ + write_buffer_size_, buf, room);
    write_buffer_size_ += room;
    buf += room;
    remaining -= room;
    FlushIfFull();
  }
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
  if (write_buffer_size_ > 0) {
    TAS_VLOG(2) << TAS_FLASHSTR("write_buffer_size_=") << write_buffer_size_;
    if (!hasWriteError()) {
      TAS_VLOG(2) << TAS_FLASHSTR("hasWriteError=") << false;
      auto cursor = write_buffer_;
      auto remaining = write_buffer_size_;
      while (true) {
        auto wrote = client().write(cursor, remaining);
        TAS_VLOG(2) << TAS_FLASHSTR("wrote=") << wrote;
        TAS_DCHECK_LE(wrote, remaining);
        if (wrote <= 0 || !client().connected()) {
          setWriteError(1);
          break;
        }
        if (wrote >= remaining) {
          break;
        }
        cursor += wrote;
        remaining -= wrote;
      }
    } else {
      TAS_VLOG(2) << TAS_FLASHSTR("hasWriteError=") << true;
    }
    write_buffer_size_ = 0;
    return;
  }
}
void WriteBufferedWrappedClientConnection::FlushIfFull() {
  if (write_buffer_size_ >= write_buffer_limit_) {
    flush();
  }
}

}  // namespace alpaca
