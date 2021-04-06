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
  AlpacaRequest request;
  request.http_method = EHttpMethod::PUT;
  PropertySourceFunctionAdapter source([](JsonObjectEncoder& encoder) {});
  PrintToStdString out;
  EXPECT_TRUE(WriteResponse::OkResponse(request, source, out));
  const auto expected_body = absl::StrCat("{}", kEOL);
  const auto expected =                                             // Force
      absl::StrCat("HTTP/1.1 200 OK", kEOL,                         // line
                   "Server: TinyAlpacaServer", kEOL,                // wrapping
                   "Content-Type: application/json", kEOL,          // right
                   "Content-Length: ", expected_body.size(), kEOL,  // here.
                   kEOL, expected_body);
  EXPECT_EQ(out.str(), expected);
}

TEST(AlpacaResponseTest, SimpleOkWithClose) {
  AlpacaRequest request;
  request.http_method = EHttpMethod::PUT;
  request.do_close = true;
  PropertySourceFunctionAdapter source([](JsonObjectEncoder& encoder) {});
  PrintToStdString out;
  EXPECT_FALSE(WriteResponse::OkResponse(request, source, out));
  const std::string expected_body = absl::StrCat("{}", kEOL);
  const auto expected =                                             // Force
      absl::StrCat("HTTP/1.1 200 OK", kEOL,                         // line
                   "Server: TinyAlpacaServer", kEOL,                // wrapping
                   "Connection: close", kEOL,                       // here
                   "Content-Type: application/json", kEOL,          // and
                   "Content-Length: ", expected_body.size(), kEOL,  // here.
                   kEOL, expected_body);
  EXPECT_EQ(out.str(), expected);
}

TEST(AlpacaResponseTest, ArrayOfLiterals) {
  const Literal kLiterals[] = {Literals::DeviceType(),
                               Literals::ManufacturerVersion()};
  LiteralArray value(kLiterals);

  AlpacaRequest request;
  PrintToStdString out;
  EXPECT_TRUE(WriteResponse::LiteralArrayResponse(request, value, out));

  const std::string expected_body =
      absl::StrCat(R"({"Value": ["DeviceType", "ManufacturerVersion"],)",
                   R"( "ErrorNumber": 0, "ErrorMessage": ""})", kEOL);
  EXPECT_EQ(out.str(),
            absl::StrCat("HTTP/1.1 200 OK", kEOL, "Server: TinyAlpacaServer",
                         kEOL, "Content-Type: application/json", kEOL,
                         "Content-Length: ", expected_body.size(), kEOL, kEOL,
                         expected_body));
}

TEST(AlpacaResponseTest, BoolTrue) {
  AlpacaRequest request;
  request.set_server_transaction_id(0);
  PrintToStdString out;
  EXPECT_TRUE(WriteResponse::BoolResponse(request, true, out));

  const std::string expected_body =
      absl::StrCat(R"({"Value": true, "ServerTransactionId": 0,)",
                   R"( "ErrorNumber": 0, "ErrorMessage": ""})", kEOL);
  const auto expected =                                             // Force
      absl::StrCat("HTTP/1.1 200 OK", kEOL,                         // line
                   "Server: TinyAlpacaServer", kEOL,                // wrapping
                   "Content-Type: application/json", kEOL,          // right
                   "Content-Length: ", expected_body.size(), kEOL,  // here.
                   kEOL, expected_body);
  EXPECT_EQ(out.str(), expected);
}

TEST(AlpacaResponseTest, DoubleWithClose) {
  const double value = 3.1415926;
  std::string value_str;
  {
    PrintToStdString out;
    out.print(value);
    value_str = out.str();
  }

  AlpacaRequest request;
  request.set_client_transaction_id(99);
  request.do_close = true;
  PrintToStdString out;
  EXPECT_FALSE(WriteResponse::DoubleResponse(request, value, out));

  const std::string expected_body = absl::StrCat(
      R"({"Value": )", value_str, R"(, "ClientTransactionId": 99,)",
      R"( "ErrorNumber": 0, "ErrorMessage": ""})", kEOL);
  const auto expected =                                             // Force
      absl::StrCat("HTTP/1.1 200 OK", kEOL,                         // line
                   "Server: TinyAlpacaServer", kEOL,                // wrapping
                   "Connection: close", kEOL,                       // here
                   "Content-Type: application/json", kEOL,          // and
                   "Content-Length: ", expected_body.size(), kEOL,  // here.
                   kEOL, expected_body);
  EXPECT_EQ(out.str(), expected);
}

}  // namespace
}  // namespace alpaca
