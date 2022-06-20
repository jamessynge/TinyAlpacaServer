#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_ALPACA_RESPONSE_VALIDATOR_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_ALPACA_RESPONSE_VALIDATOR_H_

// Test helper for validating HTTP responses from Tiny Alpaca Server, and for
// extracting information (e.g. the Value property from the JSON body).

#include <cstdint>
#include <optional>
#include <string>

#include "absl/status/statusor.h"
#include "alpaca_request.h"
#include "mcucore/extras/test_tools/http_request.h"
#include "mcucore/extras/test_tools/http_response.h"

namespace alpaca {
namespace test {

struct AlpacaResponseValidator {
  void SetTransactionIdsFromAlpacaRequest(const AlpacaRequest& request);
  void SetClientTransactionIdFromRequest(
      const mcucore::test::HttpRequest& request);
  void IncrementExpectedTransactionIds();

  // Validates that an HTTP response is 200 OK. If the body is JSON, verifies
  // that it is a JSON object, and that any standard Alpaca properties present
  // have their expected types (e.g. "ErrorMessage" has a value of type string).
  absl::StatusOr<mcucore::test::HttpResponse> ValidateResponseIsOk(
      const std::string& response_string);

  // Extends ValidateResponseIsOk, validating that the response contains a JSON
  // body and that there is not an error in the JSON body; validates also that
  // if the request contained a ClientTransactionID, it is returned in the JSON
  // body.
  absl::StatusOr<mcucore::test::HttpResponse> ValidateJsonResponseIsOk(
      const std::string& response_string);

  // Validates that an HTTP response is 200 OK, but has an ASCOM error.
  absl::StatusOr<mcucore::test::HttpResponse> ValidateJsonResponseHasError(
      const std::string& response_string, int expected_error_number);

  // Extends ValidateJsonResponseIsOk by verifying that the JSON object has no
  // Value property.
  absl::StatusOr<mcucore::test::HttpResponse> ValidateValuelessResponse(
      const std::string& response_string);

  // Returns the value of the Value property if the response is OK and contains
  // a Value property, else an error with details.
  absl::StatusOr<mcucore::test::JsonValue> ValidateValueResponse(
      const std::string& response_string);

  // Returns the value of the Value property if the response is OK and contains
  // the Value property as an array, else an error with details.
  absl::StatusOr<mcucore::test::JsonValue> ValidateArrayValueResponse(
      const std::string& response_string);

  // Returns the Value array if the response is OK and contains the expected
  // array of integers, else an error with details.
  absl::StatusOr<std::vector<int64_t>> ValidateIntArrayResponse(
      const std::string& response_string);

  // Set by SetClientTransactionIdFromRequest, incremented by
  // IncrementTransactionIds, checked by ValidateResponseIsOk.
  std::optional<uint32_t> expected_client_transaction_id;

  // Set directly by test, incremented by IncrementTransactionIds, checked by
  // ValidateResponseIsOk.
  std::optional<uint32_t> expected_server_transaction_id;

  // Set and checked by ValidateResponseIsOk
  std::optional<uint32_t> last_server_transaction_id;
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_ALPACA_RESPONSE_VALIDATOR_H_
