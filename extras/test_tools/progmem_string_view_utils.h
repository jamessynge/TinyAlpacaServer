#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_PROGMEM_STRING_VIEW_UTILS_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_PROGMEM_STRING_VIEW_UTILS_H_

// Host utilities for working with alpaca::ProgmemStringView in the context of
// the C++ standard library.
//
// Author: james.synge@gmail.com

#include <ostream>

#include "experimental/users/jamessynge/arduino/mcucore/src/progmem_string_view.h"

namespace alpaca {

inline std::ostream& operator<<(std::ostream& out,
                                const ProgmemStringView& view) {
  for (const char c : view) {
    out << c;
  }
  return out;
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_PROGMEM_STRING_VIEW_UTILS_H_
