#include "utils/any_printable.h"

#include <string>

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace {

#ifdef TEST_TEMPORARY_PRINTABLE
// For testing that we can't pass temporary Printable instances into an
// AnyPrintable constructor.
class SomePrintable : public Printable {
 public:
  size_t printTo(Print& out) const override { return 0; }
};

TEST(AnyPrintableTest, PrintableNegativeCompilation) {
  // None of these lines should compile.
  auto a = AnyPrintable{SomePrintable()};
  auto b = AnyPrintable(SomePrintable());
  AnyPrintable c = SomePrintable();
  AnyPrintable d((SomePrintable()));
}
#endif

std::string AnyPrintableToString(const AnyPrintable& any_printable) {
  PrintToStdString out;
  const size_t count = out.print(any_printable);
  const std::string result = out.str();
  EXPECT_EQ(count, result.size());
  // Test copying.
  {
    AnyPrintable copy = any_printable;
    PrintToStdString out2;
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

// This provides coverage of various AnyString, Literal and StringView methods.
template <int N>
void VerifyStringLiteralPrinting(const char (&buf)[N]) {
  const std::string expected(buf);
  EXPECT_EQ(expected.size(), N - 1);

  {
    Literal value(buf);
    EXPECT_EQ(value.size(), expected.size());
    EXPECT_EQ(AnyPrintableToString(value), expected);
    AnyString any_string(value);
    EXPECT_EQ(any_string.size(), expected.size());
    EXPECT_EQ(AnyPrintableToString(any_string), expected);
  }

  {
    StringView value(buf);
    EXPECT_EQ(value.size(), expected.size());
    EXPECT_EQ(AnyPrintableToString(value), expected);
    AnyString any_string(value);
    EXPECT_EQ(any_string.size(), expected.size());
    EXPECT_EQ(AnyPrintableToString(any_string), expected);
  }
}

TEST(AnyPrintableTest, StringLiterals) {
  VerifyStringLiteralPrinting("");
  VerifyStringLiteralPrinting(" ");
  VerifyStringLiteralPrinting("some literal text");
}

TEST(AnyPrintableTest, PrintableReference) {
  constexpr char kStr[] = "some more text";
  StringView view(kStr);
  AnyString any_string(view);
  Printable& printable = any_string;
  AnyPrintable any_printable(printable);
  EXPECT_EQ(AnyPrintableToString(any_printable), kStr);
}

TEST(AnyPrintableTest, ConstPrintableReference) {
  constexpr char kStr[] = "some other text";
  StringView view(kStr);
  const AnyString any_string(view);
  const Printable& printable = any_string;
  AnyPrintable any_printable(printable);
  EXPECT_EQ(AnyPrintableToString(any_printable), kStr);
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

  float f = 3.14f;
  EXPECT_EQ(PrintRefViaAnyPrintable(f), std::to_string(f));
  EXPECT_EQ(PrintValueViaAnyPrintable(f), std::to_string(f));
  EXPECT_EQ(PrintValueViaAnyPrintable(3.14f), std::to_string(f));

  double d = 3.14;
  EXPECT_EQ(PrintRefViaAnyPrintable(d), std::to_string(d));
  EXPECT_EQ(PrintValueViaAnyPrintable(d), std::to_string(d));
  EXPECT_EQ(PrintValueViaAnyPrintable(3.14), std::to_string(d));

  StringView some_text("some_text");
  EXPECT_EQ(PrintRefViaAnyPrintable(some_text), "some_text");
  EXPECT_EQ(PrintValueViaAnyPrintable(some_text), "some_text");
  EXPECT_EQ(PrintValueViaAnyPrintable(AnyString(some_text)), "some_text");
}

}  // namespace
}  // namespace alpaca
