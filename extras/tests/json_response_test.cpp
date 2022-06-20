#include "json_response.h"

#include <McuCore.h>
#include <stdint.h>

#include "alpaca_request.h"
#include "gtest/gtest.h"
#include "mcucore/extras/test_tools/json_test_utils.h"
#include "mcucore/extras/test_tools/print_to_std_string.h"

namespace alpaca {
namespace test {
namespace {

using ::mcucore::test::ElementSourceFunctionAdapter;

TEST(JsonMethodResponseTest, AllFields) {
  AlpacaRequest request;
  request.set_server_transaction_id(123);
  request.set_client_transaction_id(789);

  uint32_t error_number = 98765;
  mcucore::AnyPrintable error_message(
      mcucore::StringView("Are you saying \"Hey, look at that!\"?"));

  JsonMethodResponse response(request, error_number, error_message);

  mcucore::test::PrintToStdString out;
  mcucore::JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(),
            R"({"ClientTransactionID": 789, )"
            R"("ServerTransactionID": 123, )"
            R"("ErrorNumber": 98765, )"
            R"("ErrorMessage": "Are you saying \"Hey, look at that!\"?"})");
}

TEST(JsonMethodResponseTest, NoError) {
  AlpacaRequest request;
  request.set_server_transaction_id(123);
  request.set_client_transaction_id(789);

  JsonMethodResponse response(request);

  mcucore::test::PrintToStdString out;
  mcucore::JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"ClientTransactionID": 789, )"
                       R"("ServerTransactionID": 123, )"
                       R"("ErrorNumber": 0, )"
                       R"("ErrorMessage": ""})");
}

TEST(JsonArrayResponseTest, Empty) {
  ElementSourceFunctionAdapter elements(
      [](mcucore::JsonArrayEncoder& encoder) {});
  AlpacaRequest request;
  request.set_client_id(2);  // Ignored.
  request.set_server_transaction_id(0);
  JsonArrayResponse response(request, elements);

  mcucore::test::PrintToStdString out;
  mcucore::JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"Value": [], )"
                       R"("ServerTransactionID": 0, )"
                       R"("ErrorNumber": 0, )"
                       R"("ErrorMessage": ""})");
}

TEST(JsonArrayResponseTest, Mixed) {
  ElementSourceFunctionAdapter elements([](mcucore::JsonArrayEncoder& encoder) {
    encoder.AddBooleanElement(false);
    encoder.AddIntElement(-1);
    encoder.AddStringElement(mcucore::StringView("\r\n"));
  });
  AlpacaRequest request;
  JsonArrayResponse response(request, elements);

  mcucore::test::PrintToStdString out;
  mcucore::JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"Value": [false, -1, ")"
                       "\\r\\n"
                       R"("], )"
                       R"("ErrorNumber": 0, )"
                       R"("ErrorMessage": ""})");
}

TEST(JsonBoolResponseTest, True) {
  AlpacaRequest request;
  JsonBoolResponse response(request, true);

  mcucore::test::PrintToStdString out;
  mcucore::JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"Value": true, )"
                       R"("ErrorNumber": 0, )"
                       R"("ErrorMessage": ""})");
}

TEST(JsonBoolResponseTest, False) {
  AlpacaRequest request;
  request.set_client_transaction_id(2);
  request.set_server_transaction_id(3);
  JsonBoolResponse response(request, false);

  mcucore::test::PrintToStdString out;
  mcucore::JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"Value": false, )"
                       R"("ClientTransactionID": 2, )"
                       R"("ServerTransactionID": 3, )"
                       R"("ErrorNumber": 0, )"
                       R"("ErrorMessage": ""})");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
