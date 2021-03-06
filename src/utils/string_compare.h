#ifndef TINY_ALPACA_SERVER_SRC_UTILS_STRING_COMPARE_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_STRING_COMPARE_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

#include "utils/literal.h"
#include "utils/platform.h"
#include "utils/string_view.h"

namespace alpaca {

// Returns true if a Literal and a StringView have identical contents.
bool operator==(const Literal& a, const StringView& b);
bool operator==(const StringView& a, const Literal& b);
bool ExactlyEqual(const Literal& a, const StringView& b);

// Returns true if a Literal and a StringView are not identical.
bool operator!=(const Literal& a, const StringView& b);
bool operator!=(const StringView& a, const Literal& b);

// Returns true if a Literal and a StringView are the same, case-insensitively.
bool CaseEqual(const Literal& a, const StringView& b);
bool CaseEqual(const StringView& a, const Literal& b);

// Returns true if text starts with prefix.
bool StartsWith(const StringView& text, const Literal& prefix);

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_STRING_COMPARE_H_
