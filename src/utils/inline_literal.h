#ifndef TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_

// Provides macros for storing string literals in program memory (PROGMEM)
// rather than RAM, when compiled with avr-gcc for the AVR line of processors.
// The linker should be able to collapse multiple occurrences of the same string
// literal into a single array in PROGMEM; this isn't true when using the
// Arduino defined F(string_literal) macro, where every occurrence in a single
// file is stored separately.
//
// TASLIT(string_literal) expands to a Printable instance with string_literal as
// the value that is printed.
//
// TAS_FLASHSTR(string_literal) expands to a const __FlashStringHelper* pointer
// value, just as F(string_literal) does, but without the wasted storage.
//
// This is based on https://github.com/irrequietus/typestring, by George
// Makrydakis <george@irrequietus.eu>. Makrydakis's typestring.hh expands a
// string literal passed to a macro function into a list of char values, thus
// enabling a string literal to be (indirectly) used as a template parameter,
// i.e. by turning it into a list of chars used as the arguments of a variadic
// template, and then using type deduction to determine the length of the
// string.
//
// Since the goal here is to add this capability to this library where Literal
// already supports reading from PROGMEM, this isn't just a copy of
// typestring.hh. Instead, it contains an adaptation of Makrydakis's approach
// to this context.
//
// TODO(jamessynge): Come up with a way for TASLIT and TAS_FLASHSTR to fail if
// the string is too long, i.e. if the NUL terminator is not found. This will
// allow me to reduce the maximum size string that is supported by default
// (probably 32, maybe 64), and to instead require that TASLITnnn and
// TAS_FLASHSTRnnn are used when the string is known to be too long. This is
// likely to greatly improve compile times because the type deduction clearly
// has a very bad impact on the speed of compiling (at least with avr-gcc).
//
// Author: james.synge@gmail.com

#include "utils/platform.h"
#include "utils/progmem_string_view.h"  // IWYU pragma: export

namespace alpaca {
namespace progmem_data {

template <char... C>
struct ProgmemStringStorage final {
  // We add a trailing NUL here so that we can interpret kData as a
  // __FlashStringHelper instance (see TAS_FLASHSTR for how we do that);
  // Arduino's Print::print(const __FlashStringHelper*) needs the string to be
  // NUL terminated so that it knows when it has found the end of the string.
  static constexpr char const kData[sizeof...(C) + 1] AVR_PROGMEM = {C..., 0};
};

template <char... C>
constexpr char const
    ProgmemStringStorage<C...>::kData[sizeof...(C) + 1] AVR_PROGMEM;

template <class PSS>
ProgmemStringView MakeProgmemStringView() {
  return ProgmemStringView(PSS::kData, (sizeof PSS::kData) - 1);
}

// Type deduction related templates from typestring.hh. They have the effect of
// determining the length of the string literal by detecting the NUL that
// terminates the literal. This means that they would not work with a literal
// that has an embedded NUL in it.

// When typoke has a single parameter, the return type has the same type as that
// parameter.
template <char... X>
auto typoke(ProgmemStringStorage<X...>)  // as is...
    -> ProgmemStringStorage<X...>;

// Keep only the characters before the NUL.
template <char... X, char... Y>
auto typoke(ProgmemStringStorage<X...>, ProgmemStringStorage<'\0'>,
            ProgmemStringStorage<Y>...) -> ProgmemStringStorage<X...>;

// For any other character A, append it to the template parameter pack X, then
// consider what to do with the characters following A.
template <char A, char... X, char... Y>
auto typoke(ProgmemStringStorage<X...>, ProgmemStringStorage<A>,
            ProgmemStringStorage<Y>...)
    -> decltype(typoke(ProgmemStringStorage<X..., A>(),
                       ProgmemStringStorage<Y>()...));

// typeek is the "entry point"; it is used to split the output of TASLITnnn into
// nnn separate chars, and to then use typoke to find the rightmost '/' and the
// leftmost NUL.
template <char... C>
auto typeek(ProgmemStringStorage<C...>)
    -> decltype(typoke(ProgmemStringStorage<C>()...));

// Get the Nth char from a string literal of length M, where that length
// includes the trailing NUL at index M-1. This is used to produce the comma
// separated lists of chars that make up a literal string. If N is > M, the
// trailing NUL is returned; as a result, _TAS_EXPAND_16(, "Hello") becomes:
//
//   'H','e','l','l','o','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
template <int N, int M>
constexpr char GetNthCharOfM(char const (&c)[M]) {
  return c[N < M ? N : M - 1];
}

}  // namespace progmem_data
}  // namespace alpaca

