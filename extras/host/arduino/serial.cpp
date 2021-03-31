#include "extras/host/arduino/serial.h"

#include <iostream>
#include <string_view>

#include "absl/strings/match.h"

FakeSerial::FakeSerial() : out_(std::cout) {}
FakeSerial::FakeSerial(bool) : out_(std::cerr) {}

size_t FakeSerial::write(uint8_t b) {
  out_.put(static_cast<char>(b));
  return 1;
}

size_t FakeSerial::write(const uint8_t* buffer, size_t size) {
  out_.write(reinterpret_cast<const char*>(buffer), size);
  return size;
}

void FakeSerial::flush() { out_.flush(); }

FakeSerial Serial;          // NOLINT
FakeSerial ToStdErr(true);  // NOLINT
