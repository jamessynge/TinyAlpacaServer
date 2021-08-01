#include "utils/o_print_stream.h"

#include <stdint.h>

#include <string_view>

#include "gtest/gtest.h"
#include "mcucore/extrastest_tools/print_to_std_string.h"
#include "mcucore/extrastest_tools/sample_printable.h"

namespace alpaca {
namespace test {
namespace {

template <typename T>
void VerifyOPrintStream(const T value, std::string_view expected) {
  mcucore::test::PrintToStdString p2ss;
  OPrintStream out(p2ss);
  out << value;
  EXPECT_EQ(p2ss.str(), expected) << "Value: " << value;
}

TEST(OPrintStreamTest, BuiltInTypes) {
  VerifyOPrintStream<char>('a', "a");
  VerifyOPrintStream<char>('\0', std::string_view("\0", 1));

  VerifyOPrintStream<unsigned char>(0, "0");
  VerifyOPrintStream<unsigned char>(255, "255");

  VerifyOPrintStream<int16_t>(-32768, "-32768");
  VerifyOPrintStream<int16_t>(0, "0");
  VerifyOPrintStream<int16_t>(32767, "32767");

  VerifyOPrintStream<uint16_t>(0, "0");
  VerifyOPrintStream<uint16_t>(65535, "65535");

  VerifyOPrintStream<int32_t>(-2147483648, "-2147483648");
  VerifyOPrintStream<int32_t>(0, "0");
  VerifyOPrintStream<int32_t>(2147483647, "2147483647");

  VerifyOPrintStream<uint32_t>(0, "0");
  VerifyOPrintStream<uint32_t>(4294967295, "4294967295");

  VerifyOPrintStream<int64_t>(0, "0");
  VerifyOPrintStream<uint64_t>(0, "0");

  // 2 digits to the right of the decimal point, unless more features are added
  // to OPrintStream to allow specifying these values, as std::basic_ostream
  // does via std::hex, etc.
  VerifyOPrintStream<float>(-1, "-1.00");
  VerifyOPrintStream<float>(0, "0.00");
  VerifyOPrintStream<float>(0.99999, "1.00");
}

TEST(OPrintStreamTest, StringLiteral) {
  mcucore::test::PrintToStdString p2ss;
  OPrintStream out(p2ss);
  out << "abc";
  EXPECT_EQ(p2ss.str(), "abc");
}

TEST(OPrintStreamTest, Printable) {
  mcucore::test::SamplePrintable value("abc");
  {
    mcucore::test::PrintToStdString p2ss;
    OPrintStream out(p2ss);
    out << value;
    EXPECT_EQ(p2ss.str(), "abc");
  }
  {
    auto& value_ref = value;
    mcucore::test::PrintToStdString p2ss;
    OPrintStream out(p2ss);
    out << value_ref;
    EXPECT_EQ(p2ss.str(), "abc");
  }
}

TEST(OPrintStreamTest, ConstPrintable) {
  const mcucore::test::SamplePrintable value("abc");
  {
    mcucore::test::PrintToStdString p2ss;
    OPrintStream out(p2ss);
    out << value;
    EXPECT_EQ(p2ss.str(), "abc");
  }
  {
    auto& value_ref = value;
    mcucore::test::PrintToStdString p2ss;
    OPrintStream out(p2ss);
    out << value_ref;
    EXPECT_EQ(p2ss.str(), "abc");
  }
}

TEST(OPrintStreamTest, ChangeBase) {
  mcucore::test::PrintToStdString p2ss;
  OPrintStream out(p2ss);

  out << 127 << " " << BaseHex << 127 << ' ' << BaseTwo << 127;
  EXPECT_EQ(p2ss.str(), "127 0x7F 0b1111111");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
