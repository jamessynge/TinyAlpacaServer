#include "json_response.h"

#include "extras/tests/json_test_utils.h"
#include "extras/tests/test_utils.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace {

TEST(JsonMethodResponseTest, AllFields) {
  AlpacaRequest request;
  request.Reset();
  request.set_server_transaction_id(123);
  request.set_client_transaction_id(789);

  JsonMethodResponse response(
      request, /*error_number=*/98765,
      /*error_message=*/StringView("Are you saying \"Hey, look at that!\"?"));

  PrintToString out;
  JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(),
            R"({"ClientTransactionId": 789, )"
            R"("ServerTransactionId": 123, )"
            R"("ErrorNumber": 98765, )"
            R"("ErrorMessage": "Are you saying \"Hey, look at that!\"?"})");
}

TEST(JsonArrayResponseTest, Empty) {
  ElementSourceFunctionAdapter elements([](JsonArrayEncoder& encoder) {});
  AlpacaRequest request;
  request.set_client_id(2);  // Ignored.
  request.set_server_transaction_id(0);
  JsonArrayResponse response(request, elements);

  PrintToString out;
  JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"Value": [], )"
                       R"("ServerTransactionId": 0, )"
                       R"("ErrorNumber": 0, )"
                       R"("ErrorMessage": ""})");
}

TEST(JsonArrayResponseTest, Mixed) {
  ElementSourceFunctionAdapter elements([](JsonArrayEncoder& encoder) {
    encoder.AddBooleanElement(false);
    encoder.AddIntegerElement(-1);
    encoder.AddStringElement(StringView("\r\n"));
  });
  AlpacaRequest request;
  JsonArrayResponse response(request, elements);

  PrintToString out;
  JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"Value": [false, -1, ")"
                       "\\r\\n"
                       R"("], )"
                       R"("ErrorNumber": 0, )"
                       R"("ErrorMessage": ""})");
}

TEST(JsonBoolResponseTest, True) {
  AlpacaRequest request;
  JsonBoolResponse response(request, true);

  PrintToString out;
  JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"Value": true, )"
                       R"("ErrorNumber": 0, )"
                       R"("ErrorMessage": ""})");
}

TEST(JsonBoolResponseTest, False) {
  AlpacaRequest request;
  request.set_client_transaction_id(2);
  request.set_server_transaction_id(3);
  JsonBoolResponse response(request, false);

  PrintToString out;
  JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"Value": false, )"
                       R"("ClientTransactionId": 2, )"
                       R"("ServerTransactionId": 3, )"
                       R"("ErrorNumber": 0, )"
                       R"("ErrorMessage": ""})");
}

}  // namespace
}  // namespace alpaca