#define _TAS_GET_NTH_CHAR(n, x) ::alpaca::progmem_data::GetNthCharOfM<0x##n>(x)

/* 2^4 = 16 */
#define _TAS_EXPAND_16(n, x)                                  \
  _TAS_GET_NTH_CHAR(n##0, x), _TAS_GET_NTH_CHAR(n##1, x),     \
      _TAS_GET_NTH_CHAR(n##2, x), _TAS_GET_NTH_CHAR(n##3, x), \
      _TAS_GET_NTH_CHAR(n##4, x), _TAS_GET_NTH_CHAR(n##5, x), \
      _TAS_GET_NTH_CHAR(n##6, x), _TAS_GET_NTH_CHAR(n##7, x), \
      _TAS_GET_NTH_CHAR(n##8, x), _TAS_GET_NTH_CHAR(n##9, x), \
      _TAS_GET_NTH_CHAR(n##A, x), _TAS_GET_NTH_CHAR(n##B, x), \
      _TAS_GET_NTH_CHAR(n##C, x), _TAS_GET_NTH_CHAR(n##D, x), \
      _TAS_GET_NTH_CHAR(n##E, x), _TAS_GET_NTH_CHAR(n##F, x)

/* 2^6 = 64 */
#define _TAS_EXPAND_64(n, x)                                                 \
  _TAS_EXPAND_16(n##0, x), _TAS_EXPAND_16(n##1, x), _TAS_EXPAND_16(n##2, x), \
      _TAS_EXPAND_16(n##3, x)

/* 2^7 = 128 */
#define _TAS_EXPAND_128(n, x)                                                \
  _TAS_EXPAND_16(n##0, x), _TAS_EXPAND_16(n##1, x), _TAS_EXPAND_16(n##2, x), \
      _TAS_EXPAND_16(n##3, x), _TAS_EXPAND_16(n##4, x),                      \
      _TAS_EXPAND_16(n##5, x), _TAS_EXPAND_16(n##6, x),                      \
      _TAS_EXPAND_16(n##7, x)

/* 2^8 = 256 */
#define _TAS_EXPAND_256(n, x)                                                \
  _TAS_EXPAND_16(n##0, x), _TAS_EXPAND_16(n##1, x), _TAS_EXPAND_16(n##2, x), \
      _TAS_EXPAND_16(n##3, x), _TAS_EXPAND_16(n##4, x),                      \
      _TAS_EXPAND_16(n##5, x), _TAS_EXPAND_16(n##6, x),                      \
      _TAS_EXPAND_16(n##7, x), _TAS_EXPAND_16(n##8, x),                      \
      _TAS_EXPAND_16(n##9, x), _TAS_EXPAND_16(n##A, x),                      \
      _TAS_EXPAND_16(n##B, x), _TAS_EXPAND_16(n##C, x),                      \
      _TAS_EXPAND_16(n##D, x), _TAS_EXPAND_16(n##E, x),                      \
      _TAS_EXPAND_16(n##F, x)

/* 2^9 = 512 */
#define _TAS_EXPAND_512(n, x) _TAS_EXPAND_256(n##0, x), _TAS_EXPAND_256(n##1, x)

/* 2^10 = 1024 */
// WARNING: 1024 is too long in practice, clang gives up on my workstation,
// avr-gcc probably crashes my laptop.
#define _TAS_EXPAND_1024(n, x)                        \
  _TAS_EXPAND_256(n##0, x), _TAS_EXPAND_256(n##1, x), \
      _TAS_EXPAND_256(n##2, x), _TAS_EXPAND_256(n##4, x)

// If your string literals have more than 128 characters (not including the
// trailing NUL), replace _TAS_EXPAND_128 with _TAS_EXPAND_256 or
// _TAS_EXPAND_512, as necessary.

#define TASLIT(x)                                                       \
  (::alpaca::progmem_data::MakeProgmemStringView<                       \
      decltype(::alpaca::progmem_data::typeek(                          \
          ::alpaca::progmem_data::ProgmemStringStorage<_TAS_EXPAND_128( \
              , x)>()))>())

#define TAS_FLASHSTR(x)                                                 \
  (reinterpret_cast<const __FlashStringHelper*>(                        \
      decltype(::alpaca::progmem_data::typeek(                          \
          ::alpaca::progmem_data::ProgmemStringStorage<_TAS_EXPAND_128( \
              , x)>()))::kData))

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_
