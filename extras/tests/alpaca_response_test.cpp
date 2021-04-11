#include "alpaca_response.h"

#include <string_view>
#include <utility>
#include <vector>

#include "absl/strings/str_cat.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "extras/test_tools/json_test_utils.h"
#include "extras/test_tools/print_to_std_string.h"
#include "extras/test_tools/sample_printable.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "literals.h"
#include "utils/any_printable.h"
#include "utils/array_view.h"
#include "utils/literal.h"

namespace alpaca {
namespace {

constexpr char kEOL[] = "\r\n";
constexpr bool kDoNotClose = false;
constexpr bool kDoClose = true;

std::string MakeExpectedResponseHeader(std::string_view expected_body,
                                       bool do_close = kDoNotClose) {
  return                                                            // Force
      absl::StrCat("HTTP/1.1 200 OK", kEOL,                         // line
                   "Server: TinyAlpacaServer", kEOL,                // wrapping
                   do_close ? "Connection: close\r\n" : "",         // right
                   "Content-Type: application/json", kEOL,          // here
                   "Content-Length: ", expected_body.size(), kEOL,  // and.
                   kEOL);                                           // here.
}

// Make a "standard" ASCOM response, where there is no error and an optional
// value.
std::string MakeExpectedResponse(std::string_view raw_json_for_value,
                                 bool do_close = kDoNotClose, int txn_id = 0) {
  std::string expected_body = "{";
  if (!raw_json_for_value.empty()) {
    expected_body += absl::StrCat(R"("Value": )", raw_json_for_value, ", ");
  }
  if (txn_id >= 0) {
    expected_body += absl::StrCat(R"("ServerTransactionID": )", txn_id, ", ");
  }
  expected_body +=
      absl::StrCat(R"("ErrorNumber": 0, "ErrorMessage": ""})", kEOL);
  std::string header = MakeExpectedResponseHeader(expected_body, do_close);
  return header + expected_body;
}

std::string MakeExpectedBoolResponse(bool value, bool do_close = kDoNotClose,
                                     int txn_id = 0) {
  return MakeExpectedResponse(value ? "true" : "false", do_close, txn_id);
}

std::string MakeExpectedDoubleResponse(double value,
                                       bool do_close = kDoNotClose,
                                       int txn_id = 0) {
  // We need to produce a string in the form that the double value will be
  // printed (i.e. not every double formatter will produce the same string).
  std::string value_str;
  {
    PrintToStdString out;
    out.print(value);
    value_str = out.str();
  }
  return MakeExpectedResponse(value_str, do_close, txn_id);
}

std::string MakeExpectedIntResponse(int value, bool do_close = kDoNotClose,
                                    int txn_id = 0) {
  std::string value_str;
  {
    PrintToStdString out;
    out.print(value);
    value_str = out.str();
  }
  return MakeExpectedResponse(value_str, do_close, txn_id);
}

// Direct test of WriteResponse::OkResponse, where we're producing an empty
// JSON object as the body.
TEST(AlpacaResponseTest, OkWithEmptyJsonObject) {
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

TEST(AlpacaResponseTest, StatusOrBoolResponse) {
  {
    AlpacaRequest request;
    request.set_server_transaction_id(98765);
    request.do_close = true;
    PrintToStdString out;
    EXPECT_FALSE(WriteResponse::StatusOrBoolResponse(request, false, out));
    EXPECT_EQ(out.str(), MakeExpectedBoolResponse(false, kDoClose, 98765));
  }
  {
    AlpacaRequest request;
    PrintToStdString out;
    EXPECT_TRUE(WriteResponse::StatusOrBoolResponse(
        request, StatusOr<bool>(false), out));
    EXPECT_EQ(out.str(), MakeExpectedBoolResponse(false, kDoNotClose, -1));
  }
  {
    AlpacaRequest request;
    request.set_server_transaction_id(88);
    request.do_close = false;
    PrintToStdString out;
    EXPECT_TRUE(WriteResponse::StatusOrBoolResponse(request, true, out));
    EXPECT_EQ(out.str(), MakeExpectedBoolResponse(true, kDoNotClose, 88));
  }
  {
    AlpacaRequest request;
    request.set_server_transaction_id(0);
    request.do_close = true;
    PrintToStdString out;
    EXPECT_FALSE(WriteResponse::StatusOrBoolResponse(
        request, StatusOr<bool>(true), out));
    EXPECT_EQ(out.str(), MakeExpectedBoolResponse(true, kDoClose));
  }
  {
    AlpacaRequest request;
    request.set_client_transaction_id(432);
    request.set_server_transaction_id(321);
    request.do_close = true;
    PrintToStdString out;
    EXPECT_FALSE(WriteResponse::StatusOrBoolResponse(
        request, ErrorCodes::InvalidWhileSlaved(), out));
    const std::string expected_body = absl::StrCat(
        R"({"ClientTransactionID": 432, "ServerTransactionID": 321, )",
        R"("ErrorNumber": )", ErrorCodes::kInvalidWhileSlaved,
        R"(, "ErrorMessage": ")",
        PrintValueToStdString(Literals::ErrorInvalidWhileSlaved()), R"("})",
        kEOL);
    const std::string expected_response =
        MakeExpectedResponseHeader(expected_body, kDoClose) + expected_body;
    EXPECT_EQ(out.str(), expected_response);
  }
}

