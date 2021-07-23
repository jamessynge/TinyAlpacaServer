// A refinement of inline_literal.h and its test, which better documents the
// template functions and the type deduction applied to them.

#include <string_view>

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gtest.h"
#include "utils/inline_literal.h"

namespace alpaca {
namespace progmem_data {
////////////////////////////////////////////////////////////////////////////////

// LiteralStringPack is used instead of ProgmemStringStorage when we're
// searching for NUL or forward slash so that we don't have to worry about the
// compiler spending time examining the definition of ProgmemStringStorage,
// which is a slightly more complex class template than is LiteralStringPack.
template <char... C>
struct LiteralStringPack final {};

// When KeepBeforeNUL has a single parameter, the return type has the same type
// as that parameter. This is the case once we've used the specializations of
// KeepBeforeNUL to identify the location of the NUL and keep only the
// characters before that.
template <char... X>
auto KeepBeforeNUL(LiteralStringPack<X...>)  // as is...
    -> LiteralStringPack<X...>;

// The second and subsequent args are of type LiteralStringPack<C>, where C is
// a char value. If the second arg is of type LiteralStringPack<'\0'>, then
// we've located the NUL of the literal string, and can discard it and the
// subsequent args; the type of the result is LiteralStringPack<X...>, where
// X... represents the literal character values of the literal string. At this
// point no more substitution of deduced template arguments for KeepBeforeNUL is
// performed, and we now know the return type of ExpandLiteralKeepBeforeNUL.
template <char... X, char... Y>
auto KeepBeforeNUL(LiteralStringPack<X...>, LiteralStringPack<'\0'>,
                   LiteralStringPack<Y>...) -> LiteralStringPack<X...>;

// For any character A other than '\0', append it to the template parameter pack
// X, then consider the characters following A.
template <char A, char... X, char... Y>
auto KeepBeforeNUL(LiteralStringPack<X...>, LiteralStringPack<A>,
                   LiteralStringPack<Y>...)
    -> decltype(KeepBeforeNUL(LiteralStringPack<X..., A>(),
                              LiteralStringPack<Y>()...));

// Entry point for finding the NUL that terminates the string literal.
template <char... C>
auto ExpandLiteralKeepBeforeNUL(LiteralStringPack<C...>)
    -> decltype(KeepBeforeNUL(LiteralStringPack<C>()...));

// Specialization for empty strings. In theory we shouldn't need these, but in
// practice it avoids problems.
template <char... C>
auto ExpandLiteralKeepBeforeNUL(LiteralStringPack<'\0', C...>)
    -> LiteralStringPack<>;

// Used to switch
template <char... C>
auto ProvideStorage(LiteralStringPack<C...>) -> ProgmemStringStorage<C...>;

// Max length 64

#define _TAS_KEEP_LITERAL_BEFORE_NUL_64(x)                     \
  decltype(::alpaca::progmem_data::ExpandLiteralKeepBeforeNUL( \
      ::alpaca::progmem_data::LiteralStringPack<_TAS_EXPAND_64(, x)>()))

#define _TASLIT_TYPE_64(x)                         \
  decltype(::alpaca::progmem_data::ProvideStorage( \
      _TAS_KEEP_LITERAL_BEFORE_NUL_64(x)()))

#define TAS_FLASHSTR2_64(x) \
  (reinterpret_cast<const __FlashStringHelper*>(_TASLIT_TYPE_64(x)::kData))

// Max length 128

#define _TAS_KEEP_LITERAL_BEFORE_NUL_128(x)                    \
  decltype(::alpaca::progmem_data::ExpandLiteralKeepBeforeNUL( \
      ::alpaca::progmem_data::LiteralStringPack<_TAS_EXPAND_128(, x)>()))

#define _TASLIT_TYPE_128(x)                        \
  decltype(::alpaca::progmem_data::ProvideStorage( \
      _TAS_KEEP_LITERAL_BEFORE_NUL_128(x)()))

#define TAS_FLASHSTR2_128(x) \
  (reinterpret_cast<const __FlashStringHelper*>(_TASLIT_TYPE_128(x)::kData))

// Max length 256

#define _TAS_KEEP_LITERAL_BEFORE_NUL_256(x)                    \
  decltype(::alpaca::progmem_data::ExpandLiteralKeepBeforeNUL( \
      ::alpaca::progmem_data::LiteralStringPack<_TAS_EXPAND_256(, x)>()))

#define _TASLIT_TYPE_256(x)                        \
  decltype(::alpaca::progmem_data::ProvideStorage( \
      _TAS_KEEP_LITERAL_BEFORE_NUL_256(x)()))

#define TAS_FLASHSTR2_256(x) \
  (reinterpret_cast<const __FlashStringHelper*>(_TASLIT_TYPE_256(x)::kData))

// Max length 512

#define _TAS_KEEP_LITERAL_BEFORE_NUL_512(x)                    \
  decltype(::alpaca::progmem_data::ExpandLiteralKeepBeforeNUL( \
      ::alpaca::progmem_data::LiteralStringPack<_TAS_EXPAND_512(, x)>()))

#define _TASLIT_TYPE_512(x)                        \
  decltype(::alpaca::progmem_data::ProvideStorage( \
      _TAS_KEEP_LITERAL_BEFORE_NUL_512(x)()))

#define TAS_FLASHSTR2_512(x) \
  (reinterpret_cast<const __FlashStringHelper*>(_TASLIT_TYPE_512(x)::kData))

// Max length 1024

#define _TAS_KEEP_LITERAL_BEFORE_NUL_1024(x)                   \
  decltype(::alpaca::progmem_data::ExpandLiteralKeepBeforeNUL( \
      ::alpaca::progmem_data::LiteralStringPack<_TAS_EXPAND_1024(, x)>()))

#define _TASLIT_TYPE_1024(x)                       \
  decltype(::alpaca::progmem_data::ProvideStorage( \
      _TAS_KEEP_LITERAL_BEFORE_NUL_1024(x)()))

#define TAS_FLASHSTR2_1024(x) \
  (reinterpret_cast<const __FlashStringHelper*>(_TASLIT_TYPE_1024(x)::kData))

}  // namespace progmem_data

namespace test {
namespace {
using ::alpaca::progmem_data::ProgmemStringStorage;

TEST(InlineLiteralTest, RawProgmemString) {
  using Type = ProgmemStringStorage<'H', 'E', 'L', 'L', 'O'>;
  auto printable = alpaca::progmem_data::MakePrintable<Type>();
  EXPECT_EQ(printable.size(), 5);
  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 5);
  EXPECT_EQ(out.str(), "HELLO");
}

TEST(InlineLiteralTest, TASLIT16_String) {
  using Type = ProgmemStringStorage<_TAS_EXPAND_16(, "Hello!")>;
  auto printable = alpaca::progmem_data::MakePrintable<Type>();
  EXPECT_EQ(printable.size(), 16);
  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 16);
  EXPECT_EQ(out.str(), std::string_view("Hello!\0\0\0\0\0\0\0\0\0\0", 16));
}

TEST(InlineLiteralTest, TasLit) {
  auto printable = TASLIT("Hey There!");
  EXPECT_EQ(printable.size(), 10);
  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 10);
  EXPECT_EQ(out.str(), "Hey There!");
}

