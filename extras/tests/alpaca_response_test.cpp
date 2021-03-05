#include "alpaca_response.h"

#include "extras/tests/json_test_utils.h"
#include "extras/tests/test_utils.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace {

constexpr char kEOL[] = "\r\n";

TEST(AlpacaResponseTest, SimpleOk) {
  PropertySourceFunctionAdapter source([](JsonObjectEncoder& encoder) {});
  PrintToString out;
  WriteOkResponse(source, out);
  EXPECT_EQ(
      out.str(),
      absl::StrCat("HTTP/1.1 200 OK", kEOL, "Server: TinyAlpacaServer", kEOL,
                   "Connection: close", kEOL, "Content-Type: application/json",
                   kEOL, "Content-Length: 2", kEOL, kEOL, "{}"));
}

}  // namespace
}  // namespace alpaca