TEST(AlpacaResponseTest, StatusOrDoubleResponse) {
  {
    AlpacaRequest request;
    request.set_server_transaction_id(7);
    request.do_close = true;
    PrintToStdString out;
    EXPECT_FALSE(WriteResponse::StatusOrDoubleResponse(
        request, StatusOr<double>(45.5), out));
    EXPECT_EQ(out.str(), MakeExpectedDoubleResponse(45.5, kDoClose, 7));
  }
  {
    AlpacaRequest request;
    request.set_server_transaction_id(3);
    // do_close is overridden by WriteResponse::AscomErrorResponse.
    request.do_close = false;
    PrintToStdString out;
    EXPECT_FALSE(WriteResponse::StatusOrDoubleResponse(
        request, ErrorCodes::InvalidWhileSlaved(), out));
    const std::string expected_body =
        absl::StrCat(R"({"ServerTransactionID": 3,)", R"( "ErrorNumber": )",
                     ErrorCodes::kInvalidWhileSlaved, R"(, "ErrorMessage": ")",
                     PrintValueToStdString(Literals::ErrorInvalidWhileSlaved()),
                     R"("})", kEOL);
    const std::string expected_response =
        MakeExpectedResponseHeader(expected_body, kDoClose) + expected_body;
    EXPECT_EQ(out.str(), expected_response);
  }
}

TEST(AlpacaResponseTest, StatusOrFloatResponse) {
  {
    AlpacaRequest request;
    request.set_server_transaction_id(7);
    request.do_close = false;
    PrintToStdString out;
    EXPECT_TRUE(WriteResponse::StatusOrFloatResponse(
        request, StatusOr<float>(45.5), out));
    EXPECT_EQ(out.str(), MakeExpectedDoubleResponse(45.5, kDoNotClose, 7));
  }
  {
    AlpacaRequest request;
    request.set_server_transaction_id(3);
    request.do_close = true;
    PrintToStdString out;
    EXPECT_FALSE(WriteResponse::StatusOrFloatResponse(
        request, ErrorCodes::InvalidWhileSlaved(), out));
    const std::string expected_body =
        absl::StrCat(R"({"ServerTransactionID": 3,)", R"( "ErrorNumber": )",
                     ErrorCodes::kInvalidWhileSlaved, R"(, "ErrorMessage": ")",
                     PrintValueToStdString(Literals::ErrorInvalidWhileSlaved()),
                     R"("})", kEOL);
    const std::string expected_response =
        MakeExpectedResponseHeader(expected_body, kDoClose) + expected_body;
    EXPECT_EQ(out.str(), expected_response);
  }
}

