#include "utils/inline_literal.h"

#include <string_view>

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gtest.h"
#include "utils/literal.h"
#include "utils/o_print_stream.h"
#include "utils/progmem_string_view.h"

namespace alpaca {
namespace test {
namespace {
using ::alpaca::progmem_data::ProgmemStrData;

TEST(InlineLiteralTest, ExplicitProgmemStrData) {
  using Type = ProgmemStrData<'H', 'E', 'L', 'L', 'O'>;
  auto printable = alpaca::progmem_data::MakeProgmemStringView<Type>();
  EXPECT_EQ(printable.size(), 5);
  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 5);
  EXPECT_EQ(out.str(), "HELLO");
}

TEST(InlineLiteralTest, EmptyProgmemStrData) {
  using Type = ProgmemStrData<>;
  auto printable = alpaca::progmem_data::MakeProgmemStringView<Type>();
  EXPECT_EQ(printable.size(), 0);
  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 0);
  EXPECT_EQ(out.str(), "");
}

TEST(InlineLiteralTest, TasExpand16) {
  // When we don't search for the NUL at the end, the ProgmemStrData is padded
  // out to the size nnn, determined by the _TAS_EXPAND_nnn used.
  using Type = ProgmemStrData<_TAS_EXPAND_16(, "Hello!")>;
  auto printable = alpaca::progmem_data::MakeProgmemStringView<Type>();
  EXPECT_EQ(printable.size(), 16);
  PrintToStdString out;
  EXPECT_EQ(printable.printTo(out), 16);
  EXPECT_EQ(out.str(), std::string_view("Hello!\0\0\0\0\0\0\0\0\0\0", 16));
}

TEST(InlineLiteralTest, PrintTasFlashstr) {
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR("Echo, echo, echo, echo, echo")), 28);
  EXPECT_EQ(out.str(), "Echo, echo, echo, echo, echo");
}

TEST(InlineLiteralTest, PrintEmptyTasFlashstr) {
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR("")), 0);
  EXPECT_EQ(out.str(), "");
}

TEST(InlineLiteralTest, StreamTasFlashstr) {
  PrintToStdString out;
  OPrintStream strm(out);
  strm << TAS_FLASHSTR("foo, Bar, BAZ");
  EXPECT_EQ(out.str(), "foo, Bar, BAZ");
}

TEST(InlineLiteralTest, LeadingNUL) {
  using Type = _TAS_PSD_TYPE_128("\0abc");
  EXPECT_EQ(1, sizeof(Type::kData));
  PrintToStdString out;
  EXPECT_EQ(out.print(TAS_FLASHSTR_128("\0abc")), 0);
  EXPECT_EQ(out.str(), "");
}

TEST(InlineLiteralTest, TasPsvPrintTo) {
  PrintToStdString out;
  EXPECT_EQ(TAS_PSV("Hey There").printTo(out), 9);
  EXPECT_EQ(out.str(), "Hey There");
  EXPECT_EQ(TAS_PSV("Hey There").size(), 9);
}

TEST(InlineLiteralTest, StreamTasPsv) {
  PrintToStdString out;
  OPrintStream strm(out);
  strm << TAS_PSV("Hey There");
  EXPECT_EQ(out.str(), "Hey There");
}

TEST(InlineLiteralTest, TasPsvToProgmemStringView) {
  ProgmemStringView progmem_string_view = TAS_PSV("Hey There");
  EXPECT_EQ(progmem_string_view.size(), 9);
  PrintToStdString out;
  EXPECT_EQ(progmem_string_view.printTo(out), 9);
  EXPECT_EQ(out.str(), "Hey There");
}

TEST(InlineLiteralTest, TasPsvToLiteral) {
  Literal literal = TAS_PSV("Hey There!");
  EXPECT_EQ(literal.size(), 10);
  PrintToStdString out;
  EXPECT_EQ(literal.printTo(out), 10);
  EXPECT_EQ(out.str(), "Hey There!");
}

TEST(InlineLiteralTest, StreamTasLit) {
  PrintToStdString out;
  OPrintStream strm(out);
  strm << TASLIT("Echo, echo, etc");
  EXPECT_EQ(out.str(), "Echo, echo, etc");
}

////////////////////////////////////////////////////////////////////////////////
// Tests of the upper limits of string literal length.

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
