#ifndef TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_

// Provides macros for storing string literals in program memory (PROGMEM)
// rather than RAM, when compiled with avr-gcc for the AVR line of processors.
// The linker should be able to collapse multiple occurrences of the same string
// literal into a single array in PROGMEM; this isn't true when using the
// Arduino defined F(string_literal) macro, where every occurrence in a single
// file is stored separately.
//
// TASLIT(string_literal) expands to a ProgmemStringView instance with
// string_literal as the value it views.
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
// Since the goal here is to add this capability to this library where
// ProgmemStringView already supports reading from PROGMEM, this isn't just a
// copy of typestring.hh. Instead, it contains an adaptation of Makrydakis's
// approach to this context. Furthermore, these macros will fail to compile if
// the string literal is too long.
//
// Author: james.synge@gmail.com

#include "utils/platform.h"
#include "utils/progmem_string_view.h"  // IWYU pragma: export

namespace alpaca {
namespace progmem_data {

// Instantiations of this template provide static, constexpr storage for string
// literals. By placing them in the ::alpaca::progmem_data namespace, the linker
// will combine multiple occurrences of the same TAS_FLASHSTR(string_literal)
// across multiple files such that they share the storage.
template <char... C>
struct ProgmemStrData final {
  // We add a trailing NUL here so that we can interpret kData as a
  // __FlashStringHelper instance (see TAS_FLASHSTR for how we do that);
  // Arduino's Print::print(const __FlashStringHelper*) needs the string to be
  // NUL terminated so that it knows when it has found the end of the string.
  static constexpr char const kData[sizeof...(C) + 1] AVR_PROGMEM = {C..., 0};
};

template <char... C>
constexpr char const ProgmemStrData<C...>::kData[sizeof...(C) + 1] AVR_PROGMEM;

template <class PSS>
ProgmemStringView MakeProgmemStringView() {
  return ProgmemStringView(PSS::kData, (sizeof PSS::kData) - 1);
}

// "Forward" declaration of an undefined type. If this appears in a compiler
// error message for the expansion of TAS_FLASHSTR_nnn, or a related macro, it
// means that the string literal is too long (>= nnn).
class StringLiteralIsTooLong;

// LiteralStringPack is used instead of ProgmemStrData when we're searching for
// the terminating NUL so that we don't have to worry about the compiler
// spending time examining the definition of ProgmemStrData, which is a slightly
// more complex class template than is LiteralStringPack. Furthermore, the first
// template parameter is a bool indicating whether the terminating NUL has been
// located. This helps us generate a compile error if the string is longer than
// the template parameter pack expansion allowed for.
template <bool T, char... C>
struct LiteralStringPack final {};

// When KeepBeforeNUL has a single parameter, the return type has the same type
// as that parameter. This is the case once we've used the specializations of
// KeepBeforeNUL to identify the location of the NUL and keep only the
// characters before that.
template <bool T, char... X>
auto KeepBeforeNUL(LiteralStringPack<T, X...>)  // as is...
    -> LiteralStringPack<T, X...>;

// The second and subsequent args are of type LiteralStringPack<C>, where C is
// a char value. If the second arg is of type LiteralStringPack<'\0'>, then
// we've located the NUL of the literal string, and can discard it and the
// subsequent args; the type of the result is LiteralStringPack<X...>, where
// X... represents the literal character values of the literal string. At this
// point no more substitution of deduced template arguments for KeepBeforeNUL is
// performed, and we now know the return type of ExpandLiteralKeepBeforeNUL.
template <char... X, char... Y>
auto KeepBeforeNUL(LiteralStringPack<false, X...>,
                   LiteralStringPack<false, '\0'>,
                   LiteralStringPack<false, Y>...)
    -> LiteralStringPack<true, X...>;

// For any character A other than '\0', append it to the template parameter pack
// X, then consider the characters following A.
template <char A, char... X, char... Y>
auto KeepBeforeNUL(LiteralStringPack<false, X...>, LiteralStringPack<false, A>,
                   LiteralStringPack<false, Y>...)
    -> decltype(KeepBeforeNUL(LiteralStringPack<false, X..., A>(),
                              LiteralStringPack<false, Y>()...));

// Entry point for finding the NUL that terminates the string literal.
template <char... C>
auto ExpandLiteralKeepBeforeNUL(LiteralStringPack<false, C...>)
    -> decltype(KeepBeforeNUL(LiteralStringPack<false, C>()...));

// Specialization for empty strings. In theory we shouldn't need these, but in
// practice it avoids problems.
template <char... C>
auto ExpandLiteralKeepBeforeNUL(LiteralStringPack<false, '\0', C...>)
    -> LiteralStringPack<true>;

// If we were able to find the NUL at the end of the string literal, then
// ProvideStorage will return a type that has a static array with the string in
// it.
template <char... C>
auto ProvideStorage(LiteralStringPack<true, C...>) -> ProgmemStrData<C...>;

// Else if the literal is too long for the expension macro used, ProvideStorage
// will return a type that isn't useful for our purposes below, and whose name
// hints at the problem.
template <char... C>
auto ProvideStorage(LiteralStringPack<false, C...>) -> StringLiteralIsTooLong;

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

////////////////////////////////////////////////////////////////////////////////
// Macros for expanding a string literal into a comma separated list of
// characters of a hard coded length. If that length is longer than the string
// literal's length, the trailing characters are all NULs.

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

/* 2^5 = 32 */
#define _TAS_EXPAND_32(n, x) _TAS_EXPAND_16(n##0, x), _TAS_EXPAND_16(n##1, x)

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
// WARNING: 1024 is too long in practice, clang gives up on a very beefy
// workstation, avr-gcc probably crashes my laptop.
#define _TAS_EXPAND_1024(n, x)                        \
  _TAS_EXPAND_256(n##0, x), _TAS_EXPAND_256(n##1, x), \
      _TAS_EXPAND_256(n##2, x), _TAS_EXPAND_256(n##4, x)

////////////////////////////////////////////////////////////////////////////////
// Macros for using type deduction to produce a unique template instantiation
// for a string literal, with storage for just the characters of the string
// literal, but not more.
//
// _TAS_KEEP_LITERAL_BEFORE_NUL_nnn expands a string literal whose length is
// less than nnn to a LiteralStringPack instantiation for that string literal.
// If the string is too long, the expansion is instead to the type
// StringLiteralIsTooLong, whose name is intended to clue the developer in about
// the problem.
//
// _TAS_PSD_TYPE_nnn expands a string literal whose length is less than nnn to a
// ProgmemStrData instantiation for that string literal.

// Max length 31 (not including trailing NUL).

#define _TAS_KEEP_LITERAL_BEFORE_NUL_32(x)                     \
  decltype(::alpaca::progmem_data::ExpandLiteralKeepBeforeNUL( \
      ::alpaca::progmem_data::LiteralStringPack<false,         \
                                                _TAS_EXPAND_32(, x)>()))

#define _TAS_PSD_TYPE_32(x)                        \
  decltype(::alpaca::progmem_data::ProvideStorage( \
      _TAS_KEEP_LITERAL_BEFORE_NUL_32(x)()))

#define TAS_PSV_32(x) \
  (::alpaca::progmem_data::MakeProgmemStringView<_TAS_PSD_TYPE_32(x)>())

#define TAS_FLASHSTR_32(x) \
  (reinterpret_cast<const __FlashStringHelper*>(_TAS_PSD_TYPE_32(x)::kData))

// Max length 63 (not including trailing NUL).

#define _TAS_KEEP_LITERAL_BEFORE_NUL_64(x)                     \
  decltype(::alpaca::progmem_data::ExpandLiteralKeepBeforeNUL( \
      ::alpaca::progmem_data::LiteralStringPack<false,         \
                                                _TAS_EXPAND_64(, x)>()))

#define _TAS_PSD_TYPE_64(x)                        \
  decltype(::alpaca::progmem_data::ProvideStorage( \
      _TAS_KEEP_LITERAL_BEFORE_NUL_64(x)()))

#define TAS_PSV_64(x) \
  (::alpaca::progmem_data::MakeProgmemStringView<_TAS_PSD_TYPE_64(x)>())

#define TAS_FLASHSTR_64(x) \
  (reinterpret_cast<const __FlashStringHelper*>(_TAS_PSD_TYPE_64(x)::kData))

// Max length 127 (not including trailing NUL).

#define _TAS_KEEP_LITERAL_BEFORE_NUL_128(x)                    \
  decltype(::alpaca::progmem_data::ExpandLiteralKeepBeforeNUL( \
      ::alpaca::progmem_data::LiteralStringPack<false,         \
                                                _TAS_EXPAND_128(, x)>()))

#define _TAS_PSD_TYPE_128(x)                       \
  decltype(::alpaca::progmem_data::ProvideStorage( \
      _TAS_KEEP_LITERAL_BEFORE_NUL_128(x)()))

#define TAS_PSV_128(x) \
  (::alpaca::progmem_data::MakeProgmemStringView<_TAS_PSD_TYPE_128(x)>())

#define TAS_FLASHSTR_128(x) \
  (reinterpret_cast<const __FlashStringHelper*>(_TAS_PSD_TYPE_128(x)::kData))

// Max length 255 (not including trailing NUL).

#define _TAS_KEEP_LITERAL_BEFORE_NUL_256(x)                    \
  decltype(::alpaca::progmem_data::ExpandLiteralKeepBeforeNUL( \
      ::alpaca::progmem_data::LiteralStringPack<false,         \
                                                _TAS_EXPAND_256(, x)>()))

#define _TAS_PSD_TYPE_256(x)                       \
  decltype(::alpaca::progmem_data::ProvideStorage( \
      _TAS_KEEP_LITERAL_BEFORE_NUL_256(x)()))

#define TAS_PSV_256(x) \
  (::alpaca::progmem_data::MakeProgmemStringView<_TAS_PSD_TYPE_256(x)>())

#define TAS_FLASHSTR_256(x) \
  (reinterpret_cast<const __FlashStringHelper*>(_TAS_PSD_TYPE_256(x)::kData))

// Max length 511 (not including trailing NUL).

#define _TAS_KEEP_LITERAL_BEFORE_NUL_512(x)                    \
  decltype(::alpaca::progmem_data::ExpandLiteralKeepBeforeNUL( \
      ::alpaca::progmem_data::LiteralStringPack<false,         \
                                                _TAS_EXPAND_512(, x)>()))

#define _TAS_PSD_TYPE_512(x)                       \
  decltype(::alpaca::progmem_data::ProvideStorage( \
      _TAS_KEEP_LITERAL_BEFORE_NUL_512(x)()))

#define TAS_PSV_512(x) \
  (::alpaca::progmem_data::MakeProgmemStringView<_TAS_PSD_TYPE_512(x)>())

#define TAS_FLASHSTR_512(x) \
  (reinterpret_cast<const __FlashStringHelper*>(_TAS_PSD_TYPE_512(x)::kData))

// Max length 1023 (not including trailing NUL).

#define _TAS_KEEP_LITERAL_BEFORE_NUL_1024(x)                   \
  decltype(::alpaca::progmem_data::ExpandLiteralKeepBeforeNUL( \
      ::alpaca::progmem_data::LiteralStringPack<false,         \
                                                _TAS_EXPAND_1024(, x)>()))

#define _TAS_PSD_TYPE_1024(x)                      \
  decltype(::alpaca::progmem_data::ProvideStorage( \
      _TAS_KEEP_LITERAL_BEFORE_NUL_1024(x)()))

#define TAS_PSV_1024(x) \
  (::alpaca::progmem_data::MakeProgmemStringView<_TAS_PSD_TYPE_1024(x)>())

#define TAS_FLASHSTR_1024(x) \
  (reinterpret_cast<const __FlashStringHelper*>(_TAS_PSD_TYPE_1024(x)::kData))

////////////////////////////////////////////////////////////////////////////////
// Because we expect almost all string literals to be less than 32 characters
// long, we define these two macros without specific lengths, and require the
// developer to use the macros whose name specifies the limit when their strings
// are longer.

#define TAS_PSV(x) TAS_PSV_64(x)
#define TAS_FLASHSTR(x) TAS_FLASHSTR_64(x)
#define TASLIT(x) TAS_PSV_128(x)

}  // namespace progmem_data
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_INLINE_LITERAL_H_
