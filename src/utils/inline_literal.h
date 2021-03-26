#ifndef TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_

// Provides a macro, TASLIT(x), that expands to a Literal instance with the
// string x as the value, which in the case of the avr-gcc compiler is stored in
// program memory (PROGMEM) rather than RAM.
//
// This is based on https://github.com/irrequietus/ProgmemString, by George
// Makrydakis <george@irrequietus.eu>. Makrydakis's typestring.hh expands a
// string literal passed to a macro function into a list of char values, thus
// enabling a string literal to be (indirectly) used as a template parameter,
// i.e. by turning it into a list of chars used as the arguments of  a variadic
// template.
//
// Since the goal here is to add this capability to this library where Literal
// already supports reading from PROGMEM, this isn't just a copy of
// typestring.hh. Instead, it contains an adaptation of Makrydakis's approach
// to this context.

#include "utils/literal.h"
#include "utils/platform.h"

namespace alpaca {
namespace progmem_data {
template <char... C>
class ProgmemString final {
 public:
  static constexpr Literal MakeLiteral() {
    return Literal(data_, sizeof...(C));
  }

 private:
  // NOTE: there is no trailing NUL here because we are using this to make a
  // Literal instance, and we know exactly the size of the literal string (i.e.
  // without the NUL), so we can pass that size to the Literal ctor.
  static constexpr char const data_[sizeof...(C)] AVR_PROGMEM = {C...};
};

template <char... C>
constexpr char const ProgmemString<C...>::data_[sizeof...(C)] AVR_PROGMEM;

// Get the Nth char from a string of length M.
template <int N, int M>
constexpr char GetNthCharOfM(char const (&c)[M]) {
  return c[N < M ? N : M - 1];
}

// Type deduction related templates from typestring.hh. They have the effect of
// determining the length of the string literal by detecting the NUL that
// terminates the literal. This means that they would not work with a literal
// that has an embedded NUL in it.

template <char... X>
auto typoke(ProgmemString<X...>)  // as is...
    -> ProgmemString<X...>;

template <char... X, char... Y>
auto typoke(ProgmemString<X...>, ProgmemString<'\0'>, ProgmemString<Y>...)
    -> ProgmemString<X...>;

template <char A, char... X, char... Y>
auto typoke(ProgmemString<X...>, ProgmemString<A>, ProgmemString<Y>...)
    -> decltype(typoke(ProgmemString<X..., A>(), ProgmemString<Y>()...));

template <char... C>
auto typeek(ProgmemString<C...>) -> decltype(typoke(ProgmemString<C>()...));

}  // namespace progmem_data
}  // namespace alpaca

#define TASLIT1(n, x) alpaca::progmem_data::GetNthCharOfM<0x##n##0>(x)

/* 2^1 = 2 */
#define TASLIT2(n, x)                               \
  alpaca::progmem_data::GetNthCharOfM<0x##n##0>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##1>(x)

/* 2^2 = 2 */
#define TASLIT4(n, x)                                   \
  alpaca::progmem_data::GetNthCharOfM<0x##n##0>(x),     \
      alpaca::progmem_data::GetNthCharOfM<0x##n##1>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##2>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##3>(x)

/* 2^3 = 8 */
#define TASLIT8(n, x)                                   \
  alpaca::progmem_data::GetNthCharOfM<0x##n##0>(x),     \
      alpaca::progmem_data::GetNthCharOfM<0x##n##1>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##2>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##3>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##4>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##5>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##6>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##7>(x)

/* 2^4 = 16 */
#define TASLIT16(n, x)                                  \
  alpaca::progmem_data::GetNthCharOfM<0x##n##0>(x),     \
      alpaca::progmem_data::GetNthCharOfM<0x##n##1>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##2>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##3>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##4>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##5>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##6>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##7>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##8>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##9>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##A>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##B>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##C>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##D>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##E>(x), \
      alpaca::progmem_data::GetNthCharOfM<0x##n##F>(x)

/* 2^5 = 32 */
#define TASLIT32(n, x) TASLIT16(n##0, x), TASLIT16(n##1, x)

/* 2^6 = 64 */
#define TASLIT64(n, x) \
  TASLIT16(n##0, x), TASLIT16(n##1, x), TASLIT16(n##2, x), TASLIT16(n##3, x)

// Not yet working. Sigh.
#define TASLIT(x)                           \
  (decltype(::alpaca::progmem_data::typeek( \
      ::alpaca::progmem_data::ProgmemString<TASLIT64(, x)>()))::MakeLiteral())

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_
