#include "utils/o_print_stream.h"

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace {

template <typename T>
void VerifyOPrintStream(const T value, std::string_view expected) {
  PrintToStdString p2ss;
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

}  // namespace
}  // namespace alpaca
