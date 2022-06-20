#include "extras/test_tools/alpaca_response_validator.h"

#include <optional>
#include <string>

#include "absl/status/status.h"
#include "alpaca_request.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mcucore/extras/test_tools/http_request.h"
#include "mcucore/extras/test_tools/http_response.h"

namespace alpaca {
namespace test {
namespace {

using ::mcucore::test::HttpResponse;
using ::testing::ElementsAre;
using ::testing::HasSubstr;
using ::testing::status::IsOkAndHolds;
using ::testing::status::StatusIs;

constexpr int kExpectedServerTransactionId = 1;

bool HasTestFailed() {
  auto current_test_info =
      ::testing::UnitTest::GetInstance()->current_test_info();
  if (current_test_info == nullptr) {
    return false;
  }
  auto test_result = current_test_info->result();
  return test_result != nullptr && test_result->Failed();
}

// `message` is OK, well-formed from Alpaca perspective.
void ExpectAlpacaResponseWellFormed(const std::string& message) {
  {
    ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(message));
    ASSERT_OK(response.IsOk());
    ASSERT_TRUE(response.json_value.is_object());
  }

  AlpacaResponseValidator validator;
  validator.expected_server_transaction_id = kExpectedServerTransactionId;

  validator.last_server_transaction_id = std::nullopt;
  ASSERT_OK(validator.ValidateResponseIsOk(message));
}

// `message` is OK, well-formed from Alpaca perspective.
void ExpectAlpacaResponseWithoutError(const std::string& message) {
  ExpectAlpacaResponseWellFormed(message);
  if (HasTestFailed()) return;
  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_OK_AND_ASSIGN(auto response,
                         validator.ValidateJsonResponseIsOk(message));
    ASSERT_EQ(response.json_value.GetValue("ErrorNumber"), 0);
    ASSERT_EQ(response.json_value.GetValue("ErrorMessage"), "");
  }
  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_THAT(validator.ValidateJsonResponseHasError(message, 1),
                StatusIs(absl::StatusCode::kInvalidArgument,
                         HasSubstr("Expected ErrorNumber=1;")));
  }
}

// `message` is OK, with no Alpaca error, nor a value.
void ExpectAlpacaResponseWithoutErrorOrValue(const std::string& message) {
  ExpectAlpacaResponseWithoutError(message);
  if (HasTestFailed()) return;

  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_OK(validator.ValidateValuelessResponse(message));
  }

  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_THAT(
        validator.ValidateValueResponse(message),
        StatusIs(
            absl::StatusCode::kInvalidArgument,
            HasSubstr(
                "Expected the JSON body object to have a Value property")));
  }

  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_THAT(
        validator.ValidateArrayValueResponse(message),
        StatusIs(
            absl::StatusCode::kInvalidArgument,
            HasSubstr(
                "Expected the JSON body object to have a Value property")));
  }

  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_THAT(
        validator.ValidateIntArrayResponse(message),
        StatusIs(
            absl::StatusCode::kInvalidArgument,
            HasSubstr(
                "Expected the JSON body object to have a Value property")));
  }
}

// `message` is OK, with Alpaca error number 1, with the assumption that there
// is no Value property as a result.
void ExpectAlpacaResponseWithError1(const std::string& message,
                                    const std::string& error_message) {
  ExpectAlpacaResponseWellFormed(message);
  if (HasTestFailed()) return;

  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_OK_AND_ASSIGN(auto response,
                         validator.ValidateJsonResponseHasError(message, 1));
    ASSERT_EQ(response.json_value.GetValue("ErrorMessage"), error_message);
  }

  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_THAT(validator.ValidateJsonResponseHasError(message, 2),
                StatusIs(absl::StatusCode::kInvalidArgument,
                         HasSubstr("Expected ErrorNumber=2;")));
  }

  const auto expected_error =
      StatusIs(absl::StatusCode::kInvalidArgument,
               HasSubstr(absl::StrCat("Expected no ASCOM error, got:\n",
                                      " ErrorNumber: 1\n",
                                      "ErrorMessage: ", error_message)));
  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    EXPECT_THAT(validator.ValidateJsonResponseIsOk(message), expected_error);
  }
  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    EXPECT_THAT(validator.ValidateValuelessResponse(message), expected_error);
  }
  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    EXPECT_THAT(validator.ValidateValueResponse(message), expected_error);
  }
  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    EXPECT_THAT(validator.ValidateArrayValueResponse(message), expected_error);
  }
  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    EXPECT_THAT(validator.ValidateIntArrayResponse(message), expected_error);
  }
}

