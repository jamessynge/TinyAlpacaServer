#include "extras/host/arduino/print.h"

#include <cstdint>
#include <limits>
#include <sstream>

#include "absl/strings/str_cat.h"
#include "extras/host/arduino/wstring.h"
#include "extras/test_tools/print_to_std_string.h"
#include "extras/test_tools/sample_printable.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace {
using ::alpaca::PrintToStdString;
using ::alpaca::SamplePrintable;

template <typename T, typename U = T>
const U MaybePromote(const T value) {
  return value;
}

template <>
const int MaybePromote(const char value) {
  return value + 0;
}

template <>
const unsigned int MaybePromote(const unsigned char value) {
  return value + 0;
}

// Verify's print with one arg of type const T.
template <typename T>
void VerifyPrint1(const T value, std::string_view expected) {
  PrintToStdString out;
  out.println(value);
  out.print(value);
  EXPECT_EQ(out.str(), absl::StrCat(expected, "\n", expected))
      << "Value: " << MaybePromote(value);
}

// Verify's Print.print with two args, one of type const T, the other an int.
template <typename T>
void VerifyPrint2(const T value, int arg2, std::string_view expected) {
  PrintToStdString out;
  out.println(value, arg2);
  out.print(value, arg2);
  EXPECT_EQ(out.str(), absl::StrCat(expected, "\n", expected))
      << "Value: " << MaybePromote(value);
}

template <typename T>
void VerifyPrintHex(const T value, std::string_view expected) {
  VerifyPrint2<T>(value, HEX, expected);
}

TEST(PrintTest, Char) {
  VerifyPrint1<char>('a', "a");
  VerifyPrint1<char>('\0', std::string_view("\0", 1));
  VerifyPrint1<const char*>("abc", "abc");
}

TEST(PrintTest, FlashString) {
  VerifyPrint1<const __FlashStringHelper*>(FLASHSTR("abc"), "abc");
}

TEST(PrintTest, NumericTypes) {
  VerifyPrint1<unsigned char>(0, "0");
  VerifyPrint1<unsigned char>(127, "127");
  VerifyPrint1<unsigned char>(128, "128");
  VerifyPrint1<unsigned char>(255, "255");

  VerifyPrint1<int16_t>(-32768, "-32768");
  VerifyPrint1<int16_t>(-1, "-1");
  VerifyPrint1<int16_t>(0, "0");
  VerifyPrint1<int16_t>(1, "1");
  VerifyPrint1<int16_t>(32767, "32767");

  VerifyPrint1<uint16_t>(0, "0");
  VerifyPrint1<uint16_t>(32767, "32767");
  VerifyPrint1<uint16_t>(32768, "32768");
  VerifyPrint1<uint16_t>(65535, "65535");

  VerifyPrint1<int32_t>(-2147483648, "-2147483648");
  VerifyPrint1<int32_t>(-1, "-1");
  VerifyPrint1<int32_t>(0, "0");
  VerifyPrint1<int32_t>(1, "1");
  VerifyPrint1<int32_t>(2147483647, "2147483647");

  VerifyPrint1<uint32_t>(0, "0");
  VerifyPrint1<uint32_t>(2147483647, "2147483647");
  VerifyPrint1<uint32_t>(2147483648, "2147483648");
  VerifyPrint1<uint32_t>(4294967295, "4294967295");

  VerifyPrint1<int64_t>(std::numeric_limits<int64_t>::min(),
                        "-9223372036854775808");
  VerifyPrint1<int64_t>(-1, "-1");
  VerifyPrint1<int64_t>(0, "0");
  VerifyPrint1<int64_t>(1, "1");
  VerifyPrint1<int64_t>(std::numeric_limits<int64_t>::max(),
                        "9223372036854775807");

  VerifyPrint1<uint64_t>(0, "0");
  VerifyPrint1<uint64_t>(1, "1");
  VerifyPrint1<uint64_t>(std::numeric_limits<uint64_t>::max(),
                         "18446744073709551615");

  VerifyPrint1<float>(-1, "-1.00");
  VerifyPrint1<float>(0, "0.00");
  VerifyPrint1<float>(1, "1.00");
  VerifyPrint1<float>(3.1415, "3.14");
  VerifyPrint1<float>(0.99999, "1.00");

  VerifyPrint2<float>(-1, -1, "-1");
  VerifyPrint2<float>(-1, 0, "-1");
  VerifyPrint2<float>(-1, 1, "-1.0");
  VerifyPrint2<float>(-1, 2, "-1.00");

  VerifyPrint2<float>(0.9994, -1, "1");
  VerifyPrint2<float>(0.9994, 0, "1");
  VerifyPrint2<float>(0.9994, 1, "1.0");
  VerifyPrint2<float>(0.9994, 2, "1.00");
  VerifyPrint2<float>(0.9994, 3, "0.999");
  VerifyPrint2<float>(0.9994, 4, "0.9994");
  VerifyPrint2<float>(0.9994, 5, "0.99940");

  // Impl limits us to 20 digits (arbitrary choice).
  VerifyPrint2<float>(0, 30, "0.00000000000000000000");
}

