// An experiment based on inline_literal2_test.cc, seeing if we can count the
// size of the string, and thus determine whether the macro used handled the
// expansion into characters appropriately.

#include <string_view>

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gtest.h"
#include "utils/literal.h"
#include "utils/o_print_stream.h"
#include "utils/progmem_string_view.h"

namespace alpaca {
namespace progmem_data {
////////////////////////////////////////////////////////////////////////////////

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

// LiteralStringPack is used instead of ProgmemStrData when we're
// searching for NUL or forward slash so that we don't have to worry about the
// compiler spending time examining the definition of ProgmemStrData,
// which is a slightly more complex class template than is LiteralStringPack.
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

}  // namespace progmem_data

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
#define TASLIT(x) TAS_PSV_128x)

namespace test {
namespace {
using ::alpaca::progmem_data::ProgmemStrData;

TEST(InlineLiteralTest, RawProgmemString) {
  using Type = ProgmemStrData<'H', 'E', 'L', 'L', 'O'>;
  auto printable = alpaca::progmem_data::MakeProgmemStringView<Type>();
  EXPECT_EQ(printable.size(), 5);
  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 5);
  EXPECT_EQ(out.str(), "HELLO");
}

TEST(InlineLiteralTest, TASLIT16_String) {
  using Type = ProgmemStrData<_TAS_EXPAND_16(, "Hello!")>;
  auto printable = alpaca::progmem_data::MakeProgmemStringView<Type>();
  EXPECT_EQ(printable.size(), 16);
  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 16);
  EXPECT_EQ(out.str(), std::string_view("Hello!\0\0\0\0\0\0\0\0\0\0", 16));
}

TEST(InlineLiteralTest, TasPsvToProgmemStringView) {
  ProgmemStringView progmem_string_view = TAS_PSV("Hey There");
  EXPECT_EQ(progmem_string_view.size(), 9);
  {
    PrintToStdString out;
    EXPECT_EQ(progmem_string_view.printTo(out), 9);
    EXPECT_EQ(out.str(), "Hey There");
  }
  {
    PrintToStdString out;
    OPrintStream ops(out);
    ops << progmem_string_view;
    EXPECT_EQ(out.str(), "Hey There");
  }
}

TEST(InlineLiteralTest, TasPsvToLiteral) {
  Literal literal = TAS_PSV("Hey There!");
  EXPECT_EQ(literal.size(), 10);
  PrintToStdString out;
  EXPECT_EQ(literal.printTo(out), 10);
  EXPECT_EQ(out.str(), "Hey There!");
}

TEST(InlineLiteralTest, TasFlashstr) {
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR("Echo, echo, echo, echo, echo")), 28);
  EXPECT_EQ(out.str(), "Echo, echo, echo, echo, echo");
}

TEST(InlineLiteralTest, EmptyTasFlashstr) {
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR("")), 0);
  EXPECT_EQ(out.str(), "");
}

TEST(InlineLiteralTest, LeadingNUL) {
  using Type = _TAS_PSD_TYPE_128("\0abc");
  EXPECT_EQ(1, sizeof(Type::kData));
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR_128("\0abc")), 0);
  EXPECT_EQ(out.str(), "");
}

#define STR31 "abcdefghijklmnopqrstuvwxyz01234"
#define STR32 STR31 "5"
#define STR33 STR32 "6"

#define STR63 STR32 STR31
#define STR64 STR32 STR32
#define STR65 STR32 STR33

#define STR127 STR64 STR63
#define STR128 STR64 STR64
#define STR129 STR64 STR65

#define STR255 STR128 STR127
#define STR256 STR128 STR128
#define STR257 STR128 STR129

#define STR511 STR256 STR255
#define STR512 STR256 STR256
#define STR513 STR256 STR257

#define STR1023 STR512 STR511
#define STR1024 STR512 STR512
#define STR1025 STR512 STR513

#define FITS_TEST(x, len, max)                        \
  TEST(InlineLiteralTest, Literal##len##Fits##max) {  \
    using Type = _TAS_PSD_TYPE_##max(x);              \
    EXPECT_EQ(len + 1, sizeof(Type::kData));          \
    PrintToStdString out;                             \
    EXPECT_EQ(out.print(TAS_FLASHSTR_##max(x)), len); \
    EXPECT_EQ(out.str(), x);                          \
  }

FITS_TEST(STR31, 31, 32);
FITS_TEST(STR63, 63, 64);
FITS_TEST(STR127, 127, 128);
FITS_TEST(STR255, 255, 256);
FITS_TEST(STR511, 511, 512);

// Each of the following static_asserts should fail to compile, hence each is
// in an #ifdef block that, by default, is not active.

#define OVERFLOWS_TEST(len, max)                                      \
  using Type_##len##_Overflows_##max = _TAS_PSD_TYPE_##max(STR##len); \
  static_assert(sizeof(Type_##len##_Overflows_##max) == 999999,       \
                "Should not compile.")

#ifdef STR_32_OVERFLOWS_32
OVERFLOWS_TEST(32, 32);
#endif

#ifdef STR_33_OVERFLOWS_32
OVERFLOWS_TEST(33, 32);
#endif

#ifdef STR_64_OVERFLOWS_64
OVERFLOWS_TEST(64, 64);
#endif

#ifdef STR_65_OVERFLOWS_64
OVERFLOWS_TEST(65, 64);
#endif

#ifdef STR_128_OVERFLOWS_128
OVERFLOWS_TEST(128, 128);
#endif

#ifdef STR_129_OVERFLOWS_128
OVERFLOWS_TEST(129, 128);
#endif

#ifdef STR_256_OVERFLOWS_256
OVERFLOWS_TEST(256, 256);
#endif

#ifdef STR_257_OVERFLOWS_256
OVERFLOWS_TEST(257, 256);
#endif

#ifdef STR_512_OVERFLOWS_512
OVERFLOWS_TEST(512, 512);
#endif

#ifdef STR_513_OVERFLOWS_512
OVERFLOWS_TEST(513, 512);
#endif

#ifdef STR_1024_OVERFLOWS_1024
// This will probably just exceed the compiler's ability to expand it.
OVERFLOWS_TEST(1024, 1024);
#endif

#ifdef STR_1025_OVERFLOWS_1024
// This will probably just exceed the compiler's ability to expand it.
OVERFLOWS_TEST(1025, 1024);
#endif

}  // namespace
}  // namespace test
}  // namespace alpaca