TEST(AlpacaResponseValidatorTest, SetTransactionIdsFromAlpacaRequest) {
  AlpacaResponseValidator validator;

  {
    AlpacaRequest request;
    validator.SetTransactionIdsFromAlpacaRequest(request);
  }

  EXPECT_EQ(validator.expected_client_transaction_id, std::nullopt);
  EXPECT_EQ(validator.expected_server_transaction_id, std::nullopt);

  {
    AlpacaRequest request;
    request.set_client_transaction_id(123);
    validator.SetTransactionIdsFromAlpacaRequest(request);
  }

  EXPECT_EQ(validator.expected_client_transaction_id, 123);
  EXPECT_EQ(validator.expected_server_transaction_id, std::nullopt);
  validator.IncrementExpectedTransactionIds();
  EXPECT_EQ(validator.expected_client_transaction_id, 124);
  EXPECT_EQ(validator.expected_server_transaction_id, std::nullopt);

  {
    AlpacaRequest request;
    request.set_server_transaction_id(1234);
    validator.SetTransactionIdsFromAlpacaRequest(request);
  }

  EXPECT_EQ(validator.expected_client_transaction_id, std::nullopt);
  EXPECT_EQ(validator.expected_server_transaction_id, 1234);
  validator.IncrementExpectedTransactionIds();
  EXPECT_EQ(validator.expected_client_transaction_id, std::nullopt);
  EXPECT_EQ(validator.expected_server_transaction_id, 1235);

  {
    AlpacaRequest request;
    request.set_client_transaction_id(999);
    request.set_server_transaction_id(777);
    validator.SetTransactionIdsFromAlpacaRequest(request);
  }

  EXPECT_EQ(validator.expected_client_transaction_id, 999);
  EXPECT_EQ(validator.expected_server_transaction_id, 777);
  validator.IncrementExpectedTransactionIds();
  EXPECT_EQ(validator.expected_client_transaction_id, 1000);
  EXPECT_EQ(validator.expected_server_transaction_id, 778);
}

TEST(AlpacaResponseValidatorTest, SetClientTransactionIdFromRequest) {
  AlpacaResponseValidator validator;
  {
    mcucore::test::HttpRequest request("/foo");
    request.AddParameterIfUnset("ClientTransactionID", "321");
    validator.SetClientTransactionIdFromRequest(request);
  }
  EXPECT_EQ(validator.expected_client_transaction_id, 321);
  {
    mcucore::test::HttpRequest request("/foo");
    validator.SetClientTransactionIdFromRequest(request);
  }
  EXPECT_EQ(validator.expected_client_transaction_id, std::nullopt);
}

