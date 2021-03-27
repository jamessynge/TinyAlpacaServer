#include "utils/string_view.h"

#if TAS_HOST_TARGET
#include <string_view>
#endif  // TAS_HOST_TARGET

#include "utils/platform.h"

#if TAS_HOST_TARGET
#include "absl/strings/escaping.h"
#endif  // TAS_HOST_TARGET

namespace alpaca {

// Generally speaking, methods are implemented in the same order in which they
// were declared in the header file.

#if TAS_HOST_TARGET
StringView::StringView(const std::string& str)
    : ptr_(str.data()), size_(str.size()) {
  TAS_DCHECK_LE(str.size(), kMaxSize);
}
#endif

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
  TAS_DVLOG(2,
            "StringView::to_uint32 converting " << ToHexEscapedString(*this));
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

  TAS_DVLOG(2, "StringView::to_uint32 produced " << value);
  out = value;
  return true;
}

size_t StringView::printTo(Print& p) const { return p.write(ptr_, size_); }

#if TAS_HOST_TARGET
std::string ToStdString(const StringView& view) {
  return std::string(view.data(), view.size());
}

std::string ToHexEscapedString(const StringView& view) {
  auto std_view = std::string_view(view.data(), view.size());
  return absl::StrCat("\"", absl::CHexEscape(std_view), "\"");
}

std::ostream& operator<<(std::ostream& out, const StringView& view) {
  return out.write(view.data(), view.size());
}

bool operator==(const StringView& a, std::string_view b) {
  return a == StringView(b.data(), b.size());
}

bool operator==(std::string_view a, const StringView& b) {
  return b == StringView(a.data(), a.size());
}
#endif

}  // namespace alpaca
