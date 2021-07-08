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
// Author: james.synge@gmail.com

#include "utils/platform.h"
#include "utils/printable_progmem_string.h"  // IWYU pragma: export

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
PrintableProgmemString MakePrintable() {
  return PrintableProgmemString(PSS::kData, (sizeof PSS::kData) - 1);
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
// trailing NUL is returned; as a result, TASLIT16(, "Hello") becomes:
//
//   'H','e','l','l','o','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
template <int N, int M>
constexpr char GetNthCharOfM(char const (&c)[M]) {
  return c[N < M ? N : M - 1];
}

}  // namespace progmem_data
}  // namespace alpaca

#define TASLIT_N_M(n, m, x) ::alpaca::progmem_data::GetNthCharOfM<0x##n##m>(x)

/* 2^4 = 16 */
#define TASLIT16(n, x)                                               \
  TASLIT_N_M(n, 0, x), TASLIT_N_M(n, 1, x), TASLIT_N_M(n, 2, x),     \
      TASLIT_N_M(n, 3, x), TASLIT_N_M(n, 4, x), TASLIT_N_M(n, 5, x), \
      TASLIT_N_M(n, 6, x), TASLIT_N_M(n, 7, x), TASLIT_N_M(n, 8, x), \
      TASLIT_N_M(n, 9, x), TASLIT_N_M(n, A, x), TASLIT_N_M(n, B, x), \
      TASLIT_N_M(n, C, x), TASLIT_N_M(n, D, x), TASLIT_N_M(n, E, x), \
      TASLIT_N_M(n, F, x)

/* 2^6 = 64 */
#define TASLIT64(n, x) \
  TASLIT16(n##0, x), TASLIT16(n##1, x), TASLIT16(n##2, x), TASLIT16(n##3, x)

/* 2^7 = 128 */
#define TASLIT128(n, x)                                                       \
  TASLIT16(n##0, x), TASLIT16(n##1, x), TASLIT16(n##2, x), TASLIT16(n##3, x), \
      TASLIT16(n##4, x), TASLIT16(n##5, x), TASLIT16(n##6, x),                \
      TASLIT16(n##7, x)

/* 2^8 = 256 */
#define TASLIT256(n, x)                                                       \
  TASLIT16(n##0, x), TASLIT16(n##1, x), TASLIT16(n##2, x), TASLIT16(n##3, x), \
      TASLIT16(n##4, x), TASLIT16(n##5, x), TASLIT16(n##6, x),                \
      TASLIT16(n##7, x), TASLIT16(n##8, x), TASLIT16(n##9, x),                \
      TASLIT16(n##A, x), TASLIT16(n##B, x), TASLIT16(n##C, x),                \
      TASLIT16(n##D, x), TASLIT16(n##E, x), TASLIT16(n##F, x)

/* 2^9 = 512 */
#define TASLIT512(n, x) TASLIT256(n##0, x), TASLIT256(n##1, x)

// If your string literals have more than 127 characters (not including the
// trailing NUL), replace TASLIT128 with TASLIT256 or TASLIT512, as necessary.

#define TASLIT(x)                              \
  (::alpaca::progmem_data::MakePrintable<      \
      decltype(::alpaca::progmem_data::typeek( \
          ::alpaca::progmem_data::ProgmemStringStorage<TASLIT128(, x)>()))>())

#define TAS_FLASHSTR(x)                                           \
  (reinterpret_cast<const __FlashStringHelper*>(                  \
      decltype(::alpaca::progmem_data::typeek(                    \
          ::alpaca::progmem_data::ProgmemStringStorage<TASLIT128( \
              , x)>()))::kData))

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_
