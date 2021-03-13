#ifndef TINY_ALPACA_SERVER_SRC_UTILS_LITERAL_TOKEN_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_LITERAL_TOKEN_H_

// Provides support for searching fixed size tables of Literals for a match to
// a StringView. For now these tables are in the .data section (i.e. copied into
// RAM), but ideally we'd be able store these in PROGMEM and thus save RAM on
// the AVR chips used on Arduino Unos, Megas, and related boards.

#include "utils/literal.h"
#include "utils/platform.h"
#include "utils/string_compare.h"
#include "utils/string_view.h"

namespace alpaca {

template <typename T>
struct LiteralToken {
  // constexpr LiteralToken(const Literal& str, T id) : str(str), id(id) {}

  const Literal str;
  const T id;
};

using LiteralMatchFunction = bool (*)(const Literal&, const StringView&);

// Returns true if func returns true for one of the literals, and sets
// matched_id to the corresponding id.
template <typename T, int N>
bool FindFirstMatchingLiteralToken(const StringView& view,
                                   const LiteralToken<T> (&tokens)[N],
                                   LiteralMatchFunction func, T& matched_id) {
  for (int i = 0; i < N; ++i) {
    const auto& token = tokens[i];
    if (func(token.str, view)) {
      TAS_DVLOG(3, "FindFirstMatchingLiteralToken matched "
                       << ToHexEscapedString(token.str) << " to "
                       << ToHexEscapedString(view) << ", with id "
                       << (token.id + 0L));
      matched_id = token.id;
      return true;
    }
  }
  TAS_DVLOG(3, "FindFirstMatchingLiteralToken unable to match "
                   << ToHexEscapedString(view));
  return false;
}

// Returns true if one of the literals exactly matches the view, and if so sets
// matched_id to the id of that literal.
template <typename T, int N>
bool MaybeMatchLiteralTokensExactly(const StringView& view,
                                    const LiteralToken<T> (&tokens)[N],
                                    T& matched_id) {
  TAS_DVLOG(
      3, "MaybeMatchLiteralTokensExactly view: " << ToHexEscapedString(view));
  return FindFirstMatchingLiteralToken(view, tokens, ExactlyEqual, matched_id);
}

// Returns true if one of the literals matches the view case-insensitively, and
// if so sets matched_id to the id of that literal.
template <typename T, int N>
bool MaybeMatchLiteralTokensCaseInsensitively(
    const StringView& view, const LiteralToken<T> (&tokens)[N], T& matched_id) {
  TAS_DVLOG(3, "MaybeMatchLiteralTokensCaseInsensitively view: "
                   << ToHexEscapedString(view));
  return FindFirstMatchingLiteralToken(view, tokens, CaseEqual, matched_id);
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_LITERAL_TOKEN_H_
