#include "json_response.h"

#include <stdint.h>

#include "alpaca_request.h"
#include "extras/test_tools/json_test_utils.h"
#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gtest.h"
#include "utils/any_printable.h"
#include "utils/json_encoder.h"
#include "utils/string_view.h"

namespace alpaca {
namespace test {
namespace {

TEST(JsonMethodResponseTest, AllFields) {
  AlpacaRequest request;
  request.set_server_transaction_id(123);
  request.set_client_transaction_id(789);

  uint32_t error_number = 98765;
  AnyPrintable error_message(
      StringView("Are you saying \"Hey, look at that!\"?"));

  JsonMethodResponse response(request, error_number, error_message);

  PrintToStdString out;
  JsonObjectEncoder::Encode(response, out);
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

  PrintToStdString out;
  JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"ClientTransactionID": 789, )"
                       R"("ServerTransactionID": 123})");
}

TEST(JsonArrayResponseTest, Empty) {
  ElementSourceFunctionAdapter elements([](JsonArrayEncoder& encoder) {});
  AlpacaRequest request;
  request.set_client_id(2);  // Ignored.
  request.set_server_transaction_id(0);
  JsonArrayResponse response(request, elements);

  PrintToStdString out;
  JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"Value": [], )"
                       R"("ServerTransactionID": 0})");
}

TEST(JsonArrayResponseTest, Mixed) {
  ElementSourceFunctionAdapter elements([](JsonArrayEncoder& encoder) {
    encoder.AddBooleanElement(false);
    encoder.AddIntElement(-1);
    encoder.AddStringElement(StringView("\r\n"));
  });
  AlpacaRequest request;
  JsonArrayResponse response(request, elements);

  PrintToStdString out;
  JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"Value": [false, -1, ")"
                       "\\r\\n"
                       R"("]})");
}

TEST(JsonBoolResponseTest, True) {
  AlpacaRequest request;
  JsonBoolResponse response(request, true);

  PrintToStdString out;
  JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"Value": true})");
}

TEST(JsonBoolResponseTest, False) {
  AlpacaRequest request;
  request.set_client_transaction_id(2);
  request.set_server_transaction_id(3);
  JsonBoolResponse response(request, false);

  PrintToStdString out;
  JsonObjectEncoder::Encode(response, out);
  EXPECT_EQ(out.str(), R"({"Value": false, )"
                       R"("ClientTransactionID": 2, )"
                       R"("ServerTransactionID": 3})");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
