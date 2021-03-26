#ifndef TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_

// Support for streaming into a Print instance, primarily for logging.

#include "utils/platform.h"

namespace alpaca {

struct OPrintStream {
  explicit OPrintStream(Print& out) : out(out) {}
  Print& out;
};

inline OPrintStream& operator<<(OPrintStream& out, const char* value) {
  out.out.print(value);
  return out;
}

inline OPrintStream& operator<<(OPrintStream& out, char value) {
  out.out.print(value);
  return out;
}

inline OPrintStream& operator<<(OPrintStream& out, unsigned char value) {
  out.out.print(value);
  return out;
}

inline OPrintStream& operator<<(OPrintStream& out, int16_t value) {
  out.out.print(value);
  return out;
}

inline OPrintStream& operator<<(OPrintStream& out, uint16_t value) {
  out.out.print(value);
  return out;
}

inline OPrintStream& operator<<(OPrintStream& out, int32_t value) {
  out.out.print(value);
  return out;
}

inline OPrintStream& operator<<(OPrintStream& out, uint32_t value) {
  out.out.print(value);
  return out;
}

#if TAS_HOST_TARGET
inline OPrintStream& operator<<(OPrintStream& out, int64_t value) {
  out.out.print(value);
  return out;
}

inline OPrintStream& operator<<(OPrintStream& out, uint64_t value) {
  out.out.print(value);
  return out;
}
#endif  // TAS_HOST_TARGET

inline OPrintStream& operator<<(OPrintStream& out, double value) {
  out.out.print(value);
  return out;
}

inline OPrintStream& operator<<(OPrintStream& out, const Printable& value) {
  value.printTo(out.out);
  return out;
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_O_PRINT_STREAM_H_
