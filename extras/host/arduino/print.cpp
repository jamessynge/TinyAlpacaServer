#include "extras/host/arduino/print.h"

#include <stdint.h>

#include <bitset>
#include <cstdio>
#include <cstring>
#include <limits>
#include <ostream>
#include <sstream>
#include <type_traits>

#include "absl/strings/ascii.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "logging.h"

namespace {

template <typename T>
size_t printIntegerWithBaseTo(T value, const int base, Print& out) {
  if (base == 0) {
    return out.write(value);
  }
  if (base == 10 || base < 2 || base > 36) {
    auto s = std::to_string(value);
    return out.write(s.data(), s.size());
  }

  using UT = typename std::make_unsigned<T>::type;
  UT v2 = static_cast<UT>(value);

  // Room for base 2, i.e. 8 binary digits + a trailing NUL.
  char buffer[1 + 8 * sizeof v2];
  char* p = buffer + sizeof buffer;
  *(--p) = '\0';  // NUL terminate the string.
  do {
    DCHECK_GT(p, buffer);
    uint8_t low_digit = v2 % base;
    v2 /= base;
    if (low_digit < 10) {
      *(--p) = '0' + low_digit;
    } else {
      *(--p) = 'A' + (low_digit - 10);
    }
  } while (v2 != 0);
  DCHECK_GT(strlen(p), 0) << "\nvalue=" << value << ", base=" << base;
  return out.write(p);
}

}  // namespace

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

size_t Print::print(const __FlashStringHelper* str) {
  // Assuming here that PROGMEM is meaningless on the host (i.e. that I haven't
  // implemented some fancy emulation).
  return write(reinterpret_cast<const char*>(str));
}

size_t print(const __FlashStringHelper* str);

size_t Print::print(const char str[]) { return write(str); }

size_t Print::print(char c) { return write(c); }

size_t Print::print(unsigned char value, int base) {
  return printIntegerWithBaseTo(value, base, *this);
}

size_t Print::print(int value, int base) {
  return printIntegerWithBaseTo(value, base, *this);
}

size_t Print::print(unsigned int value, int base) {
  return printIntegerWithBaseTo(value, base, *this);
}

size_t Print::print(short value, int base) {  // NOLINT
  return printIntegerWithBaseTo(value, base, *this);
}

size_t Print::print(unsigned short value, int base) {  // NOLINT
  return printIntegerWithBaseTo(value, base, *this);
}

size_t Print::print(long value, int base) {  // NOLINT
  return printIntegerWithBaseTo(value, base, *this);
}

size_t Print::print(unsigned long value, int base) {  // NOLINT
  return printIntegerWithBaseTo(value, base, *this);
}

size_t Print::print(double value, int digits) {
  return printDouble(value, digits);
}

size_t Print::print(const Printable& value) { return value.printTo(*this); }

size_t Print::println(const __FlashStringHelper* str) {
  size_t count = print(str);
  return count + println();
}

size_t Print::println(const char str[]) {
  size_t count = write(str);
  return count + println();
}

size_t Print::println(char c) {
  size_t count = write(c);
  return count + println();
}

size_t Print::println(unsigned char value, int base) {
  size_t count = printIntegerWithBaseTo(value, base, *this);
  return count + println();
}

size_t Print::println(int value, int base) {
  size_t count = printIntegerWithBaseTo(value, base, *this);
  return count + println();
}

size_t Print::println(unsigned int value, int base) {
  size_t count = printIntegerWithBaseTo(value, base, *this);
  return count + println();
}

size_t Print::println(short value, int base) {  // NOLINT
  size_t count = printIntegerWithBaseTo(value, base, *this);
  return count + println();
}

size_t Print::println(unsigned short value, int base) {  // NOLINT
  size_t count = printIntegerWithBaseTo(value, base, *this);
  return count + println();
}

size_t Print::println(long value, int base) {  // NOLINT
  size_t count = printIntegerWithBaseTo(value, base, *this);
  return count + println();
}

size_t Print::println(unsigned long value, int base) {  // NOLINT
  size_t count = printIntegerWithBaseTo(value, base, *this);
  return count + println();
}

size_t Print::println(double value, int digits) {
  size_t count = printDouble(value, digits);
  return count + println();
}

size_t Print::println(const Printable& value) {
  size_t count = value.printTo(*this);
  return count + println();
}

size_t Print::println() { return write('\n'); }

void Print::flush() {}

int Print::getWriteError() { return write_error_; }

void Print::clearWriteError() { setWriteError(0); }

void Print::setWriteError(int err) { write_error_ = err; }

size_t Print::printDouble(double value, int digits) {
  if (digits > 20) {
    digits = 20;
  } else if (digits <= 0) {
    // Printed without a decimal point or any fractional digits.
    return print(static_cast<long>(std::lround(value)), 10);  // NOLINT
  }

  // Excessive length to avoid worrying about bounds checking.
  char buffer[128];
  int len = std::snprintf(buffer, sizeof buffer, "%#.*f", digits, value);
  CHECK_EQ(buffer[len], '\0');

  return print(buffer);
}

namespace {
class PrintToStdOStream : public Print {
 public:
  explicit PrintToStdOStream(std::ostream& out) : Print(), out_(out) {}

  size_t write(uint8_t b) override {
    out_.write(reinterpret_cast<char*>(&b), 1);
    return 1;
  }

  size_t write(const uint8_t* buffer, size_t size) override {
    out_.write(reinterpret_cast<const char*>(buffer), size);
    return size;
  }

  // // Pull in the other variants of write; otherwise, only the above two are
  // // visible.
  // using Print::write;

 private:
  std::ostream& out_;
};
}  // namespace

std::ostream& operator<<(std::ostream& out, const Printable& printable) {
  PrintToStdOStream adapter(out);
  printable.printTo(adapter);
  return out;
}
