#ifndef TINY_ALPACA_SERVER_TOKEN_H_
#define TINY_ALPACA_SERVER_TOKEN_H_

// Defines a compile type evaluable Token class, used to build arrays at compile
// time of tokens to be recognized.
//
// Author: james.synge@gmail.com

#include "platform.h"
#include "string_view.h"

namespace alpaca {
namespace internal {

#ifdef INCOMPLETE_EXPERIMENT
// GOAL: I want to figure out how to use std::enable_if and functions such as
// the following at compile time to prevent MakeToken from being used to create
// a token from being created that has the wrong type of characters in it (e.g.
// an HTTP method name is all upper case, while an ASCOM Alpaca device type is
// all lower case).

// Compile time test of whether a literal is made up only of lower case letters.
template <StringView::size_type N>
constexpr bool IsLowerCaseLiteral(const char (&buf)[N]) {
  if (N == 0) {
    return true;
  } else if (N == 1) {
    return buf[0] == '\0';
  } else {
    return 'a' <= buf[0] && buf[0] <= 'z' && IsLowerCaseLiteral<N - 1>(buf + 1);
  }
}

// Compile time test of whether a literal contains no upper case letters.
template <StringView::size_type N>
constexpr bool IsNonUpperCaseLiteral(const char (&buf)[N]) {
  if (N == 0) {
    return true;
  } else if (N == 1) {
    return buf[0] == '\0';
  } else {
    return !('A' <= buf[0] && buf[0] <= 'Z') &&
           IsNonUpperCaseLiteral<N - 1>(buf + 1);
  }
}

#endif  // INCOMPLETE_EXPERIMENT

}  // namespace internal

template <typename E>
struct Token {
  constexpr Token(const StringView& str, E id) : str(str), id(id) {}
  constexpr Token(const Token<E>&) = default;
  constexpr Token(Token<E>&&) = default;

  constexpr std::pair<E, StringView::size_type> match_info() const {
    return {id, str.size()};
  }

  const StringView str;
  const E id;
};

template <StringView::size_type N, typename E>
constexpr Token<E> MakeToken(const char (&buf)[N], E e) {
  return Token<E>(StringView(buf), e);
}

template <typename E>
constexpr Token<E> MakeToken(const StringView& view, E e) {
  return Token<E>(view, e);
}

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_TOKEN_H_
