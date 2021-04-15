#include "utils/printable_progmem_string.h"

#include "utils/platform.h"

namespace alpaca {

size_t PrintableProgmemString::printTo(Print& out) const {
  char buffer[32];
  const char* next = ptr_;
  size_t remaining = size_;
  size_t total = 0;
  while (remaining > sizeof buffer) {
    memcpy_P(buffer, next, sizeof buffer);
    total += out.write(buffer, sizeof buffer);
    next += sizeof buffer;
    remaining -= sizeof buffer;
  }
  if (remaining > 0) {
    memcpy_P(buffer, next, remaining);
    total += out.write(buffer, remaining);
  }
  return total;
}

}  // namespace alpaca
