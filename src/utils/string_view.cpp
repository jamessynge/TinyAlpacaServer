#include "utils/string_view.h"

#include "utils/hex_escape.h"

namespace alpaca {

// Generally speaking, methods are implemented in the same order in which they
// were declared in the header file.

bool StringView::operator==(const StringView& other) const {
  if (other.size_ != size_) {
    return false;
  }
  auto ndx = size_;
  while (ndx > 0) {
    --ndx;
    if (ptr_[ndx] != other.ptr_[ndx]) {
      return false;
    }
  }
  return true;
}

bool StringView::operator==(const char* other) const {
  if (other == nullptr) {
    return empty();
  }
  for (size_type ndx = 0; ndx < size_ && *other != '\0'; ++ndx, ++other) {
    if (at(ndx) != *other) {
      return false;
    }
  }
  return *other == '\0';
}

bool StringView::operator!=(const StringView& other) const {
  return !(*this == other);
}

bool StringView::to_uint32(uint32_t& out) const {
  TAS_VLOG(7) << TASLIT("StringView::to_uint32 converting ")
              << HexEscaped(*this);
  if (empty()) {
    return false;
  }
  constexpr uint32_t value_max = 0xFFFFFFFF;
  constexpr uint32_t value_max_div_10 = value_max / 10;
  uint32_t value = 0;
  for (const char c : *this) {
    if (!('0' <= c && c <= '9')) {
      return false;
    }
    uint32_t digit = static_cast<uint32_t>(c - '0');
    if (value > value_max_div_10) {
      return false;
    }
    value *= 10;
    if (value > value_max - digit) {
      return false;
    }
    value += digit;
  }
  TAS_VLOG(5) << TASLIT("StringView::to_uint32 produced ") << value;
  out = value;
  return true;
}

bool StringView::to_int32(int32_t& out) const {
  TAS_VLOG(7) << TASLIT("StringView::to_int32 converting ")
              << HexEscaped(*this);
  StringView copy(*this);
  bool negative = copy.match_and_consume('-');
  uint32_t value;
  if (!copy.to_uint32(value)) {
    return false;
  }
  if (negative) {
    // I'm assuming 2s complement numbers.
    constexpr uint32_t value_max = 0x80000000;
    if (value <= value_max) {
      out = -value;
    } else {
      return false;
    }
  } else {
    constexpr uint32_t value_max = 0x7fffffff;
    if (value > value_max) {
      return false;
    }
    out = static_cast<int32_t>(value);
  }
  TAS_VLOG(5) << TASLIT("StringView::to_int32 produced ") << out;
  return true;
}

size_t StringView::printTo(Print& p) const { return p.write(ptr_, size_); }

}  // namespace alpaca
