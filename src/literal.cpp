#include "src/literal.h"

namespace alpaca {

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

bool Literal::copyTo(char* out, size_type size) {
  if (size <= size_) {
    return false;
  }
  memcpy_P(out, ptr_, size_ + 1);
  return true;
}

namespace {
char pgm_read_char(const char* ptr) {
  auto byte = pgm_read_byte(reinterpret_cast<const uint8_t*>(ptr));
  return static_cast<char>(byte);
}
}  // namespace

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
