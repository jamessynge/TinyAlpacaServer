#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_LITERAL_UTILS_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_LITERAL_UTILS_H_

// Host utilities for working with alpaca::Literal in the context of the C++
// standard library.
//
// Author: james.synge@gmail.com

#include "utils/literal.h"

namespace alpaca {

inline std::ostream& operator<<(std::ostream& out, const Literal& literal) {
  for (const char c : literal) {
    out << c;
  }
  return out;
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_LITERAL_UTILS_H_
