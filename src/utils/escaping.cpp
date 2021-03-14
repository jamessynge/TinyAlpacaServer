// TODO(jamessynge): Describe why this file exists/what it provides.

#include "utils/escaping.h"

#include "utils/logging.h"
#include "utils/platform.h"

namespace alpaca {

PrintJsonEscaped::PrintJsonEscaped(Print& wrapped) : wrapped_(wrapped) {}

size_t PrintJsonEscaped::write(uint8_t b) {
  return PrintCharJsonEscaped(wrapped_, static_cast<char>(b));
}

size_t PrintJsonEscaped::write(const uint8_t* buffer, size_t size) {
  size_t count = 0;
  for (int ndx = 0; ndx < size; ++ndx) {
    count += PrintCharJsonEscaped(wrapped_, static_cast<char>(buffer[ndx]));
  }
  return count;
}

size_t PrintJsonEscapedTo(const Printable& value, Print& raw_output) {
  PrintJsonEscaped out(raw_output);
  return value.printTo(out);
}

size_t PrintJsonEscapedStringTo(const Printable& value, Print& raw_output) {
  PrintJsonEscaped out(raw_output);
  size_t count = raw_output.print('"');
  count += value.printTo(out);
  count += raw_output.print('"');
  return count;
}

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
