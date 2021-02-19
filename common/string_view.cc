#include "alpaca-decoder/string_view.h"

#include <string>
#include <string_view>

#include "absl/strings/escaping.h"

namespace alpaca {

bool StringView::to_uint32(uint32_t& out) const {
#if ALPACA_DECODER_HAVE_STD_STRING
  DVLOG(2) << "StringView::to_uint32 converting " << ToEscapedString();
#endif
  if (empty()) {
    return false;
  }
  constexpr uint32_t value_max = std::numeric_limits<uint32_t>::max();
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

  DVLOG(2) << "StringView::to_uint32 produced " << value;
  out = value;
  return true;
}

#if ALPACA_DECODER_HAVE_STD_STRING_VIEW
std::string_view StringView::ToStdStringView() const {
  return std::string_view(data(), size());
}

std::string StringView::ToEscapedString() const {
  return absl::StrCat("\"", absl::CHexEscape(ToStdStringView()), "\"");
}

bool operator==(const StringView& a, std::string_view b) {
  return a == StringView(b.data(), b.size());
}
#endif

#if ALPACA_DECODER_HAVE_STD_STRING
bool operator==(const StringView& a, const std::string& b) {
  return a.size() == b.size() && a == StringView(b.data(), b.size());
}

bool operator==(const std::string& a, const StringView& b) {
  return a.size() == b.size() && b == StringView(a.data(), a.size());
}
#endif  // ALPACA_DECODER_HAVE_STD_STRING

#if ALPACA_DECODER_HAVE_STD_OSTREAM
std::ostream& operator<<(std::ostream& out, StringView view) {
  return out.write(view.data(), view.size());
}
#endif  // ALPACA_DECODER_HAVE_STD_OSTREAM

}  // namespace alpaca
