// Tests of TinyAlpacaServerBase, via the concrete sub-class
// TestTinyAlpacaServer.

#include <iostream>
#include <memory>
#include <string>

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "alpaca_devices.h"
#include "alpaca_discovery_server.h"
#include "alpaca_response.h"
#include "constants.h"
#include "device_info.h"
#include "device_interface.h"
#include "device_types/device_impl_base.h"
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
namespace {

using ::alpaca::ServerDescription;
using ::testing::IsEmpty;
using ::testing::StartsWith;

constexpr int kDeviceNumber = 87405;
constexpr int kClientId = 91240;
constexpr int kClientTransactionId = 42050;
constexpr int kServerTransactionId = 54981;

TAS_DEFINE_LITERAL(ServerName, "OurServer");
TAS_DEFINE_LITERAL(Manufacturer, "Us");
TAS_DEFINE_LITERAL(ManufacturerVersion, "0.0.1");
TAS_DEFINE_LITERAL(DeviceLocation, "Right Here");

class TinyAlpacaServerBaseTest : public testing::Test {
 protected:
  TinyAlpacaServerBaseTest()
      : server_description_({.server_name = ServerName(),
                             .manufacturer = Manufacturer(),
                             .manufacturer_version = ManufacturerVersion(),
                             .location = DeviceLocation()}),
        server_(server_description_, ArrayView<DeviceInterface*>()) {}
  void SetUp() override {}

  ServerDescription server_description_;
  TestTinyAlpacaServer server_;
};

TEST_F(TinyAlpacaServerBaseTest, InitializeAndMaintain) {
  EXPECT_TRUE(server_.Initialize());
  server_.MaintainDevices();
}

TEST_F(TinyAlpacaServerBaseTest, EmptyConnection) {
  EXPECT_TRUE(server_.Initialize());

  auto result = server_.AnnounceConnect("");
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_THAT(result.output, IsEmpty());
  EXPECT_FALSE(result.connection_closed);

  result = server_.AnnounceCanRead("");
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_THAT(result.output, IsEmpty());
  EXPECT_FALSE(result.connection_closed);

  result = server_.AnnounceHalfClosed();
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_THAT(result.output, IsEmpty());
  EXPECT_TRUE(result.connection_closed);
}

TEST_F(TinyAlpacaServerBaseTest, Setup) {
  EXPECT_TRUE(server_.Initialize());

  // We include two unsupported (ignored) headers, including one with a very
  // large value, to verify that they are ignored, including skipping over a
  // header value that is too long to fit into the decoder's input buffer.
  const auto full_request =
      absl::StrCat("GET /setup HTTP/1.1\r\n",  // Line break......
                   "Host: example.com\r\n",    // Line break
                   "Foo-Bar-Baz:", std::string(10000, '0'), "\r\n",  //
                   "\r\n");

  auto result = server_.AnnounceConnect("");
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_THAT(result.output, IsEmpty());
  EXPECT_FALSE(result.connection_closed);

  result = server_.AnnounceCanRead("");
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_THAT(result.output, IsEmpty());
  EXPECT_FALSE(result.connection_closed);

  result = server_.AnnounceCanRead(full_request);
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_FALSE(result.output.empty());
  EXPECT_FALSE(result.connection_closed);

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(result.output));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_TRUE(response.HasHeader("CONTENT-LENGTH"));
  EXPECT_TRUE(response.HasHeaderValue("content-Type", "text/html"));
  EXPECT_THAT(response.body_and_beyond, StartsWith("<html>"));
}

TEST_F(TinyAlpacaServerBaseTest, KnownHeaderTooLarge) {
  EXPECT_TRUE(server_.Initialize());

  const auto full_request =
      absl::StrCat("GET /setup HTTP/1.1\r\n",  // Line break
                   "Host: example.com\r\n",    // Line break
                   "Content-Length:", std::string(1000, '0'),
                   "\r\n",  // Line break
                   "\r\n");

  auto result = server_.AnnounceConnect("");
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_THAT(result.output, IsEmpty());
  EXPECT_FALSE(result.connection_closed);

  // Decode just the HTTP method first.
  result = server_.AnnounceCanRead(full_request.substr(0, 4));
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_THAT(result.output, IsEmpty());
  EXPECT_FALSE(result.connection_closed);

  // Then decode the rest of the request.
  result = server_.AnnounceCanRead(full_request.substr(4));
  EXPECT_THAT(result.remaining_input, StartsWith("000"));
  EXPECT_FALSE(result.output.empty());
  EXPECT_TRUE(result.connection_closed);

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(result.output));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 431);
  EXPECT_EQ(response.status_message, "Request Header Fields Too Large");
  EXPECT_THAT(response.body_and_beyond, IsEmpty());
}

TEST_F(TinyAlpacaServerBaseTest, ConfiguredDevices) {
  EXPECT_TRUE(server_.Initialize());

  const std::string full_request(
      "GET /management/v1/configureddevices HTTP/1.1\r\n"
      "\r\n");

  auto result = server_.AnnounceConnect(full_request);
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_FALSE(result.output.empty());
  EXPECT_FALSE(result.connection_closed);

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(result.output));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  ASSERT_FALSE(response.json_value.is_unset());

  auto json_body = response.json_value;
  ASSERT_TRUE(json_body.GetValue("ServerTransactionID").is_integer());
  ASSERT_FALSE(json_body.HasKey("ClientTransactionID"));
  ASSERT_FALSE(json_body.HasKey("ErrorNumber"));
  ASSERT_FALSE(json_body.HasKey("ErrorMessage"));
  ASSERT_TRUE(json_body.GetValue("Value").is_array());
  auto value_array = json_body.GetValue("Value");
  ASSERT_THAT(value_array.as_array(), IsEmpty());
}

}  // namespace
}  // namespace test
}  // namespace alpaca
