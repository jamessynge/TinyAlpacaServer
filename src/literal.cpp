#include "literal.h"

#include "logging.h"
#include "platform.h"

namespace alpaca {
namespace {
// Returns the char at the specified location in PROGMEM.
inline char pgm_read_char(PGM_P ptr) {
  auto byte = pgm_read_byte(reinterpret_cast<const uint8_t*>(ptr));
  return static_cast<char>(byte);
}
}  // namespace

char Literal::at(const Literal::size_type pos) const {
  TAS_DCHECK_LT(pos, size_, "");
  return pgm_read_char(ptr_ + pos);
}

bool Literal::operator==(const StringView& view) const {
  if (size_ != view.size()) {
    return false;
  }
  return 0 == memcmp_P(ptr_, view.data(), size_);
}

bool Literal::operator!=(const StringView& view) const {
  return !(*this == view);
}

bool Literal::case_equal(const StringView& view) const {
  if (size_ != view.size()) {
    return false;
  }
  return 0 == strncasecmp_P(ptr_, view.data(), size_);
}

bool Literal::lowered_equal(const StringView& view) const {
  if (size_ != view.size()) {
    return false;
  }
  for (size_type offset = 0; offset < size_; ++offset) {
    const char c = at(offset);
    const char lc_c = isUpperCase(c) ? (c | static_cast<char>(0x20)) : c;
    if (lc_c != view.at(offset)) {
      return false;
    }
  }
  return true;
}

bool Literal::copyTo(char* out, size_type size) {
  if (size <= size_) {
    return false;
  }
  memcpy_P(out, ptr_, size_ + 1);
  return true;
}

size_t Literal::printTo(Print& out) const {
  // Not particularly efficient, but probably OK. If not, use memcpy_P to copy
  // in sequential chunks of the literal into a small stack allocated buffer
  // (e.g. char buffer[16]), printing the buffer's contents after each copy
  // operation.
  size_t total = 0;
  for (size_type offset = 0; offset < size_; ++offset) {
    char c = pgm_read_char(ptr_ + offset);
    total += out.print(c);
  }
  return total;
}

size_t Literal::printJsonEscapedTo(Print& out) const {
  size_t total = out.print('"');
  for (size_type offset = 0; offset < size_; ++offset) {
    char c = pgm_read_char(ptr_ + offset);
    total += JsonStringView::GetJsonEscaped(c).printTo(out);
  }
  total += out.print('"');
  return total;
}

}  // namespace alpaca