TEST(PrintTest, IntegerToHex) {
  VerifyPrintHex<unsigned char>(0, "0");
  VerifyPrintHex<unsigned char>(127, "7F");
  VerifyPrintHex<unsigned char>(128, "80");
  VerifyPrintHex<unsigned char>(255, "FF");

  VerifyPrintHex<int16_t>(-32768, "8000");
  VerifyPrintHex<int16_t>(-1, "FFFF");
  VerifyPrintHex<int16_t>(0, "0");
  VerifyPrintHex<int16_t>(1, "1");
  VerifyPrintHex<int16_t>(10, "A");
  VerifyPrintHex<int16_t>(15, "F");
  VerifyPrintHex<int16_t>(16, "10");
  VerifyPrintHex<int16_t>(32767, "7FFF");

  VerifyPrintHex<uint16_t>(0, "0");
  VerifyPrintHex<uint16_t>(1, "1");
  VerifyPrintHex<uint16_t>(10, "A");
  VerifyPrintHex<uint16_t>(15, "F");
  VerifyPrintHex<uint16_t>(16, "10");
  VerifyPrintHex<uint16_t>(32767, "7FFF");
  VerifyPrintHex<uint16_t>(32768, "8000");
  VerifyPrintHex<uint16_t>(65535, "FFFF");

  VerifyPrintHex<int32_t>(-2147483648, "80000000");
  VerifyPrintHex<int32_t>(-1, "FFFFFFFF");
  VerifyPrintHex<int32_t>(0, "0");
  VerifyPrintHex<int32_t>(1, "1");
  VerifyPrintHex<uint16_t>(10, "A");
  VerifyPrintHex<uint16_t>(15, "F");
  VerifyPrintHex<uint16_t>(16, "10");
  VerifyPrintHex<int32_t>(2147483647, "7FFFFFFF");

  VerifyPrintHex<uint32_t>(0, "0");
  VerifyPrintHex<uint32_t>(1, "1");
  VerifyPrintHex<uint32_t>(10, "A");
  VerifyPrintHex<uint32_t>(15, "F");
  VerifyPrintHex<uint32_t>(16, "10");
  VerifyPrintHex<uint32_t>(2147483647, "7FFFFFFF");
  VerifyPrintHex<uint32_t>(2147483648, "80000000");
  VerifyPrintHex<uint32_t>(4294967295, "FFFFFFFF");
}

TEST(PrintTest, SamplePrintable) {
  constexpr char kText[] = "asdlkjasd;flkjasf";
  SamplePrintable sp;
  sp.str = kText;

  PrintToStdString out;
  out.println(sp);
  out.print(sp);

  EXPECT_EQ(out.str(), absl::StrCat(kText, "\n", kText));
}

TEST(PrintTest, StreamPrintable) {
  constexpr char kText[] = "sdlkadkjfdakjklaj";
  SamplePrintable sp;
  sp.str = kText;
  {
    std::ostringstream oss;
    oss << sp << '\n' << sp;
    EXPECT_EQ(oss.str(), absl::StrCat(kText, "\n", kText));
  }
}

TEST(PrintTest, Misc) {
  PrintToStdString out;
  out.println();
  EXPECT_EQ(out.str(), "\n");
}

}  // namespace
