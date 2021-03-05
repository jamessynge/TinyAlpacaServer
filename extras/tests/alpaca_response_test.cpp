#include "alpaca_response.h"

#include "absl/strings/str_cat.h"
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

TEST(AlpacaResponseTest, ValueTrue) {
  AlpacaRequest request;
  request.set_server_transaction_id(0);
  PrintToString out;
  WriteBoolResponse(request, true, out);

  std::string body = R"({"Value": true, "ServerTransactionId": 0,)"
                     R"( "ErrorNumber": 0, "ErrorMessage": ""})";

  EXPECT_EQ(
      out.str(),
      absl::StrCat("HTTP/1.1 200 OK", kEOL, "Server: TinyAlpacaServer", kEOL,
                   "Connection: close", kEOL, "Content-Type: application/json",
                   kEOL, "Content-Length: ", body.size(), kEOL, kEOL, body));
}

TEST(AlpacaResponseTest, Double) {
  const double value = 3.1415926;
  std::string value_str;
  {
    PrintToString out;
    out.print(value);
    value_str = out.str();
  }

  AlpacaRequest request;
  request.set_client_transaction_id(99);
  PrintToString out;
  WriteDoubleResponse(request, value, out);

  std::string body = absl::StrCat(R"({"Value": )", value_str,
                                  R"(, "ClientTransactionId": 99,)",
                                  R"( "ErrorNumber": 0, "ErrorMessage": ""})");

  EXPECT_EQ(
      out.str(),
      absl::StrCat("HTTP/1.1 200 OK", kEOL, "Server: TinyAlpacaServer", kEOL,
                   "Connection: close", kEOL, "Content-Type: application/json",
                   kEOL, "Content-Length: ", body.size(), kEOL, kEOL, body));
}

}  // namespace
}  // namespace alpaca
