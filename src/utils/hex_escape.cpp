#include "utils/hex_escape.h"

#include "utils/logging.h"
#include "utils/platform.h"

namespace alpaca {

constexpr char kHexDigits[] PROGMEM = "0123456789ABCDEF";

size_t PrintCharHexEscaped(Print& out, const char c) {
  size_t total = 0;
  if (' ' <= c && c < 127) {
    if (c == '"') {
      total += out.print('\\');
      total += out.print('"');
    } else if (c == '\\') {
      total += out.print('\\');
      total += out.print('\\');
#if 0
    } else if (c == '?') {
      // C++ 14 and before support trigraphs as a way of representing
      // characters. If that is important, modify the #if around this.
      total += out.print('\\');
      total += out.print('?');
#endif
    } else {
      total += out.print(c);
    }
  } else if (c == '\n') {
    total += out.print('\\');
    total += out.print('n');
  } else if (c == '\r') {
    total += out.print('\\');
    total += out.print('r');
  } else if (c == '\t') {
    total += out.print('\\');
    total += out.print('t');
  } else {
    total += out.print('\\');
    total += out.print('x');
    total += out.print(kHexDigits[(c >> 4) & 0xf]);
    total += out.print(kHexDigits[c & 0xf]);
  }
  return total;
}

PrintHexEscaped::PrintHexEscaped(Print& wrapped) : wrapped_(wrapped) {}

size_t PrintHexEscaped::write(uint8_t b) {
  return PrintCharHexEscaped(wrapped_, static_cast<char>(b));
}

size_t PrintHexEscaped::write(const uint8_t* buffer, size_t size) {
  size_t count = 0;
  for (size_t ndx = 0; ndx < size; ++ndx) {
    count += PrintCharHexEscaped(wrapped_, static_cast<char>(buffer[ndx]));
  }
  return count;
}

// size_t HexEscapedInteger::printTo(Print& out) const {
//   char buffer[16];
//   char* p = buffer + sizeof buffer;
//   *(--p) = '\0';  // NUL terminate the string.
//   auto value = value_;
//   *(--p) = kHexDigits[value & 0xf];
//   value >>= 4;
//   while (value != 0) {
//     TAS_CHECK_GT(p, buffer);
//     *(--p) = kHexDigits[value & 0xf];
//     value >>= 4;
//   }
//   TAS_CHECK_GT(p, buffer);
//   *(--p) = 'x';
//   TAS_CHECK_GT(p, buffer);
//   *(--p) = '0';
//   return out.write(p);
// }

}  // namespace alpaca
