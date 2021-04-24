#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_LITERAL_UTILS_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_LITERAL_UTILS_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

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
