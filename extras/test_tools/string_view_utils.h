#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_STRING_VIEW_UTILS_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_STRING_VIEW_UTILS_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

#include <ostream>
#include <string_view>

#include "utils/string_view.h"

namespace alpaca {

inline StringView MakeStringView(const std::string& str) {
  return StringView(str.data(), str.size());
}

inline StringView MakeStringView(std::string_view view) {
  return StringView(view.data(), view.size());
}

inline std::string_view MakeStdStringView(const StringView& view) {
  return std::string_view(view.data(), view.size());
}

// Insertion streaming operator (i.e. operator<<) for values of type StringView,
// used on host for tests, DCHECK_EQ, DVLOG, etc.
inline std::ostream& operator<<(std::ostream& out, const StringView& view) {
  return out << MakeStdStringView(view);
}

// The equals operators below are used for tests, CHECK_EQ, etc., where we want
// to compare StringViews against strings from the standard library. They aren't
// used by the embedded portion of the decoder.
inline bool operator==(const StringView& a, std::string_view b) {
  return MakeStdStringView(a) == b;
}
inline bool operator==(std::string_view a, const StringView& b) {
  return MakeStdStringView(b) == a;
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_STRING_VIEW_UTILS_H_
