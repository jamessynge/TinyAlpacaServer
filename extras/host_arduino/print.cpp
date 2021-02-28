#include "extras/host_arduino/print.h"

#include <cstring>
#include <ostream>

namespace alpaca {

Printable::~Printable() {}

Print::Print() {}
Print::~Print() {}

size_t Print::write(const char* str) {
  if (str == nullptr) return 0;
  return write(reinterpret_cast<const uint8_t*>(str), strlen(str));
}
size_t Print::write(const char* buffer, size_t size) {
  return write(reinterpret_cast<const uint8_t*>(buffer), size);
}

size_t Print::print(char c) { return write(c); }

size_t Print::print(uint8_t value) { return printInteger(value); }

size_t Print::print(int16_t value) { return printInteger(value); }

size_t Print::print(uint16_t value) { return printInteger(value); }

size_t Print::print(int32_t value) { return printInteger(value); }

size_t Print::print(uint32_t value) { return printInteger(value); }

size_t Print::print(double value) { return printDouble(value); }

size_t Print::print(const Printable& value) { return value.printTo(*this); }

size_t Print::printInteger(int64_t value) {
  auto s = std::to_string(value);
  return write(s.data(), s.size());
}

size_t Print::printDouble(double value) {
  auto s = std::to_string(value);
  return write(s.data(), s.size());
}

class PrintToOStream : public Print {
 public:
  explicit PrintToOStream(std::ostream& out) : Print(), out_(out) {}

  size_t write(uint8_t b) override {
    out_.write(reinterpret_cast<char*>(&b), 1);
    return 1;
  }
  size_t write(const uint8_t* buffer, size_t size) override {
    out_.write(reinterpret_cast<const char*>(buffer), size);
    return size;
  }

  // Pull in the other variants of write; otherwise, only the above two are
  // visible.
  using Print::write;

 private:
  std::ostream& out_;
};

std::ostream& operator<<(std::ostream& out, const Printable& printable) {
  PrintToOStream adapter(out);
  printable.printTo(adapter);
  return out;
}

}  // namespace alpaca
