// Tests of TinyAlpacaDeviceServer.

#include <McuCore.h>
#include <McuNet.h>

#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "device_interface.h"
#include "extras/test_tools/decode_and_dispatch_test_base.h"
#include "extras/test_tools/test_tiny_alpaca_server.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "literals.h"
#include "mcucore/extras/test_tools/http_request.h"
#include "mcucore/extras/test_tools/http_response.h"
#include "server_description.h"

namespace alpaca {
namespace test {
namespace {

using ::alpaca::ServerDescription;
using ::mcucore::test::HttpRequest;
using ::mcucore::test::HttpResponse;
using ::testing::IsEmpty;
using ::testing::StartsWith;

constexpr int kDeviceNumber = 87405;
constexpr int kClientId = 91240;
constexpr int kClientTransactionId = 42050;
constexpr int kServerTransactionId = 54981;

#define kServerName "Device-less Server"
#define kManufacturer "No One"
#define kManufacturerVersion "0.0.0"
#define kDeviceLocation "No Where"

const ServerDescription kServerDescription  // NOLINT
    {
        .server_name = MCU_FLASHSTR(kServerName),
        .manufacturer = MCU_FLASHSTR(kManufacturer),
        .manufacturer_version = MCU_FLASHSTR(kManufacturerVersion),
        .location = MCU_FLASHSTR(kDeviceLocation),
    };

class TinyAlpacaServerBaseExplicitLifecycleTest
    : public DecodeAndDispatchTestBase {
 protected:
  void SetUp() override {
    // NOT calling base class SetUp, but do call PrepareEeprom because we need
    // that else we crash.
    PrepareEeprom();
  }

  const ServerDescription& GetServerDescription() override {
    return kServerDescription;
  }

  mcucore::ArrayView<DeviceInterface*> GetDeviceInterfaces() override {
    return {};
  }
  std::string_view GetDeviceTypeName() override { return ""; }
};

TEST_F(TinyAlpacaServerBaseExplicitLifecycleTest, CreateInitializeAndMaintain) {
  server_ = CreateServer();

  server_->ValidateAndReset();
  server_->InitializeForServing();

  // MaintainDevices will be called many times, so let's do so more than once.
  server_->MaintainDevices();
  server_->MaintainDevices();
}

class TinyAlpacaServerBaseTest : public DecodeAndDispatchTestBase {
 protected:
  mcucore::ArrayView<DeviceInterface*> GetDeviceInterfaces() override {
    return {};
  }

  std::string_view GetDeviceTypeName() override { return ""; }
};

TEST_F(TinyAlpacaServerBaseTest, OpenAndCloseConnection) {
  // We send no data, which should be tolerated.
  ConnectionResult result =
      SendAndReceive("", /*half_close_when_drained=*/false);
  EXPECT_FALSE(result.connection_closed);
  EXPECT_TRUE(result.output.empty());
  EXPECT_TRUE(result.remaining_input.empty());
  server_->MaintainDevices();
  server_->AnnounceDisconnect();
  server_->MaintainDevices();
}

TEST_F(TinyAlpacaServerBaseTest, ServerStatus) {
  // We include two unsupported (ignored) headers, including one with a very
  // large value, to verify that they are ignored, including skipping over a
  // header value that is too long to fit into the decoder's input buffer.
  const auto full_request =
      absl::StrCat("GET /?Clientid=123 HTTP/1.1\r\n",  // Line break......
                   "Host: example.com\r\n",            // Line break
                   "Foo-Bar-Baz:", std::string(10000, '0'), "\r\n",  //
                   "\r\n");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripSoleRequest(full_request));
  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(response_str));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_FALSE(response.HasHeader("CONTENT-LENGTH"));
  EXPECT_TRUE(response.HasHeaderValue("content-Type", "text/html"));
  EXPECT_THAT(response.body_and_beyond, StartsWith("<html>"));
  LOG(INFO) << "response.body_and_beyond:\n\n"
            << response.body_and_beyond << "\n\n";
}

TEST_F(TinyAlpacaServerBaseTest, Setup) {
  // We include two unsupported (ignored) headers, including one with a very
  // large value, to verify that they are ignored, including skipping over a
  // header value that is too long to fit into the decoder's input buffer.
  const auto full_request =
      absl::StrCat("GET /setup HTTP/1.1\r\n",  // Line break......
                   "Host: example.com\r\n",    // Line break
                   "Foo-Bar-Baz:", std::string(10000, '0'), "\r\n",  //
                   "\r\n");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripSoleRequest(full_request));
  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(response_str));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_TRUE(response.HasHeader("CONTENT-LENGTH"));
  EXPECT_TRUE(response.HasHeaderValue("content-Type", "text/html"));
  EXPECT_THAT(response.body_and_beyond, StartsWith("<html>"));
}

TEST_F(TinyAlpacaServerBaseTest, KnownHeaderTooLarge) {
  const auto full_request =
      absl::StrCat("GET /setup HTTP/1.1\r\n",  // Line break
                   "Host: example.com\r\n",    // Line break
                   "Content-Length:", std::string(1000, '0'),
                   "\r\n",  // Line break
                   "\r\n");

  auto result = server_->AnnounceConnect("");
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_THAT(result.output, IsEmpty());
  EXPECT_FALSE(result.connection_closed);

  // Decode just the HTTP method first.
  result = server_->AnnounceCanRead(full_request.substr(0, 4));
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_THAT(result.output, IsEmpty());
  EXPECT_FALSE(result.connection_closed);

  // Then decode the rest of the request.
  result = server_->AnnounceCanRead(full_request.substr(4));
  EXPECT_THAT(result.remaining_input, StartsWith("000000000000000"));
  EXPECT_FALSE(result.output.empty());
  EXPECT_TRUE(result.connection_closed);

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(result.output));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 431);
  EXPECT_EQ(response.status_message, "Request Header Fields Too Large");
  EXPECT_THAT(response.body_and_beyond, IsEmpty());
}

TEST_F(TinyAlpacaServerBaseTest, NoConfiguredDevices) {
  HttpRequest request("/management/v1/configureddevices");
  ASSERT_OK_AND_ASSIGN(auto response_str,
                       RoundTripSoleRequest(request.ToString()));
  response_validator_.SetClientTransactionIdFromRequest(request);

  // There aren't any devices, so the response should be OK, but the Value array
  // should be empty.
  ASSERT_OK_AND_ASSIGN(
      auto configured_devices_jv_array,
      response_validator_.ValidateArrayValueResponse(response_str));
  ASSERT_THAT(configured_devices_jv_array.as_array(), IsEmpty());
}

}  // namespace
}  // namespace test
}  // namespace alpaca
