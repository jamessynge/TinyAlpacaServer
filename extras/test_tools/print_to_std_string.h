#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_PRINT_TO_STD_STRING_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_PRINT_TO_STD_STRING_H_

// PrintToStdString helps with testing methods that accept a Print object and
// print/write to it. And PrintValueToStdString helps with formatting values for
// which we have Arduino compatible formatters, but not std::ostream formatters.
//
// Note that these are not in namespace alpaca::test so that they can be used by
// non-tests (i.e. for implementing operator<< for host use, but not necessarily
// tests).
//
// Author: james.synge@gmail.com

#include <sstream>
#include <string>

#include "extras/host/arduino/print.h"
#include "utils/o_print_stream.h"

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
  OPrintStream strm(out);
  strm << t;
  return out.str();
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_PRINT_TO_STD_STRING_H_
