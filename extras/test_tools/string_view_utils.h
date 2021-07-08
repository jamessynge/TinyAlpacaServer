#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_STRING_VIEW_UTILS_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_STRING_VIEW_UTILS_H_

// Host utilities for working with alpaca::StringView in the context of the C++
// standard library.
//
// Author: james.synge@gmail.com

#include <ostream>
#include <string>
#include <string_view>

#include "utils/string_view.h"

namespace alpaca {
namespace test {

inline StringView MakeStringView(const std::string& str) {
  return StringView(str.data(), str.size());
}

inline StringView MakeStringView(std::string_view view) {
  return StringView(view.data(), view.size());
}

inline std::string_view MakeStdStringView(const StringView& view) {
  return std::string_view(view.data(), view.size());
}

}  // namespace test

// Insertion streaming operator (i.e. operator<<) for values of type StringView,
// used on host for tests, DCHECK_EQ, DVLOG, etc.
inline std::ostream& operator<<(std::ostream& out, const StringView& view) {
  return out << test::MakeStdStringView(view);
}

// The equals operators below are used for tests, CHECK_EQ, etc., where we want
// to compare StringViews against strings from the standard library. They aren't
// used by the embedded portion of the decoder.
inline bool operator==(const StringView& a, std::string_view b) {
  return test::MakeStdStringView(a) == b;
}
inline bool operator==(std::string_view a, const StringView& b) {
  return test::MakeStdStringView(b) == a;
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_STRING_VIEW_UTILS_H_