TEST(AlpacaResponseTest, StatusOrUIntResponse) {
  {
    AlpacaRequest request;
    request.set_server_transaction_id(7);
    request.do_close = false;
    PrintToStdString out;
    EXPECT_TRUE(WriteResponse::StatusOrUIntResponse(
        request, StatusOr<uint32_t>(45), out));
    EXPECT_EQ(out.str(), MakeExpectedIntResponse(45, kDoNotClose, 7));
  }
  {
    AlpacaRequest request;
    request.set_server_transaction_id(3);
    request.do_close = true;
    PrintToStdString out;
    EXPECT_FALSE(WriteResponse::StatusOrUIntResponse(
        request, ErrorCodes::InvalidWhileParked(), out));
    const std::string expected_body =
        absl::StrCat(R"({"ServerTransactionID": 3,)", R"( "ErrorNumber": )",
                     ErrorCodes::kInvalidWhileParked, R"(, "ErrorMessage": ")",
                     PrintValueToStdString(Literals::ErrorInvalidWhileParked()),
                     R"("})", kEOL);
    const std::string expected_response =
        MakeExpectedResponseHeader(expected_body, kDoClose) + expected_body;
    EXPECT_EQ(out.str(), expected_response);
  }
}

TEST(AlpacaResponseTest, StatusOrIntResponse) {
  {
    AlpacaRequest request;
    request.set_server_transaction_id(5);
    request.do_close = true;
    PrintToStdString out;
    EXPECT_FALSE(WriteResponse::StatusOrIntResponse(
        request, StatusOr<int32_t>(-45), out));
    EXPECT_EQ(out.str(), MakeExpectedIntResponse(-45, kDoClose, 5));
  }
  {
    AlpacaRequest request;
    request.set_client_transaction_id(5);
    request.do_close = false;
    PrintToStdString out;
    EXPECT_FALSE(WriteResponse::StatusOrIntResponse(
        request, ErrorCodes::InvalidWhileParked(), out));
    const std::string expected_body =
        absl::StrCat(R"({"ClientTransactionID": 5,)", R"( "ErrorNumber": )",
                     ErrorCodes::kInvalidWhileParked, R"(, "ErrorMessage": ")",
                     PrintValueToStdString(Literals::ErrorInvalidWhileParked()),
                     R"("})", kEOL);
    const std::string expected_response =
        MakeExpectedResponseHeader(expected_body, kDoClose) + expected_body;
    EXPECT_EQ(out.str(), expected_response);
  }
}

TEST(AlpacaResponseTest, AnyPrintableStringResponse) {
  SamplePrintable sp("***");
  AlpacaRequest request;
  request.set_server_transaction_id(2);
  PrintToStdString out;
  EXPECT_TRUE(WriteResponse::AnyPrintableStringResponse(request,
                                                        AnyPrintable(sp), out));
  EXPECT_EQ(out.str(), MakeExpectedResponse(R"("***")", kDoNotClose, 2));
}

TEST(AlpacaResponseTest, StatusOrLiteralResponse) {
  {
    AlpacaRequest request;
    request.set_server_transaction_id(1);
    PrintToStdString out;
    EXPECT_TRUE(WriteResponse::StatusOrLiteralResponse(
        request, StatusOr<Literal>(Literals::DeviceType()), out));
    EXPECT_EQ(out.str(),
              MakeExpectedResponse(R"("DeviceType")", kDoNotClose, 1));
  }
  {
    AlpacaRequest request;
    request.set_client_transaction_id(100);
    request.set_server_transaction_id(4);
    PrintToStdString out;
    EXPECT_FALSE(WriteResponse::StatusOrLiteralResponse(
        request, ErrorCodes::InvalidWhileParked(), out));
    const auto error_message =
        PrintValueToStdString(Literals::ErrorInvalidWhileParked());
    const std::string expected_body = absl::StrCat(
        R"({"ClientTransactionID": 100, "ServerTransactionID": 4,)",
        R"( "ErrorNumber": )", ErrorCodes::kInvalidWhileParked,
        R"(, "ErrorMessage": ")", error_message, R"("})", kEOL);
    const std::string expected_response =
        MakeExpectedResponseHeader(expected_body, kDoClose) + expected_body;
    EXPECT_EQ(out.str(), expected_response);
  }
}

