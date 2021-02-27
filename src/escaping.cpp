// TODO(jamessynge): Describe why this file exists/what it provides.

#include "escaping.h"

#include "platform.h"

namespace alpaca {

size_t PrintCharJsonEscaped(Print& out, const char c) {
  size_t total = 0;
  if (isPrintable(c)) {
    if (c == '"') {
      total = out.print('\\');
      total = out.print('"');
    } else if (c == '\\') {
      total = out.print('\\');
      total = out.print('\\');
    } else {
      total = out.print(c);
    }
  } else if (c == '\b') {
    total = out.print('\\');
    total = out.print('b');
  } else if (c == '\f') {
    total = out.print('\\');
    total = out.print('f');
  } else if (c == '\n') {
    total = out.print('\\');
    total = out.print('n');
  } else if (c == '\r') {
    total = out.print('\\');
    total = out.print('r');
  } else if (c == '\t') {
    total = out.print('\\');
    total = out.print('t');
  }
  TAS_DCHECK(false, "Unsupported JSON character: 0x" << std::hex << (c + 0L));
  return total;
}

StringView GetCharJsonEscaped(const char& c) {  // NOLINT
  if (isPrintable(c)) {
    if (c == '"') {
      return StringView("\\\"");
    } else if (c == '\\') {
      return StringView("\\\\");
    } else {
      return StringView(&c, 1);
    }
  } else if (c == '\b') {
    return StringView("\\b");
  } else if (c == '\f') {
    return StringView("\\f");
  } else if (c == '\n') {
    return StringView("\\n");
  } else if (c == '\r') {
    return StringView("\\r");
  } else if (c == '\t') {
    return StringView("\\t");
  }
  TAS_DCHECK(false, "Unsupported JSON character: 0x" << std::hex << (c + 0));
  return StringView();
}

}  // namespace alpaca
