#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_SAMPLE_PRINTABLE_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_SAMPLE_PRINTABLE_H_

// SamplePrintable supports testing APIs that accept a Printable.

#include "extras/host_arduino/print.h"

namespace alpaca {

struct SamplePrintable : public Printable {
  SamplePrintable() {}
  explicit SamplePrintable(const std::string& value) : str(value) {}
  ~SamplePrintable() override {}

  size_t printTo(Print& p) const override {
    return p.write(str.data(), str.size());
  }

  std::string str;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_SAMPLE_PRINTABLE_H_
