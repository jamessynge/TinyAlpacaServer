#ifndef TINY_ALPACA_SERVER_SRC_UTILS_LITERAL_TOKEN_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_LITERAL_TOKEN_H_

// Provides support for searching fixed size tables of Literals for a match to
// a StringView. For now these tables are in the .data section (i.e. copied into
// RAM), but ideally we'd be able store these in PROGMEM and thus save RAM on
// the AVR chips used on Arduino Unos, Megas, and related boards.

#include "utils/array_view.h"
#include "utils/hex_escape.h"
#include "utils/literal.h"
#include "utils/platform.h"
#include "utils/string_compare.h"
#include "utils/string_view.h"

namespace alpaca {

template <typename T>
struct LiteralToken {
  using LiteralFunc = Literal (*)();
  const LiteralFunc func;
  const T id;
};

using LiteralMatchFunction = bool (*)(const Literal&, const StringView&);

// Returns true if func returns true for one of the literals, and sets
// matched_id to the corresponding id.
template <typename T>
bool FindFirstMatchingLiteralToken(
    const StringView& view, const ArrayView<const LiteralToken<T>> tokens,
    LiteralMatchFunction func, T& matched_id) {
  for (const auto& token : tokens) {
    const auto token_str = token.func();
    if (func(token_str, view)) {
      TAS_VLOG(3) << TASLIT("FindFirstMatchingLiteralToken matched ")
                  << HexEscaped(token_str) << TASLIT(" to ") << HexEscaped(view)
                  << TASLIT(", with id ") << (token.id + 0L);
      matched_id = token.id;
      return true;
    }
  }
  TAS_VLOG(3) << TASLIT("FindFirstMatchingLiteralToken unable to match ")
              << HexEscaped(view);
  return false;
}

// Returns true if one of the literals exactly matches the view, and if so sets
// matched_id to the id of that literal.
template <typename T>
bool MaybeMatchLiteralTokensExactly(
    const StringView& view, const ArrayView<const LiteralToken<T>> tokens,
    T& matched_id) {
  TAS_VLOG(3) << TASLIT("MaybeMatchLiteralTokensExactly view: ")
              << HexEscaped(view);
  return FindFirstMatchingLiteralToken(view, tokens, ExactlyEqual, matched_id);
}

// Returns true if one of the literals matches the view case-insensitively, and
// if so sets matched_id to the id of that literal.
template <typename T>
bool MaybeMatchLiteralTokensCaseInsensitively(
    const StringView& view, const ArrayView<const LiteralToken<T>> tokens,
    T& matched_id) {
  TAS_VLOG(3) << TASLIT("MaybeMatchLiteralTokensCaseInsensitively view: ")
              << HexEscaped(view);
  return FindFirstMatchingLiteralToken(view, tokens, CaseEqual, matched_id);
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_LITERAL_TOKEN_H_
