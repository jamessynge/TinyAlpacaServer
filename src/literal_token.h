#ifndef TINY_ALPACA_SERVER_SRC_LITERAL_TOKEN_H_
#define TINY_ALPACA_SERVER_SRC_LITERAL_TOKEN_H_

// Defines a compile type evaluable LiteralToken class, used to build arrays of
// enum+literal_func at compile time, where literal_func is a function that
// returns a Literal. Ideally we store those arrays in PROGMEM, though I want to
// finish writing ProgMemPointer to make that easier.

#include "literal.h"
#include "platform.h"

namespace alpaca {

template <typename T>
struct LiteralToken {
  constexpr LiteralToken(const Literal& str, T id) : str(str), id(id) {}

  const Literal str;
  const T id;
};

// tokens is an array in RAM, not PROGMEM.
template <typename T, int N>
T MatchLiteralTokensExactly(const StringView& view, T unknown_id,
                            const LiteralToken<T> (&tokens)[N]) {
  for (int i = 0; i < N; ++i) {
    if (tokens[i].str == view) {
      TAS_DVLOG(3, "MatchTokensExactly matched "
                       << tokens[i].str.ToHexEscapedString() << " to "
                       << view.ToHexEscapedString() << ", returning "
                       << tokens[i].id);
      return tokens[i].id;
    }
  }
  TAS_DVLOG(3, "MatchTokensExactly unable to match "
                   << view.ToHexEscapedString() << ", returning "
                   << unknown_id);
  return unknown_id;
}

template <typename E>
struct LiteralToken {
  constexpr LiteralToken(const Literal& str, E id) : str(str), id(id) {}
  // constexpr LiteralToken(const Token<E>&) = default;
  // constexpr Token(Token<E>&&) = default;

  const Literal str;
  const E id;
};

// Declare stuff

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_LITERAL_TOKEN_H_