TEST(AlpacaResponseTest, LiteralArrayResponse) {
  const Literal kLiterals[] = {Literals::DeviceType(),
                               Literals::ManufacturerVersion()};
  LiteralArray value(kLiterals);

  // Note that we're NOT setting the server transaction id here, nor error
  // number.
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

TEST(AlpacaResponseTest, UIntArrayResponse) {
  uint32_t versions[] = {3, 1};
  AlpacaRequest request;
  PrintToStdString out;
  EXPECT_TRUE(
      WriteResponse::UIntArrayResponse(request, MakeArrayView(versions), out));
  EXPECT_EQ(out.str(), MakeExpectedResponse("[3, 1]", kDoNotClose, -1));
}

TEST(AlpacaResponseTest, AscomActionNotImplementedResponse) {
  AlpacaRequest request;
  request.set_server_transaction_id(1);
  PrintToStdString out;
  EXPECT_FALSE(WriteResponse::AscomActionNotImplementedResponse(request, out));
  const std::string expected_body =
      absl::StrCat(R"({"ServerTransactionID": 1,)", R"( "ErrorNumber": )",
                   ErrorCodes::kActionNotImplemented, R"(, "ErrorMessage": ")",
                   PrintValueToStdString(Literals::ErrorActionNotImplemented()),
                   R"("})", kEOL);
  const std::string expected_response =
      MakeExpectedResponseHeader(expected_body, kDoClose) + expected_body;
  EXPECT_EQ(out.str(), expected_response);
}

TEST(AlpacaResponseTest, HttpErrorResponse) {
  std::vector<std::pair<int, std::string_view>> test_cases = {
      // Problems with the request:
      {400, "Bad Request"},
      {405, "Method Not Allowed"},
      {406, "Not Acceptable"},
      {411, "Length Required"},
      {413, "Payload Too Large"},
      {415, "Unsupported Media Type"},
      {431, "Request Header Fields Too Large"},

      // Problems with handling the request:
      {500, "Internal Server Error"},
      {501, "Method Not Implemented"},
      {505, "HTTP Version Not Supported"},
  };
  for (auto [status, reason] : test_cases) {
    SamplePrintable body(absl::StrCat("body for status code ", status,
                                      ", with reason ", reason));
    PrintToStdString out;
    EXPECT_FALSE(WriteResponse::HttpErrorResponse(
        static_cast<EHttpStatusCode>(status), body, out));
    const auto expected =                                        // Force
        absl::StrCat("HTTP/1.1 ", status, " ", reason, kEOL,     // line
                     "Server: TinyAlpacaServer", kEOL,           // wrapping
                     "Connection: close", kEOL,                  // right
                     "Content-Type: text/plain", kEOL,           // here,
                     "Content-Length: ", body.str.size(), kEOL,  // here,
                     kEOL,                                       // and
                     body.str);                                  // here.
    EXPECT_EQ(out.str(), expected);
  }
}

TEST(AlpacaResponseDeathTest, HttpErrorResponse_NotAnError) {
  EXPECT_DEBUG_DEATH(
      {
        SamplePrintable body("body");
        PrintToStdString out;
        EXPECT_FALSE(WriteResponse::HttpErrorResponse(EHttpStatusCode::kHttpOk,
                                                      body, out));
        auto reason_phrase = "Internal Server Error: Invalid HTTP Status Code";
        const auto expected =                                        // Force
            absl::StrCat("HTTP/1.1 500 ", reason_phrase, kEOL,       // line
                         "Server: TinyAlpacaServer", kEOL,           // wrap
                         "Connection: close", kEOL,                  // right
                         "Content-Type: text/plain", kEOL,           // here,
                         "Content-Length: ", body.str.size(), kEOL,  // here,
                         kEOL,                                       // and
                         body.str);                                  // here.
        EXPECT_EQ(out.str(), expected);
      },
      "Status code should be for an error");
}

TEST(AlpacaResponseDeathTest, HttpErrorResponse_NoReasonCode) {
  EXPECT_DEBUG_DEATH(
      {
        SamplePrintable body("");
        PrintToStdString out;
        EXPECT_FALSE(WriteResponse::HttpErrorResponse(
            static_cast<EHttpStatusCode>(499), body, out));
        const auto expected =                                        // Force
            absl::StrCat("HTTP/1.1 499 ", kEOL,                      // line
                         "Server: TinyAlpacaServer", kEOL,           // wrap
                         "Connection: close", kEOL,                  // right
                         "Content-Type: text/plain", kEOL,           // here,
                         "Content-Length: ", body.str.size(), kEOL,  // and
                         kEOL,                                       // here
                         body.str);
        EXPECT_EQ(out.str(), expected);
      },
      "Please add a case for status code.*499");
}

}  // namespace
}  // namespace alpaca
