#include "extras/test_tools/alpaca_response_validator.h"

#include <McuCore.h>

#include <optional>
#include <string>

#include "absl/status/status.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "mcucore/extras/test_tools/http_request.h"
#include "mcucore/extras/test_tools/http_response.h"
#include "mcucore/extras/test_tools/json_decoder.h"
#include "util/task/status_macros.h"

namespace alpaca {
namespace test {

using ::mcucore::test::HttpResponse;
using ::mcucore::test::JsonValue;

constexpr char kClientTransactionIDName[] = "ClientTransactionID";
constexpr char kServerTransactionIDName[] = "ServerTransactionID";
constexpr char kErrorNumberName[] = "ErrorNumber";
constexpr char kErrorMessageName[] = "ErrorMessage";

void AlpacaResponseValidator::SetTransactionIdsFromAlpacaRequest(
    const AlpacaRequest& request) {
  if (request.have_client_transaction_id) {
    expected_client_transaction_id = request.client_transaction_id;
  } else {
    expected_client_transaction_id = std::nullopt;
  }
  if (request.have_server_transaction_id) {
    expected_server_transaction_id = request.server_transaction_id;
  } else {
    expected_server_transaction_id = std::nullopt;
  }
}

void AlpacaResponseValidator::SetClientTransactionIdFromRequest(
    const mcucore::test::HttpRequest& request) {
  if (request.HasParameter(kClientTransactionIDName)) {
    auto param_value =
        request.GetParameter(kClientTransactionIDName).ValueOrDie();
    int sent_txn_id;
    CHECK(absl::SimpleAtoi(param_value, &sent_txn_id));
    expected_client_transaction_id = sent_txn_id;
  } else {
    expected_client_transaction_id = std::nullopt;
  }
}

void AlpacaResponseValidator::IncrementExpectedTransactionIds() {
  if (expected_client_transaction_id.has_value()) {
    expected_client_transaction_id = 1 + expected_client_transaction_id.value();
  }
  if (expected_server_transaction_id.has_value()) {
    expected_server_transaction_id = 1 + expected_server_transaction_id.value();
  }
}

absl::StatusOr<mcucore::test::HttpResponse>
AlpacaResponseValidator::ValidateResponseIsOk(
    const std::string& response_string) {
  ASSIGN_OR_RETURN(auto resp, HttpResponse::Make(response_string));
  if (resp.status_code != 200) {
    return absl::InvalidArgumentError(
        absl::StrCat("Expected status_code 200, not ", resp.status_code,
                     "; response:\n\n", response_string, "\n"));
  }
  if (resp.status_message != "OK") {
    return absl::InvalidArgumentError(
        absl::StrCat("Expected status_message OK, not ", resp.status_message,
                     "; response:\n\n", response_string, "\n"));
  }
  if (!resp.json_value.is_unset()) {
    // The body must be a JSON Object, and must have ErrorNumber, ErrorMessage
    // and ServerTransactionID properties.
    auto body_jv = resp.json_value;
    if (!body_jv.is_object()) {
      return absl::InvalidArgumentError(
          absl::StrCat("Expected the response body to be a JSON Object, not ",
                       body_jv.ToDebugString()));
    }
    ASSIGN_OR_RETURN(
        const auto error_number_jv,
        body_jv.GetValueOfType(kErrorNumberName, JsonValue::kInteger));
    ASSIGN_OR_RETURN(
        const auto error_message_jv,
        body_jv.GetValueOfType(kErrorMessageName, JsonValue::kString));
    ASSIGN_OR_RETURN(
        const auto server_transaction_id_jv,
        body_jv.GetValueOfType(kServerTransactionIDName, JsonValue::kInteger));

    // If there is a non-empty ErrorMessage, there must be a non-zero
    // ErrorNumber.
    if (!error_message_jv.as_string().empty() &&
        error_number_jv.as_integer() == 0) {
      return absl::InvalidArgumentError(
          absl::StrCat("Has an ", kErrorMessageName, ", but ", kErrorNumberName,
                       " is zero; JSON response: ", body_jv.ToDebugString()));
    }

    // The ClientTransactionId is optional.
    if (expected_client_transaction_id.has_value()) {
      // But we expected one in this case.
      ASSIGN_OR_RETURN(const auto client_transaction_id_jv,
                       body_jv.GetValueOfType(kClientTransactionIDName,
                                              JsonValue::kInteger));
      if (expected_client_transaction_id.has_value() &&
          expected_client_transaction_id.value() !=
              client_transaction_id_jv.as_integer()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Expected ", kClientTransactionIDName,
                         " == ", expected_client_transaction_id.value(),
                         "; received ", client_transaction_id_jv.as_integer()));
      }
    } else if (body_jv.HasKey(kClientTransactionIDName)) {
      // Not an error at the Alpaca level, but the test using this function
      // should set expected_client_transaction_id so that validation can work,
      // so we complain.
      return absl::FailedPreconditionError(
          absl::StrCat("Test must set appropriate expectation for ",
                       kClientTransactionIDName));
    }

