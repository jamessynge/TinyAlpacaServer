#include "utils/literal.h"

#include "utils/logging.h"
#include "utils/traits/print_to_trait.h"

namespace alpaca {
namespace {
// Returns the char at the specified location in PROGMEM.
inline char pgm_read_char(PGM_P ptr) {
  auto byte = pgm_read_byte(reinterpret_cast<const uint8_t*>(ptr));
  return static_cast<char>(byte);
}
}  // namespace

bool Literal::operator==(const Literal& other) const {
  if (size_ == other.size_) {
    if (ptr_ == other.ptr_) {
      return true;
    }
    for (size_type offset = 0; offset < size_; ++offset) {
      if (at(offset) != other.at(offset)) {
        return false;
      }
    }
    return true;
  }
  return false;
}

bool Literal::same(const Literal& other) const {
  return ptr_ == other.ptr_ && size_ == other.size_;
}

char Literal::at(const Literal::size_type pos) const {
  TAS_DCHECK_LT(pos, size_);
  return pgm_read_char(ptr_ + pos);
}

bool Literal::equal(const char* other, size_type other_size) const {
  if (size_ != other_size) {
    return false;
  }
  return 0 == memcmp_P(other, ptr_, size_);
}

bool Literal::case_equal(const char* other, size_type other_size) const {
  if (size_ != other_size) {
    return false;
  }
  return 0 == strncasecmp_P(other, ptr_, size_);
}

bool Literal::is_prefix_of(const char* other, size_type other_size) const {
  if (size_ > other_size) {
    // Can't be a prefix of a shorter string.
    return false;
  }
  return 0 == memcmp_P(other, ptr_, size_);
}

bool Literal::copyTo(char* out, size_type size) {
  if (size < size_) {
    return false;
  }
  memcpy_P(out, ptr_, size_);
  return true;
}

size_t Literal::printTo(Print& out) const {
  static_assert(has_print_to<decltype(*this)>{}, "has_print_to should be true");
  // Not particularly efficient, but probably OK. If not, use memcpy_P to copy
  // in sequential chunks of the literal into a small stack allocated buffer
  // (e.g. char buffer[16]), printing the buffer's contents after each copy
  // operation.
  size_t total = 0;
  for (size_type offset = 0; offset < size_; ++offset) {
    total += out.print(at(offset));
  }
  return total;
}

}  // namespace alpaca