TEST(AlpacaResponseValidatorTest, ValidateResponseIsOk) {
  ASSERT_OK_AND_ASSIGN(const auto message,
                       mcucore::test::AssembleHttpResponseMessage(
                           200, {{"Content-Type", "application/json"}},
                           R"json(
            {
              "ClientTransactionID": 100,
              "ServerTransactionID": 200,
              "ErrorNumber": 0,
              "ErrorMessage": ""
            }
            )json"));
  EXPECT_OK(HttpResponse::Make(message));

  AlpacaResponseValidator validator;
  validator.expected_client_transaction_id = 100;
  validator.expected_server_transaction_id = 200;
  validator.last_server_transaction_id = 50;

  ASSERT_OK_AND_ASSIGN(auto response, validator.ValidateResponseIsOk(message));
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_TRUE(response.json_value.is_object());
}

TEST(AlpacaResponseValidatorTest, WrongBodyType) {
  ASSERT_OK_AND_ASSIGN(
      const auto message,
      mcucore::test::AssembleHttpResponseMessage(
          200, {{"Content-Type", "application/json"}}, R"json( [] )json"));
  EXPECT_OK(HttpResponse::Make(message));

  AlpacaResponseValidator validator;
  validator.expected_client_transaction_id = 100;
  validator.expected_server_transaction_id = 200;
  validator.last_server_transaction_id = 50;

  EXPECT_THAT(
      validator.ValidateResponseIsOk(message),
      StatusIs(absl::StatusCode::kInvalidArgument,
               HasSubstr("Expected the response body to be a JSON Object")));
}

TEST(AlpacaResponseValidatorTest, MissingServerTransactionID) {
  ASSERT_OK_AND_ASSIGN(const auto message,
                       mcucore::test::AssembleHttpResponseMessage(
                           200, {{"Content-Type", "application/json"}},
                           R"json(
            {
              "ClientTransactionID": 100,
              "ErrorNumber": 0,
              "ErrorMessage": ""
            }
            )json"));
  EXPECT_OK(HttpResponse::Make(message));

  AlpacaResponseValidator validator;
  validator.expected_client_transaction_id = 100;
  validator.expected_server_transaction_id = 200;
  validator.last_server_transaction_id = 50;

  EXPECT_THAT(
      validator.ValidateResponseIsOk(message),
      StatusIs(absl::StatusCode::kFailedPrecondition,
               HasSubstr("Object does not have key 'ServerTransactionID'")));
}

TEST(AlpacaResponseValidatorTest, MissingErrorNumber) {
  ASSERT_OK_AND_ASSIGN(const auto message,
                       mcucore::test::AssembleHttpResponseMessage(
                           200, {{"Content-Type", "application/json"}},
                           R"json(
            {
              "ServerTransactionID": 200,
              "ErrorMessage": ""
            }
            )json"));
  EXPECT_OK(HttpResponse::Make(message));

  AlpacaResponseValidator validator;
  validator.last_server_transaction_id = 199;

  EXPECT_THAT(validator.ValidateResponseIsOk(message),
              StatusIs(absl::StatusCode::kFailedPrecondition,
                       HasSubstr("Object does not have key 'ErrorNumber'")));
}

TEST(AlpacaResponseValidatorTest, MissingErrorMessage) {
  ASSERT_OK_AND_ASSIGN(const auto message,
                       mcucore::test::AssembleHttpResponseMessage(
                           200, {{"Content-Type", "application/json"}},
                           R"json(
            {
              "ServerTransactionID": 200,
              "ErrorNumber": 0
            }
            )json"));
  EXPECT_OK(HttpResponse::Make(message));

  AlpacaResponseValidator validator;
  EXPECT_THAT(validator.ValidateResponseIsOk(message),
              StatusIs(absl::StatusCode::kFailedPrecondition,
                       HasSubstr("Object does not have key 'ErrorMessage'")));
}

TEST(AlpacaResponseValidatorTest, MissingClientTransactionID) {
  ASSERT_OK_AND_ASSIGN(const auto message,
                       mcucore::test::AssembleHttpResponseMessage(
                           200, {{"Content-Type", "application/json"}},
                           R"json(
            {
              "ServerTransactionID": 200,
              "ErrorNumber": 0,
              "ErrorMessage": ""
            }
            )json"));
  EXPECT_OK(HttpResponse::Make(message));

  AlpacaResponseValidator validator;
  validator.expected_client_transaction_id = 100;
  EXPECT_THAT(
      validator.ValidateResponseIsOk(message),
      StatusIs(absl::StatusCode::kFailedPrecondition,
               HasSubstr("Object does not have key 'ClientTransactionID'")));
}

TEST(AlpacaResponseValidatorTest, WrongStatusCode) {
  ASSERT_OK_AND_ASSIGN(const auto message,
                       mcucore::test::AssembleHttpResponseMessage(400, {}));
  EXPECT_OK(HttpResponse::Make(message));

  AlpacaResponseValidator validator;

  EXPECT_THAT(validator.ValidateResponseIsOk(message),
              StatusIs(absl::StatusCode::kInvalidArgument,
                       HasSubstr("Expected status_code 200, not 400;")));
}

TEST(AlpacaResponseValidatorTest, WrongStatusMessage) {
  const char kMessage[] = "HTTP/1.1 200 ok\r\n\r\n";
  EXPECT_OK(HttpResponse::Make(kMessage));

  AlpacaResponseValidator validator;

  EXPECT_THAT(validator.ValidateResponseIsOk(kMessage),
              StatusIs(absl::StatusCode::kInvalidArgument,
                       HasSubstr("Expected status_message OK, not ok;")));
}

TEST(AlpacaResponseValidatorTest, WrongServerTransactionID) {
  {
    ASSERT_OK_AND_ASSIGN(const auto message,
                         mcucore::test::AssembleHttpResponseMessage(
                             200, {{"Content-Type", "application/json"}},
                             R"json(
            {
              "ClientTransactionID": 100,
              "ServerTransactionID": 100,
              "ErrorNumber": 0,
              "ErrorMessage": ""
            }
            )json"));
    EXPECT_OK(HttpResponse::Make(message));

    AlpacaResponseValidator validator;
    validator.expected_client_transaction_id = 100;
    validator.expected_server_transaction_id = 200;
    validator.last_server_transaction_id = 50;

    EXPECT_THAT(validator.ValidateResponseIsOk(message),
                StatusIs(absl::StatusCode::kInvalidArgument,
                         HasSubstr("Expected ServerTransactionID == 200")));
  }
  {
    ASSERT_OK_AND_ASSIGN(const auto message,
                         mcucore::test::AssembleHttpResponseMessage(
                             200, {{"Content-Type", "application/json"}},
                             R"json(
            {
              "ClientTransactionID": 100,
              "ServerTransactionID": 200,
              "ErrorNumber": 0,
              "ErrorMessage": ""
            }
            )json"));
    EXPECT_OK(HttpResponse::Make(message));

    AlpacaResponseValidator validator;
    validator.expected_client_transaction_id = 100;
    validator.expected_server_transaction_id = 200;
    validator.last_server_transaction_id = 200;

    EXPECT_THAT(validator.ValidateResponseIsOk(message),
                StatusIs(absl::StatusCode::kInvalidArgument,
                         HasSubstr("Expected ServerTransactionID >= 201")));
  }
}

TEST(AlpacaResponseValidatorTest, WrongErrorNumber) {
  ASSERT_OK_AND_ASSIGN(const auto message,
                       mcucore::test::AssembleHttpResponseMessage(
                           200, {{"Content-Type", "application/json"}},
                           R"json(
            {
              "ServerTransactionID": 1,
              "ErrorNumber": 0,
              "ErrorMessage": "non-empty"
            }
            )json"));
  EXPECT_OK(HttpResponse::Make(message));

  AlpacaResponseValidator validator;
  validator.expected_server_transaction_id = kExpectedServerTransactionId;

  EXPECT_THAT(
      validator.ValidateResponseIsOk(message),
      StatusIs(absl::StatusCode::kInvalidArgument,
               HasSubstr("Has an ErrorMessage, but ErrorNumber is zero;")));
}

TEST(AlpacaResponseValidatorTest, WrongClientTransactionID) {
  ASSERT_OK_AND_ASSIGN(const auto message,
                       mcucore::test::AssembleHttpResponseMessage(
                           200, {{"Content-Type", "application/json"}},
                           R"json(
            {
              "ClientTransactionID": 200,
              "ServerTransactionID": 200,
              "ErrorNumber": 0,
              "ErrorMessage": ""
            }
            )json"));
  EXPECT_OK(HttpResponse::Make(message));
  {
    AlpacaResponseValidator validator;
    validator.expected_client_transaction_id = 100;
    validator.expected_server_transaction_id = 200;
    validator.last_server_transaction_id = 50;

    EXPECT_THAT(
        validator.ValidateResponseIsOk(message),
        StatusIs(
            absl::StatusCode::kInvalidArgument,
            HasSubstr("Expected ClientTransactionID == 100; received 200")));
  }
  {
    // ClientTransactionID is wrong in the sense that it wasn't expected.
    AlpacaResponseValidator validator;
    validator.expected_client_transaction_id = std::nullopt;
    validator.expected_server_transaction_id = 200;
    validator.last_server_transaction_id = 50;

    EXPECT_THAT(
        validator.ValidateResponseIsOk(message),
        StatusIs(
            absl::StatusCode::kFailedPrecondition,
            "Test must set appropriate expectation for ClientTransactionID"));
  }
}

TEST(AlpacaResponseValidatorTest, ValidateJsonResponseWithoutValue) {
  {
    ASSERT_OK_AND_ASSIGN(const auto message,
                         mcucore::test::AssembleHttpResponseMessage(
                             200, {{"Content-Type", "application/json"}},
                             R"json(
            {
              "ServerTransactionID": 1,
              "ErrorNumber": 0,
              "ErrorMessage": ""
            }
            )json"));
    ExpectAlpacaResponseWithoutErrorOrValue(message);
  }
  {
    ASSERT_OK_AND_ASSIGN(const auto message,
                         mcucore::test::AssembleHttpResponseMessage(
                             200, {{"Content-Type", "application/json"}},
                             R"json(
            {
              "ServerTransactionID": 1,
              "ErrorNumber": 1,
              "ErrorMessage": "message"
            }
            )json"));
    ExpectAlpacaResponseWithError1(message, "message");
  }
  {
    ASSERT_OK_AND_ASSIGN(const auto message,
                         mcucore::test::AssembleHttpResponseMessage(
                             200, {{"Content-Type", "application/json"}},
                             R"json(
            {
              "ServerTransactionID": 1,
              "ErrorNumber": 1,
              "ErrorMessage": ""
            }
            )json"));
    ExpectAlpacaResponseWithError1(message, "");
  }
}

TEST(AlpacaResponseValidatorTest, ValidateJsonResponseWithStringValue) {
  ASSERT_OK_AND_ASSIGN(const auto message,
                       mcucore::test::AssembleHttpResponseMessage(
                           200, {{"Content-Type", "application/json"}},
                           R"json(
            {
              "Value": "the value",
              "ServerTransactionID": 1,
              "ErrorNumber": 0,
              "ErrorMessage": ""
            }
            )json"));
  ExpectAlpacaResponseWithoutError(message);
  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_THAT(validator.ValidateValueResponse(message),
                IsOkAndHolds("the value"));
  }

  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_THAT(
        validator.ValidateValuelessResponse(message),
        StatusIs(
            absl::StatusCode::kInvalidArgument,
            HasSubstr(
                "Expected the JSON body object to NOT have a Value property")));
  }

  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_THAT(validator.ValidateArrayValueResponse(message),
                StatusIs(absl::StatusCode::kInvalidArgument,
                         HasSubstr("JSON value does not have the desired type "
                                   "(JsonValue::kArray)")));
  }

  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_THAT(validator.ValidateIntArrayResponse(message),
                StatusIs(absl::StatusCode::kInvalidArgument,
                         HasSubstr("JSON value does not have the desired type "
                                   "(JsonValue::kArray)")));
  }
}

TEST(AlpacaResponseValidatorTest, ValidateJsonResponseWithStringArrayValue) {
  ASSERT_OK_AND_ASSIGN(const auto message,
                       mcucore::test::AssembleHttpResponseMessage(
                           200, {{"Content-Type", "application/json"}},
                           R"json(
            {
              "Value": ["the value"],
              "ServerTransactionID": 1,
              "ErrorNumber": 0,
              "ErrorMessage": ""
            }
            )json"));
  ExpectAlpacaResponseWithoutError(message);
  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_OK_AND_ASSIGN(auto value_jv,
                         validator.ValidateValueResponse(message));
    ASSERT_TRUE(value_jv.is_array());
    ASSERT_EQ(value_jv.size(), 1);
    ASSERT_EQ(value_jv.GetElement(0), "the value");
  }

  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_OK_AND_ASSIGN(auto value_jv,
                         validator.ValidateArrayValueResponse(message));
    ASSERT_TRUE(value_jv.is_array());
    ASSERT_EQ(value_jv.size(), 1);
    ASSERT_EQ(value_jv.GetElement(0), "the value");
  }

  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_THAT(validator.ValidateIntArrayResponse(message),
                StatusIs(absl::StatusCode::kInvalidArgument,
                         HasSubstr("JSON value does not have the desired type "
                                   "(JsonValue::kInteger)")));
  }
}

TEST(AlpacaResponseValidatorTest, ValidateJsonResponseWithIntArrayValue) {
  ASSERT_OK_AND_ASSIGN(const auto message,
                       mcucore::test::AssembleHttpResponseMessage(
                           200, {{"Content-Type", "application/json"}},
                           R"json(
            {
              "Value": [1, 2],
              "ServerTransactionID": 1,
              "ErrorNumber": 0,
              "ErrorMessage": ""
            }
            )json"));
  ExpectAlpacaResponseWithoutError(message);
  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_OK_AND_ASSIGN(auto value_jv,
                         validator.ValidateValueResponse(message));
    ASSERT_TRUE(value_jv.is_array());
    ASSERT_EQ(value_jv.size(), 2);
    ASSERT_EQ(value_jv.GetElement(0), 1);
    ASSERT_EQ(value_jv.GetElement(1), 2);
  }
  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_OK_AND_ASSIGN(auto value_jv,
                         validator.ValidateArrayValueResponse(message));
    ASSERT_TRUE(value_jv.is_array());
    ASSERT_EQ(value_jv.size(), 2);
    ASSERT_EQ(value_jv.GetElement(0), 1);
    ASSERT_EQ(value_jv.GetElement(1), 2);
  }
  {
    AlpacaResponseValidator validator;
    validator.expected_server_transaction_id = kExpectedServerTransactionId;
    ASSERT_OK_AND_ASSIGN(std::vector<int64_t> vector_of_int,
                         validator.ValidateIntArrayResponse(message));
    ASSERT_THAT(vector_of_int, ElementsAre(1, 2));
  }
}

}  // namespace
}  // namespace test
}  // namespace alpaca
