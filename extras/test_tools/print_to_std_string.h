#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_PRINT_TO_STD_STRING_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_PRINT_TO_STD_STRING_H_

// Helps with testing methods that accept a Print& instance and print/write to
// it.

#include <sstream>

#include "extras/host_arduino/print.h"

namespace alpaca {

class PrintToStdString : public Print {
 public:
  size_t write(uint8_t b) override {
    const uint8_t* buffer = &b;
    return write(buffer, 1);
  }

  size_t write(const uint8_t* buffer, size_t size) override {
    out_.write(reinterpret_cast<const char*>(buffer), size);
    return size;
  }

  // Pull in the other variants of write; otherwise, only the above two are
  // visible.
  using Print::write;

  std::string str() const { return out_.str(); }

  void reset() { out_.str(""); }

 private:
  std::ostringstream out_;
};

template <class T>
std::string PrintValueToStdString(const T& t) {
  PrintToStdString out;
  out.print(t);
  return out.str();
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_PRINT_TO_STD_STRING_H_
