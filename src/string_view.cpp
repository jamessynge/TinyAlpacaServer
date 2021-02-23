#include "src/string_view.h"

// Author: james.synge@gmail.com

#include "src/platform.h"

#if TAS_HOST_TARGET
#include "absl/strings/escaping.h"
#endif

namespace alpaca {

// Generally speaking, methods are implemented in the same order in which they
// were declared in the header file.

#if TAS_HOST_TARGET
StringView::StringView(const std::string& str)
    : ptr_(str.data()), size_(str.size()) {
  TAS_DCHECK_LE(str.size(), kMaxSize, "");
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

bool StringView::equals_other_lowered(const StringView& other) const {
  if (size_ != other.size()) {
    return false;
  }
  for (StringView::size_type pos = 0; pos < size_; ++pos) {
    const char our_lc_char = ptr_[pos];
    TAS_DCHECK(!isUpperCase(our_lc_char), substr(pos, 1));
    const char other_char = other.at(pos);
    const char other_lc_char = isUpperCase(other_char)
                                   ? (other_char | static_cast<char>(0x20))
                                   : other_char;
    if (our_lc_char != other_lc_char) {
      return false;
    }
  }
  return true;
}

bool StringView::to_uint32(uint32_t& out) const {
  TAS_DVLOG(2, "StringView::to_uint32 converting " << ToHexEscapedString());
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

JsonStringView StringView::escaped() const { return JsonStringView(*this); }

size_t StringView::printTo(Print& p) const { return p.write(ptr_, size_); }

#if TAS_HOST_TARGET
// void StringView::WriteTo(std::ostream& out) const { out.write(ptr_, size_); }

std::string_view StringView::ToStdStringView() const {
  return std::string_view(data(), size());
}

std::string StringView::ToString() const { return std::string(data(), size()); }

std::string StringView::ToHexEscapedString() const {
  return absl::StrCat("\"", absl::CHexEscape(ToStdStringView()), "\"");
}
#endif

JsonStringView::JsonStringView(const StringView& view) : view_(view) {}

size_t JsonStringView::printTo(Print& p) const {
  size_t total = StringView("\"").printTo(p);
  for (const char& c : view_) {
    total += GetJsonEscaped(c).printTo(p);
  }
  total += StringView("\"").printTo(p);
  return total;
}

// static
StringView JsonStringView::GetJsonEscaped(const char& c) {  // NOLINT
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
  return StringView("");
}

#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, const StringView& view) {
  return out.write(view.data(), view.size());
}

std::ostream& operator<<(std::ostream& out, const JsonStringView& view) {
  out << StringView("\"");
  for (const char& c : view.view()) {
    out << JsonStringView::GetJsonEscaped(c);
  }
  out << StringView("\"");
  return out;
}

bool operator==(const StringView& a, std::string_view b) {
  return a == StringView(b.data(), b.size());
}

bool operator==(std::string_view a, const StringView& b) {
  return b == StringView(a.data(), a.size());
}
#endif

}  // namespace alpaca
