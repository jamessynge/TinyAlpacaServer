#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_SERIAL_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_SERIAL_H_

// Provides a fake version of the Serial instance that is used in some of these
// demo sketches. There is no attempt to support input; it just outputs to
// std::cout.

#include "extras/host_arduino/print.h"  // IWYU pragma: export

class FakeSerial : public Print {
 public:
  void begin(int) {}
  operator bool() const {  // NOLINT
    return true;
  }

  size_t write(uint8_t b) override;
  size_t write(const uint8_t* buffer, size_t size) override;

  // Pull in the other variants of write; otherwise, only the above two are
  // visible.
  using Print::write;
};

extern FakeSerial Serial;  // NOLINT

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_SERIAL_H_
