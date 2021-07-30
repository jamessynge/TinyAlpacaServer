#include "utils/progmem_string_view.h"

// NOTE: There is no use of logging.h here because that introduces an include
// cycle, and hence a BUILD dependency sycle.

#include "utils/traits/print_to_trait.h"

namespace alpaca {
namespace {
// Returns the char at the specified location in PROGMEM.
inline char pgm_read_char_near(PGM_P ptr) {
  auto byte = pgm_read_byte_near(reinterpret_cast<const uint8_t*>(ptr));
  return static_cast<char>(byte);
}
}  // namespace

size_t ProgmemStringView::printTo(Print& out) const {
  static_assert(has_print_to<decltype(*this)>{}, "has_print_to should be true");
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

bool ProgmemStringView::operator==(const ProgmemStringView& other) const {
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

bool ProgmemStringView::Identical(const ProgmemStringView& other) const {
  return ptr_ == other.ptr_ && size_ == other.size_;
}

bool ProgmemStringView::Equal(const char* other, size_type other_size) const {
  if (size_ != other_size) {
    return false;
  }
  return 0 == memcmp_P(other, ptr_, size_);
}

// Returns true if the two strings are equal, with case insensitive comparison
// of characters. other points to a string in RAM, not PROGMEM.
bool ProgmemStringView::CaseEqual(const char* other,
                                  size_type other_size) const {
  if (size_ != other_size) {
    return false;
  }
  return 0 == strncasecmp_P(other, ptr_, size_);
}

// TODO(jamessynge): Use this to save memory by allowing mixed case strings to
// be used for output (e.g. AveragePeriod), and to also be compared against
// strings that are required to be lower-case.
bool ProgmemStringView::LoweredEqual(const char* other,
                                     size_type other_size) const {
  if (size_ != other_size) {
    return false;
  }
  for (size_type offset = 0; offset < size_; ++offset) {
    const char c = at(offset);
    const char lc_c = isUpperCase(c) ? (c | static_cast<char>(0x20)) : c;
    if (lc_c != other[offset]) {
      return false;
    }
  }
  return true;
}

bool ProgmemStringView::IsPrefixOf(const char* other,
                                   size_type other_size) const {
  if (size_ > other_size) {
    // Can't be a prefix of a shorter string.
    return false;
  }
  return 0 == memcmp_P(other, ptr_, size_);
}

// If 'size_' is not greater than the provided 'size', copies the literal
// string into *out. No NUL terminator is copied.
bool ProgmemStringView::CopyTo(char* out, size_type size) {
  if (size < size_) {
    return false;
  }
  memcpy_P(out, ptr_, size_);
  return true;
}

char ProgmemStringView::at(size_type pos) const {
  return pgm_read_char_near(ptr_ + pos);
}

}  // namespace alpaca
