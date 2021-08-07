#include "utils/stream_to_print.h"

#include <stdint.h>

#include <string_view>

#include "gtest/gtest.h"
#include "mcucore/extrashost/arduino/print.h"
#include "mcucore/extrastest_tools/print_to_std_string.h"
#include "mcucore/extrastest_tools/sample_printable.h"
#include "o_print_stream.h"

namespace alpaca {
namespace test {
namespace {

template <typename T>
void VerifyStreamToPrint(const T value, std::string_view expected) {
  mcucore::test::PrintToStdString p2ss;
  p2ss << value;
  EXPECT_EQ(p2ss.str(), expected) << "Value: " << value;
}

TEST(StreamToPrintTest, BuiltInTypes) {
  VerifyStreamToPrint<char>('a', "a");
  VerifyStreamToPrint<char>('\0', std::string_view("\0", 1));

  VerifyStreamToPrint<unsigned char>(0, "0");
  VerifyStreamToPrint<unsigned char>(255, "255");

  VerifyStreamToPrint<int16_t>(-32768, "-32768");
  VerifyStreamToPrint<int16_t>(0, "0");
  VerifyStreamToPrint<int16_t>(32767, "32767");

  VerifyStreamToPrint<uint16_t>(0, "0");
  VerifyStreamToPrint<uint16_t>(65535, "65535");

  VerifyStreamToPrint<int32_t>(-2147483648, "-2147483648");
  VerifyStreamToPrint<int32_t>(0, "0");
  VerifyStreamToPrint<int32_t>(2147483647, "2147483647");

  VerifyStreamToPrint<uint32_t>(0, "0");
  VerifyStreamToPrint<uint32_t>(4294967295, "4294967295");

  VerifyStreamToPrint<int64_t>(0, "0");
  VerifyStreamToPrint<uint64_t>(0, "0");

  // 2 digits to the right of the decimal point, unless more features are added
  // to StreamToPrint to allow specifying these values, as std::basic_ostream
  // does via std::hex, etc.
  VerifyStreamToPrint<float>(-1, "-1.00");
  VerifyStreamToPrint<float>(0, "0.00");
  VerifyStreamToPrint<float>(0.99999, "1.00");
}

TEST(StreamToPrintTest, StringLiteral) {
  mcucore::test::PrintToStdString p2ss;
  p2ss << "abc";
  EXPECT_EQ(p2ss.str(), "abc");
}

TEST(StreamToPrintTest, Printable) {
  mcucore::test::SamplePrintable value("abc");
  {
    mcucore::test::PrintToStdString p2ss;
    p2ss << value;
    EXPECT_EQ(p2ss.str(), "abc");
  }
  {
    auto& value_ref = value;
    mcucore::test::PrintToStdString p2ss;
    p2ss << value_ref;
    EXPECT_EQ(p2ss.str(), "abc");
  }
}

TEST(StreamToPrintTest, ConstPrintable) {
  const mcucore::test::SamplePrintable value("abc");
  {
    mcucore::test::PrintToStdString p2ss;
    p2ss << value;
    EXPECT_EQ(p2ss.str(), "abc");
  }
  {
    auto& value_ref = value;
    mcucore::test::PrintToStdString p2ss;
    p2ss << value_ref;
    EXPECT_EQ(p2ss.str(), "abc");
  }
}

TEST(StreamToPrintTest, ChangeBase) {
  mcucore::test::PrintToStdString p2ss;
  p2ss << 127 << " " << BaseHex << 127 << ' ' << BaseDec << -123;
  EXPECT_EQ(p2ss.str(), "127 0x7F -123");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
