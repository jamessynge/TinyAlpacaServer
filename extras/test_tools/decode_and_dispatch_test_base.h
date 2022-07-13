#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_DECODE_AND_DISPATCH_TEST_BASE_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_DECODE_AND_DISPATCH_TEST_BASE_H_

// Provides a base class for testing the decoding and dispatching of Alpaca
// requests for a single device instance. This is basically parts of
// TinyAlpacaServer, but skipping dealing with PlatformEthernet, for which I
// don't yet have a fake implemented.
//
// Author: james.synge@gmail.com

#include <McuCore.h>
#include <McuNet.h>

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "device_interface.h"
#include "extras/test_tools/alpaca_response_validator.h"
#include "extras/test_tools/test_tiny_alpaca_server.h"
#include "gtest/gtest.h"
#include "literals.h"
#include "mcucore/extras/test_tools/http_request.h"
#include "mcunet/extras/test_tools/mock_platform_network.h"
#include "server_description.h"

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

  // By default clears and initializes with EepromTlv.
  virtual void PrepareEeprom();

  // Get the description of the server.
  virtual const ServerDescription& GetServerDescription();

  // Creates the test server; calls GetDeviceInterfaces to get the set of
  // supported devices.
  virtual std::unique_ptr<TestTinyAlpacaServer> CreateServer();

  // If returns true, then SetUp will call Initialize on the server.
  virtual bool InitializeServerAutomatically() { return true; }

  // Return the set of devices to include in the server.
  virtual mcucore::ArrayView<DeviceInterface*> GetDeviceInterfaces();

  // These returns the values to be used on the next device specific request.
  virtual std::string_view GetDeviceTypeName();
  virtual uint32_t GetDeviceNumber();

  // The value to be used for the clientid parameter.
  virtual uint32_t GetClientId();

  // This will be called once to create the HttpRequest instance; that instance
  // will be used by the Validate* methods to determine if the response includes
  // the correct id.
  virtual uint32_t GetNextClientTransactionId();

  // Add request headers or query params to the request.
  virtual void AddCommonParts(mcucore::test::HttpRequest& request);

  // Generate various kinds of requests...

  virtual mcucore::test::HttpRequest GenerateHomePageRequest();
  virtual mcucore::test::HttpRequest GenerateConfiguredDevicesRequest();

  // These create API requests for the device specified by the methods above.
  virtual mcucore::test::HttpRequest GenerateDeviceSetupRequest();
  virtual mcucore::test::HttpRequest GenerateDeviceApiRequest(
      std::string_view ascom_method);
  mcucore::test::HttpRequest GenerateDeviceApiPutRequest(
      std::string_view ascom_method) {
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
      mcucore::test::HttpRequest& request, bool half_close_when_drained);
  virtual absl::StatusOr<mcucore::test::JsonValue>
  RoundTripRequestWithValueResponse(mcucore::test::HttpRequest& request,
                                    bool half_close_when_drained);
  virtual absl::StatusOr<mcucore::test::JsonValue>
  RoundTripRequestWithValueResponse(mcucore::test::HttpRequest& request) {
    return RoundTripRequestWithValueResponse(request, false);
  }

  // Create a new connection, send the request, read the response, expecting
  // that the entire request is consumed and that the server closes the
  // connection. Returns all of the output from the connection, but does not
  // interpret it in any way.
  virtual absl::StatusOr<std::string> RoundTripSoleRequest(
      const std::string& request);
  virtual absl::StatusOr<std::string> RoundTripSoleRequest(
      mcucore::test::HttpRequest& request);

  // As above, but expecting the response to be an Alpaca JSON response with a
  // Value property; if that is the case, returns the value of that property.
  // Otherwise returns an error.
  virtual absl::StatusOr<mcucore::test::JsonValue>
  RoundTripSoleRequestWithValueResponse(mcucore::test::HttpRequest& request);

  mcunet::PlatformNetworkLifetime<mcunet::test::MockPlatformNetwork>
      mock_platform_network_lifetime_;
  std::unique_ptr<TestTinyAlpacaServer> server_;
  AlpacaResponseValidator response_validator_;
  uint32_t last_server_transaction_id_ = 0;
  int last_client_transaction_id_ = 0;

 private:
  std::vector<DeviceInterface*> device_interfaces_;
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_DECODE_AND_DISPATCH_TEST_BASE_H_
