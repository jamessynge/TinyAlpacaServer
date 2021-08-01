#include "utils/any_printable.h"

#include <stddef.h>
#include <stdint.h>

#include <cstring>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mcucore/extrashost/arduino/print.h"
#include "mcucore/extrastest_tools/print_to_std_string.h"
#include "utils/inline_literal.h"
#include "utils/literal.h"
#include "utils/string_view.h"

namespace alpaca {
namespace test {
namespace {

#ifdef TEST_TEMPORARY_PRINTABLE
// For testing that we can't pass temporary Printable instances into an
// AnyPrintable constructor.
TEST(AnyPrintableTest, PrintableNegativeCompilation) {
  // None of these lines should compile.
  auto a = AnyPrintable{SamplePrintable()};
  auto b = AnyPrintable(SamplePrintable());
  AnyPrintable c = SamplePrintable();
  AnyPrintable d((SamplePrintable()));
}
#endif

std::string AnyPrintableToString(const AnyPrintable& any_printable) {
  mcucore::test::PrintToStdString out;
  const size_t count = out.print(any_printable);
  const std::string result = out.str();
  EXPECT_EQ(count, result.size());
  // Test copying.
  {
    AnyPrintable copy = any_printable;
    mcucore::test::PrintToStdString out2;
    EXPECT_EQ(out2.print(copy), count);
    EXPECT_EQ(out2.str(), result);
  }
  return result;
}

template <typename T>
std::string PrintConstRefViaAnyPrintable(const T& t) {
  AnyPrintable any_printable(t);
  return AnyPrintableToString(any_printable);
}

template <typename T>
std::string PrintRefViaAnyPrintable(T& t) {
  AnyPrintable any_printable(t);
  const auto result = AnyPrintableToString(any_printable);
  // We have a ref to the value, so we can test AnyPrintable's ctor can accept
  // a const ref to that value.
  EXPECT_EQ(PrintConstRefViaAnyPrintable(t), result);
  return result;
}

template <typename T>
std::string PrintValueViaAnyPrintable(T t) {
  AnyPrintable any_printable(t);
  const auto result = AnyPrintableToString(any_printable);
  // We have a copy of the value, so we can test AnyPrintable's ctor can accept
  // a ref to that value.
  EXPECT_EQ(PrintRefViaAnyPrintable(t), result);
  return result;
}

TEST(AnyPrintableTest, Empty) {
  AnyPrintable any_printable;
  EXPECT_THAT(AnyPrintableToString(any_printable), testing::IsEmpty());
}

// This provides coverage of various Literal and StringView methods.
template <int N>
void VerifyStringLiteralPrinting(const char (&buf)[N]) {
  const std::string expected(buf);
  EXPECT_EQ(expected.size(), N - 1);

  {
    Literal value(buf);
    EXPECT_EQ(value.size(), expected.size());
    EXPECT_EQ(AnyPrintableToString(value), expected);
  }

  {
    StringView value(buf);
    EXPECT_EQ(value.size(), expected.size());
    EXPECT_EQ(AnyPrintableToString(value), expected);
  }
}

TEST(AnyPrintableTest, StringLiterals) {
  VerifyStringLiteralPrinting("");
  VerifyStringLiteralPrinting(" ");
  VerifyStringLiteralPrinting("some literal text");
}

void VerifyFlashStringPrinting(const __FlashStringHelper* str,
                               const std::string expected) {
  EXPECT_EQ(std::strlen(reinterpret_cast<const char*>(str)), expected.size());
  EXPECT_EQ(PrintValueViaAnyPrintable(str), expected);
}

#define VERIFY_FLASH_STRING_PRINTING(string_literal)      \
  VerifyFlashStringPrinting(TAS_FLASHSTR(string_literal), \
                            std::string(string_literal))

TEST(AnyPrintableTest, FlashStrings) {
  VERIFY_FLASH_STRING_PRINTING("");
  VERIFY_FLASH_STRING_PRINTING(" ");
  VERIFY_FLASH_STRING_PRINTING("some literal text");
}

TEST(AnyPrintableTest, PrintableReference) {
  constexpr char kStr[] = "some more text";
  StringView view(kStr);
  AnyPrintable printable1(view);
  Printable& printable2 = printable1;
  AnyPrintable printable3(printable2);
  EXPECT_EQ(AnyPrintableToString(printable3), kStr);
}

TEST(AnyPrintableTest, ConstPrintableReference) {
  constexpr char kStr[] = "some other text";
  const StringView view(kStr);
  const AnyPrintable printable1(view);
  const Printable& printable2 = printable1;
  const AnyPrintable printable3(printable2);
  EXPECT_EQ(AnyPrintableToString(printable3), kStr);
}

TEST(AnyPrintableTest, ManyTypes) {
  char c = '*';
  EXPECT_EQ(PrintRefViaAnyPrintable(c), "*");
  EXPECT_EQ(PrintValueViaAnyPrintable(c), "*");
  EXPECT_EQ(PrintValueViaAnyPrintable<char>(' '), " ");
  EXPECT_EQ(PrintValueViaAnyPrintable('&'), "&");

  EXPECT_EQ(PrintValueViaAnyPrintable<int32_t>(123), "123");
  EXPECT_EQ(PrintValueViaAnyPrintable(123), "123");

  int32_t i32 = -345;
  EXPECT_EQ(PrintRefViaAnyPrintable(i32), "-345");
  EXPECT_EQ(PrintValueViaAnyPrintable(i32), "-345");
  EXPECT_EQ(PrintValueViaAnyPrintable(-345), "-345");

  uint32_t ui32 = 12345678;
  EXPECT_EQ(PrintRefViaAnyPrintable(ui32), "12345678");
  EXPECT_EQ(PrintValueViaAnyPrintable(ui32), "12345678");
  EXPECT_EQ(PrintValueViaAnyPrintable(1234U), "1234");

  float f = 3.1415f;
  EXPECT_EQ(PrintRefViaAnyPrintable(f), "3.14");
  EXPECT_EQ(PrintValueViaAnyPrintable(f), "3.14");
  EXPECT_EQ(PrintValueViaAnyPrintable(3.14f), "3.14");

  double d = 2.71828;
  EXPECT_EQ(PrintRefViaAnyPrintable(d), "2.72");
  EXPECT_EQ(PrintValueViaAnyPrintable(d), "2.72");
  EXPECT_EQ(PrintValueViaAnyPrintable(2.71828), "2.72");

  StringView some_text("some_text");
  EXPECT_EQ(PrintRefViaAnyPrintable(some_text), "some_text");
  EXPECT_EQ(PrintValueViaAnyPrintable(some_text), "some_text");
  EXPECT_EQ(PrintValueViaAnyPrintable(AnyPrintable(some_text)), "some_text");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
