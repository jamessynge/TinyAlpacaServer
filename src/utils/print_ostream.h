#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PRINT_OSTREAM_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PRINT_OSTREAM_H_

// Support for streaming into a Print instance, primarily for logging.

#include "utils/platform.h"

namespace alpaca {

struct PrintOStream {
  explicit PrintOStream(Print& out) : out(out) {}
  Print& out;
};

inline PrintOStream& operator<<(PrintOStream& out, const Printable& value) {
  value.printTo(out.out);
  return out;
}

inline PrintOStream& operator<<(PrintOStream& out, uint32_t value) {
  out.out.print(value);
  return out;
}

inline PrintOStream& operator<<(PrintOStream& out, int32_t value) {
  out.out.print(value);
  return out;
}

inline PrintOStream& operator<<(PrintOStream& out, float value) {
  out.out.print(value);
  return out;
}

inline PrintOStream& operator<<(PrintOStream& out, double value) {
  out.out.print(value);
  return out;
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_PRINT_OSTREAM_H_
