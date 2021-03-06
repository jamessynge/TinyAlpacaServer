// TODO(jamessynge): Describe why this file exists/what it provides.

#include "utils/escaping.h"

#include "utils/logging.h"
#include "utils/platform.h"

namespace alpaca {

size_t PrintCharJsonEscaped(Print& out, const char c) {
  size_t total = 0;
  if (isPrintable(c)) {
    if (c == '"') {
      total += out.print('\\');
      total += out.print('"');
    } else if (c == '\\') {
      total += out.print('\\');
      total += out.print('\\');
    } else {
      total += out.print(c);
    }
  } else if (c == '\b') {
    total += out.print('\\');
    total += out.print('b');
  } else if (c == '\f') {
    total += out.print('\\');
    total += out.print('f');
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
    // This used to be a DCHECK, but a DVLOG is better because the character
    // could come from client input.
    TAS_DVLOG(4, "Unsupported JSON character: " << std::hex << (c + 0L));
  }
  return total;
}

#if TAS_HOST_TARGET
void StreamCharJsonEscaped(std::ostream& out, const char c) {
  if (isPrintable(c)) {
    if (c == '"') {
      out.put('\\');
      out.put('"');
    } else if (c == '\\') {
      out.put('\\');
      out.put('\\');
    } else {
      out.put(c);
    }
  } else if (c == '\b') {
    out.put('\\');
    out.put('b');
  } else if (c == '\f') {
    out.put('\\');
    out.put('f');
  } else if (c == '\n') {
    out.put('\\');
    out.put('n');
  } else if (c == '\r') {
    out.put('\\');
    out.put('r');
  } else if (c == '\t') {
    out.put('\\');
    out.put('t');
  } else {
    // This used to be a DCHECK, but a DVLOG is better because the character
    // could come from client input.
    TAS_DVLOG(4, "Unsupported JSON character: " << std::hex << (c + 0L));
  }
}
#endif  // TAS_HOST_TARGET

}  // namespace alpaca
