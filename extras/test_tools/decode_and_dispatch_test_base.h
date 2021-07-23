#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_DECODE_AND_DISPATCH_TEST_BASE_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_DECODE_AND_DISPATCH_TEST_BASE_H_

// Provides a base class for testing the decoding and dispatching of Alpaca
// requests for a single device instance. This is basically parts of
// TinyAlpacaServer, but skipping dealing with PlatformEthernet, for which I
// don't yet have a fake implemented.

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// TODO Remove excessive headers.

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "alpaca_devices.h"
#include "alpaca_discovery_server.h"
#include "alpaca_response.h"
#include "constants.h"
#include "device_interface.h"
#include "device_types/device_impl_base.h"
#include "extras/test_tools/http_request.h"
#include "extras/test_tools/http_response.h"
#include "extras/test_tools/mock_device_interface.h"
#include "extras/test_tools/mock_switch_group.h"
#include "extras/test_tools/print_to_std_string.h"
#include "extras/test_tools/sample_printable.h"
#include "extras/test_tools/test_tiny_alpaca_server.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "literals.h"
#include "request_listener.h"
#include "server_connection.h"
#include "server_description.h"
#include "server_sockets_and_connections.h"
#include "tiny_alpaca_server.h"
#include "utils/array_view.h"
#include "utils/platform.h"
#include "utils/platform_ethernet.h"
#include "utils/status.h"
#include "utils/string_view.h"

namespace alpaca {
namespace test {

class DecodeAndDispatchTestBase : public testing::Test {
 protected:
  DecodeAndDispatchTestBase();

  // May only be called before SetUp().
  void AddDeviceInterface(DeviceInterface& device_interface);

  // Initializes the server description, and creates the TestTinyAlpacaServer
  // instance.
  void SetUp() override;

  // Fills the provided description.
  virtual void FillServerDescription(ServerDescription& description);

  // Creates the test server; calls GetDeviceInterfaces to get the set of
  // supported devices.
  virtual std::unique_ptr<TestTinyAlpacaServer> CreateServer();

  // If returns true, then SetUp will call Initialize on the server.
  virtual bool InitializeServerAutomatically() { return true; }

  // Return the set of devices to include in the server.
  virtual ArrayView<DeviceInterface*> GetDeviceInterfaces();

  // These returns the values to be used on the next device specific request.
  virtual std::string_view GetDeviceTypeName();
  virtual uint32_t GetDeviceNumber();

  // The value to be used for the clientid parameter.
  virtual uint32_t GetClientId();

  // This will be called once to create the HttpRequest instance; that instance
  // will be used by the Validate* methods to determine if the response includes
  // the correct id.
  virtual uint32_t GetNextClientTransactionId();

  virtual void AddCommonParts(HttpRequest& request);

  virtual HttpRequest GenerateConfiguredDevicesRequest();

  // These create API requests for the device specified by the methods above.
  virtual HttpRequest GenerateDeviceSetupRequest();
  virtual HttpRequest GenerateDeviceApiRequest(std::string_view ascom_method);
  HttpRequest GenerateDeviceApiPutRequest(std::string_view ascom_method) {
    auto req = GenerateDeviceApiRequest(ascom_method);
    req.method = "PUT";
    return req;
  }

  // Create a new connection if necessary, provide the input to the server until
  // it drains it, then optionally half-close the connection.
  virtual ConnectionResult SendAndReceive(const std::string& input,
                                          bool half_close_when_drained);

  // Create a new connection if necessary, send a single, complete request, read
  // and return the response. An error is returned if the entire request is not
  // consumed.
  virtual absl::StatusOr<std::string> RoundTripRequest(
      const std::string& request, bool half_close_when_drained);
  virtual absl::StatusOr<std::string> RoundTripRequest(
      HttpRequest& request, bool half_close_when_drained);

  // Create a new connection, send the request, read the response, expecting
  // that the entire request is consumed and that the server closes the
  // connection. Returns all of the output from the connection, but does not
  // interpret it in any way.
  virtual absl::StatusOr<std::string> RoundTripSoleRequest(
      const std::string& request);
  virtual absl::StatusOr<std::string> RoundTripSoleRequest(
      HttpRequest& request);

  // Validates that an HTTP response is 200 OK. If the body is JSON, verifies
  // that it is a JSON object, and that any standard Alpaca properties present
  // have their expected types (e.g. "ErrorMessage" has a value of type string).
  absl::StatusOr<HttpResponse> ValidateResponseIsOk(
      const std::string& response_string);

  // Extends ValidateResponseIsOk, validating that the response contains a JSON
  // body and that there is not an error in the JSON body; validates also that
  // if the request contained a ClientTransactionID, it is returned in the JSON
  // body.
  absl::StatusOr<HttpResponse> ValidateJsonResponseIsOk(
      const HttpRequest& request, const std::string& response_string);

  // Validates that an HTTP response is 200 OK, but has an ASCOM error.
  absl::StatusOr<HttpResponse> ValidateJsonResponseHasError(
      const HttpRequest& request, const std::string& response_string,
      int expected_error_number);

  // Extends ValidateJsonResponseIsOk by verifying that the JSON object has no
  // Value property.
  absl::StatusOr<HttpResponse> ValidateValuelessResponse(
      const HttpRequest& request, const std::string& response_string);

  // Returns the value of the Value property if the response is OK and contains
  // a Value property, else an error with details.
  absl::StatusOr<JsonValue> ValidateValueResponse(
      const HttpRequest& request, const std::string& response_string);

  // Returns the value of the Value property if the response is OK and contains
  // the Value property as an array, else an error with details.
  absl::StatusOr<JsonValue> ValidateArrayValueResponse(
      const HttpRequest& request, const std::string& response_string);

  // Returns the Value array if the response is OK and contains the expected
  // array of integers, else an error with details.
  absl::StatusOr<std::vector<int64_t>> ValidateIntArrayResponse(
      const HttpRequest& request, const std::string& response_string);

  ServerDescription server_description_;
  std::unique_ptr<TestTinyAlpacaServer> server_;
  uint32_t last_server_transaction_id_ = 0;
  int last_client_transaction_id_ = 0;

 private:
  std::vector<DeviceInterface*> device_interfaces_;
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_DECODE_AND_DISPATCH_TEST_BASE_H_
