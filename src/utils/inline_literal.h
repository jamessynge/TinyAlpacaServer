#ifndef TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_

// Provides a macro, TASLIT(x), that expands to a Literal instance with the
// string x as the value, which in the case of the avr-gcc compiler is stored in
// program memory (PROGMEM) rather than RAM.
//
// This is based on https://github.com/irrequietus/typestring, by George
// Makrydakis <george@irrequietus.eu>. Makrydakis's typestring.hh expands a
// string literal passed to a macro function into a list of char values, thus
// enabling a string literal to be (indirectly) used as a template parameter,
// i.e. by turning it into a list of chars used as the arguments of a variadic
// template.
//
// Since the goal here is to add this capability to this library where Literal
// already supports reading from PROGMEM, this isn't just a copy of
// typestring.hh. Instead, it contains an adaptation of Makrydakis's approach
// to this context.

#include "utils/platform.h"
#include "utils/printable_progmem_string.h"

namespace alpaca {

namespace progmem_data {
template <char... C>
class ProgmemStringStorage final {
 public:
  static PrintableProgmemString MakePrintable() {
    return PrintableProgmemString(data_, sizeof...(C));
  }

 private:
  // NOTE: there is no trailing NUL here because we are using this to make a
  // PrintableProgmemString instance, and we know exactly the size of the
  // literal string (i.e. without the NUL), so we can pass that size to the
  // PrintableProgmemString ctor.
  static constexpr char const data_[sizeof...(C)] AVR_PROGMEM = {C...};
};

template <char... C>
constexpr char const
    ProgmemStringStorage<C...>::data_[sizeof...(C)] AVR_PROGMEM;

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
auto typoke(ProgmemStringStorage<X...>)  // as is...
    -> ProgmemStringStorage<X...>;

template <char... X, char... Y>
auto typoke(ProgmemStringStorage<X...>, ProgmemStringStorage<'\0'>,
            ProgmemStringStorage<Y>...) -> ProgmemStringStorage<X...>;

template <char A, char... X, char... Y>
auto typoke(ProgmemStringStorage<X...>, ProgmemStringStorage<A>,
            ProgmemStringStorage<Y>...)
    -> decltype(typoke(ProgmemStringStorage<X..., A>(),
                       ProgmemStringStorage<Y>()...));

template <char... C>
auto typeek(ProgmemStringStorage<C...>)
    -> decltype(typoke(ProgmemStringStorage<C>()...));

}  // namespace progmem_data
}  // namespace alpaca

#define TASLIT_N_M(n, m, x) alpaca::progmem_data::GetNthCharOfM<0x##n##m>(x)

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

#define TASLIT(x)                                             \
  (decltype(::alpaca::progmem_data::typeek(                   \
      ::alpaca::progmem_data::ProgmemStringStorage<TASLIT128( \
          , x)>()))::MakePrintable())

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_