    // The ServerTransactionID must be non-negative and increasing.
    auto recv_txn_id = server_transaction_id_jv.as_integer();
    if (expected_server_transaction_id.has_value() &&
        expected_server_transaction_id.value() != recv_txn_id) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Expected ", kServerTransactionIDName, " == ",
          expected_server_transaction_id.value(), "; received ", recv_txn_id));
    }
    auto min_expected_txn_id = 0;
    if (last_server_transaction_id.has_value()) {
      min_expected_txn_id = last_server_transaction_id.value() + 1;
    }
    last_server_transaction_id = recv_txn_id;
    if (recv_txn_id < min_expected_txn_id) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Expected ", kServerTransactionIDName, " >= ", min_expected_txn_id,
          "; received ", recv_txn_id));
    }
  }
  return resp;
}

absl::StatusOr<HttpResponse> AlpacaResponseValidator::ValidateJsonResponseIsOk(
    const std::string& response_string) {
  ASSIGN_OR_RETURN(auto resp, ValidateResponseIsOk(response_string));
  RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(resp.json_value, JsonValue::kObject);
  auto body_jv = resp.json_value;

  // Not OK if there is a non-zero error number.
  if (body_jv.HasKey(kErrorNumberName) &&
      body_jv.GetValue(kErrorNumberName).as_integer() != 0) {
    return absl::InvalidArgumentError(
        absl::StrCat("Expected no ASCOM error, got:\n ", kErrorNumberName, ": ",
                     body_jv.GetValue(kErrorNumberName).as_integer(), "\n",
                     kErrorMessageName, ": ",
                     (body_jv.HasKey(kErrorMessageName)
                          ? body_jv.GetValue(kErrorMessageName).as_string()
                          : "")));
  }
  return resp;
}

absl::StatusOr<HttpResponse>
AlpacaResponseValidator::ValidateJsonResponseHasError(
    const std::string& response_string, int expected_error_number) {
  ASSIGN_OR_RETURN(auto resp, ValidateResponseIsOk(response_string));
  RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(resp.json_value, JsonValue::kObject);
  auto body_jv = resp.json_value;
  if (body_jv.GetValue(kErrorNumberName) != expected_error_number) {
    return absl::InvalidArgumentError(
        absl::StrCat("Expected ", kErrorNumberName, "=", expected_error_number,
                     "; JSON response:\n", body_jv.ToDebugString()));
  }
  return resp;
}

absl::StatusOr<HttpResponse> AlpacaResponseValidator::ValidateValuelessResponse(
    const std::string& response_string) {
  ASSIGN_OR_RETURN(auto resp, ValidateJsonResponseIsOk(response_string));
  if (resp.json_value.HasKey("Value")) {
    return absl::InvalidArgumentError(
        "Expected the JSON body object to NOT have a Value property");
  }
  return resp;
}

absl::StatusOr<JsonValue> AlpacaResponseValidator::ValidateValueResponse(
    const std::string& response_string) {
  ASSIGN_OR_RETURN(auto resp, ValidateJsonResponseIsOk(response_string));
  if (!resp.json_value.HasKey("Value")) {
    return absl::InvalidArgumentError(
        "Expected the JSON body object to have a Value property");
  }
  return resp.json_value.GetValue("Value");
}

absl::StatusOr<JsonValue> AlpacaResponseValidator::ValidateArrayValueResponse(
    const std::string& response_string) {
  ASSIGN_OR_RETURN(auto value_jv, ValidateValueResponse(response_string));
  RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(value_jv, JsonValue::kArray);
  return value_jv;
}

absl::StatusOr<std::vector<int64_t>>
AlpacaResponseValidator::ValidateIntArrayResponse(
    const std::string& response_string) {
  ASSIGN_OR_RETURN(auto array_jv, ValidateArrayValueResponse(response_string));
  std::vector<int64_t> result;
  for (const auto& elem_jv : array_jv.as_array()) {
    RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(elem_jv, JsonValue::kInteger);
    result.push_back((elem_jv.as_integer()));
  }
  return result;
}

}  // namespace test
}  // namespace alpaca
