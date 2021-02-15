#ifndef ALPACA_DECODER_TOKEN_H_
#define ALPACA_DECODER_TOKEN_H_

// Author: james.synge@gmail.com

#include "absl/strings/ascii.h"
#include "alpaca-decoder/string_view.h"

namespace alpaca {
namespace internal {

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

// NOTE: Want to figure out how to use std::enable_if et al for preventing the
// creation of a Token if the literal string contains any upper case characters,
// or if the entire string is not lower case letters (depending on nature of the
// token).

template <StringView::size_type N, typename E>
constexpr Token<E> MakeToken(const char (&buf)[N], E e) {
  return Token<E>(StringView(buf), e);
}

template <typename E>
constexpr Token<E> MakeToken(const StringView& view, E e) {
  return Token<E>(view, e);
}

}  // namespace alpaca

#endif  // ALPACA_DECODER_TOKEN_H_