TEST(InlineLiteralTest, TasFlashstr) {
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR2_128("Echo, echo, echo, echo, echo")), 28);
  EXPECT_EQ(out.str(), "Echo, echo, echo, echo, echo");
}

TEST(InlineLiteralTest, EmptyTasFlashstr) {
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR2_128("")), 0);
  EXPECT_EQ(out.str(), "");
}

TEST(InlineLiteralTest, LeadingNUL) {
  using Type = _TASLIT_TYPE_128("\0abc");
  EXPECT_EQ(1, sizeof(Type::kData));
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR2_128("\0abc")), 0);
  EXPECT_EQ(out.str(), "");
}

#define STR32 "abcdefghijklmnopqrstuvwxyz012345"
#define STR64 STR32 STR32
#define STR128 STR64 STR64
#define STR256 STR128 STR128
#define STR512 STR256 STR256
#define STR1024 STR512 STR512

TEST(InlineLiteralTest, LongLiteral64) {
  using Type = _TASLIT_TYPE_64(STR64);
  EXPECT_EQ(65, sizeof(Type::kData));
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR2_64(STR64)), 64);
  EXPECT_EQ(out.str(), STR64);
}

TEST(InlineLiteralTest, LongLiteral128) {
  using Type = _TASLIT_TYPE_128(STR128);
  EXPECT_EQ(129, sizeof(Type::kData));
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR2_128(STR128)), 128);
  EXPECT_EQ(out.str(), STR128);
}

TEST(InlineLiteralTest, LongLiteral256) {
  using Type = _TASLIT_TYPE_256(STR256);
  EXPECT_EQ(257, sizeof(Type::kData));
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR2_256(STR256)), 256);
  EXPECT_EQ(out.str(), STR256);
}

TEST(InlineLiteralTest, LongLiteral512) {
  using Type = _TASLIT_TYPE_512(STR512);
  EXPECT_EQ(513, sizeof(Type::kData));
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR2_512(STR512)), 512);
  EXPECT_EQ(out.str(), STR512);
}

// 1024 is too long, explodes the stack of the clang compiler on a very large
// memory workstation.
//
// TEST(InlineLiteralTest, LongLiteral1024) {
//   using Type = _TASLIT_TYPE_1024(STR1024);
//   EXPECT_EQ(1025, sizeof(Type::kData));
//   PrintToStdString out;
//   EXPECT_EQ(out.print(TAS_FLASHSTR2_1024(STR1024)), 1024);
//   EXPECT_EQ(out.str(), STR1024);
// }

}  // namespace
}  // namespace test
}  // namespace alpaca
