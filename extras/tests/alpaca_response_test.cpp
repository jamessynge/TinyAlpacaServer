#include "alpaca_response.h"

#include "absl/strings/str_cat.h"
#include "extras/test_tools/json_test_utils.h"
#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "literals.h"
#include "utils/literal.h"

namespace alpaca {
namespace {

constexpr char kEOL[] = "\r\n";

TEST(AlpacaResponseTest, SimpleOk) {
  PropertySourceFunctionAdapter source([](JsonObjectEncoder& encoder) {});
  PrintToStdString out;
  WriteOkResponse(source, EHttpMethod::PUT, out);
  EXPECT_EQ(
      out.str(),
      absl::StrCat("HTTP/1.1 200 OK", kEOL, "Server: TinyAlpacaServer", kEOL,
                   "Connection: close", kEOL, "Content-Type: application/json",
                   kEOL, "Content-Length: 2", kEOL, kEOL, "{}"));
}

TEST(AlpacaResponseTest, ArrayOfLiterals) {
  const Literal kLiterals[] = {Literals::DeviceType(),
                               Literals::ManufacturerVersion()};
  LiteralArray value(kLiterals);

  AlpacaRequest request;
  PrintToStdString out;
  WriteLiteralArrayResponse(request, value, out);

  const std::string expected_body =
      R"({"Value": ["DeviceType", "ManufacturerVersion"],)"
      R"( "ErrorNumber": 0, "ErrorMessage": ""})";
  EXPECT_EQ(
      out.str(),
      absl::StrCat("HTTP/1.1 200 OK", kEOL, "Server: TinyAlpacaServer", kEOL,
                   "Connection: close", kEOL, "Content-Type: application/json",
                   kEOL, "Content-Length: ", expected_body.size(), kEOL, kEOL,
                   expected_body));
}

TEST(AlpacaResponseTest, BoolTrue) {
  AlpacaRequest request;
  request.set_server_transaction_id(0);
  PrintToStdString out;
  WriteBoolResponse(request, true, out);

  const std::string expected_body =
      R"({"Value": true, "ServerTransactionId": 0,)"
      R"( "ErrorNumber": 0, "ErrorMessage": ""})";
  EXPECT_EQ(
      out.str(),
      absl::StrCat("HTTP/1.1 200 OK", kEOL, "Server: TinyAlpacaServer", kEOL,
                   "Connection: close", kEOL, "Content-Type: application/json",
                   kEOL, "Content-Length: ", expected_body.size(), kEOL, kEOL,
                   expected_body));
}

TEST(AlpacaResponseTest, Double) {
  const double value = 3.1415926;
  std::string value_str;
  {
    PrintToStdString out;
    out.print(value);
    value_str = out.str();
  }

  AlpacaRequest request;
  request.set_client_transaction_id(99);
  PrintToStdString out;
  WriteDoubleResponse(request, value, out);

  const std::string expected_body = absl::StrCat(
      R"({"Value": )", value_str, R"(, "ClientTransactionId": 99,)",
      R"( "ErrorNumber": 0, "ErrorMessage": ""})");
  EXPECT_EQ(
      out.str(),
      absl::StrCat("HTTP/1.1 200 OK", kEOL, "Server: TinyAlpacaServer", kEOL,
                   "Connection: close", kEOL, "Content-Type: application/json",
                   kEOL, "Content-Length: ", expected_body.size(), kEOL, kEOL,
                   expected_body));
}

}  // namespace
}  // namespace alpaca
