#ifndef TINY_ALPACA_SERVER_EXTRAS_TESTS_TEST_UTILS_H_
#define TINY_ALPACA_SERVER_EXTRAS_TESTS_TEST_UTILS_H_

#include <sstream>

#include "extras/host_arduino/print.h"

namespace alpaca {

class PrintToString : public Print {
 public:
  size_t write(uint8_t b) override {
    const uint8_t* buffer = &b;
    return write(buffer, 1);
  }

  size_t write(const uint8_t* buffer, size_t size) override {
    out_.write(reinterpret_cast<const char*>(buffer), size);
    return size;
  }

  std::string str() const { return out_.str(); }

  void reset() { out_.str(""); }

 private:
  std::ostringstream out_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TESTS_TEST_UTILS_H_
