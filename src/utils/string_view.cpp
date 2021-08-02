#include "utils/string_view.h"

#include "experimental/users/jamessynge/arduino/mcucore/src/print_to_trait.h"
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

namespace {
constexpr uint32_t kMaxUInt = 0xFFFFFFFF;
constexpr uint32_t kMaxUIntDiv10 = kMaxUInt / 10;

bool AddOnesPlace(uint32_t& value, const char c) {
  if (!('0' <= c && c <= '9')) {
    return false;
  }
  if (value > kMaxUIntDiv10) {
    return false;
  }
  uint32_t digit = static_cast<uint32_t>(c - '0');
  value *= 10;
  if (value > kMaxUInt - digit) {
    return false;
  }
  value += digit;
  return true;
}

bool AddOnesPlace(double& value, const char c) {
  if (!('0' <= c && c <= '9')) {
    return false;
  }
  value *= 10;
  value += (c - '0');
  return true;
}

}  // namespace

bool StringView::to_uint32(uint32_t& out) const {
  TAS_VLOG(7) << TAS_FLASHSTR("StringView::to_uint32 converting ")
              << HexEscaped(*this);
  if (empty()) {
    return false;
  }
  uint32_t value = 0;
  for (const char c : *this) {
    if (!AddOnesPlace(value, c)) {
      return false;
    }
  }
  TAS_VLOG(5) << TAS_FLASHSTR("StringView::to_uint32 produced ") << value;
  out = value;
  return true;
}

bool StringView::to_int32(int32_t& out) const {
  TAS_VLOG(7) << TAS_FLASHSTR("StringView::to_int32 converting ")
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
  TAS_VLOG(5) << TAS_FLASHSTR("StringView::to_int32 produced ") << out;
  return true;
}

bool StringView::to_double(double& out) const {
  TAS_VLOG(7) << TAS_FLASHSTR("StringView::to_double converting ")
              << HexEscaped(*this);
  StringView copy(*this);
  bool negative = copy.match_and_consume('-');
  if (copy.empty()) {
    return false;
  }

  bool decoding_fraction = false;
  double value = 0;
  double divisor = 1;

  for (const char c : copy) {
    if (c == '.') {
      if (decoding_fraction) {
        return false;
      }
      if (copy.size() == 1) {
        return false;
      }
      decoding_fraction = true;
    } else {
      if (!AddOnesPlace(value, c)) {
        return false;
      }
      if (decoding_fraction) {
        divisor *= 10;
      }
    }
  }

  if (divisor > 1) {
    value /= divisor;
  }
  if (negative) {
    value = -value;
  }
  TAS_VLOG(5) << TAS_FLASHSTR("StringView::to_double produced ") << value;
  out = value;
  return true;
}

size_t StringView::printTo(Print& p) const {
  static_assert(has_print_to<decltype(*this)>{}, "has_print_to should be true");
  return p.write(ptr_, size_);
}

}  // namespace alpaca
