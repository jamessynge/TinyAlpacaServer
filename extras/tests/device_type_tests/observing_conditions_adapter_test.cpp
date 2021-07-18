#include "device_types/observing_conditions/observing_conditions_adapter.h"

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
#include "extras/test_tools/http_request.h"
#include "extras/test_tools/http_response.h"
#include "extras/test_tools/json_decoder.h"
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
using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::StartsWith;
using ::testing::UnorderedElementsAre;

constexpr int kDeviceNumber = 331;

#define DEVICE_NAME "WeatherOracle"
#define GITHUB_LINK "https://github/jamessynge/TinyAlpacaServer"
#define DEVICE_DESCRIPTION "Super Weather"
#define SUPPORTED_ACTION "MakeItRain"
#define DEVICE_DRIVER_VERSION "99"

TAS_DEFINE_LITERAL(ServerName, "OurServer");
TAS_DEFINE_LITERAL(Manufacturer, "Us");
TAS_DEFINE_LITERAL(ManufacturerVersion, "0.0.1");
TAS_DEFINE_LITERAL(DeviceLocation, "Right Here");
TAS_DEFINE_LITERAL(GithubRepoLink, GITHUB_LINK);
TAS_DEFINE_LITERAL(DeviceDriverVersion, DEVICE_DRIVER_VERSION);
TAS_DEFINE_LITERAL(DeviceName, DEVICE_NAME);
TAS_DEFINE_LITERAL(DeviceDescription, DEVICE_DESCRIPTION);
TAS_DEFINE_LITERAL(DeviceUniqueId, "0");
TAS_DEFINE_LITERAL(MakeItRain, SUPPORTED_ACTION);

class ObservingConditionsAdapterTest : public testing::Test {
 protected:
  ObservingConditionsAdapterTest()
      : server_description_({.server_name = ServerName(),
                             .manufacturer = Manufacturer(),
                             .manufacturer_version = ManufacturerVersion(),
                             .location = DeviceLocation()}),
        device_info_({
            .device_type = EDeviceType::kObservingConditions,
            .device_number = kDeviceNumber,
            .name = DeviceName(),
            .unique_id = DeviceUniqueId(),
            .description = DeviceDescription(),
            .driver_info = GithubRepoLink(),
            .driver_version = DeviceDriverVersion(),
            .supported_actions = LiteralArray(supported_actions_),
            .interface_version = 1,
        }),
        device_interface_(device_info_),
        server_(server_description_, device_interfaces_) {}
  void SetUp() override {}

  HttpRequest GenerateDeviceApiRequest(std::string_view ascom_method) {
    HttpRequest req(absl::StrCat("/api/v1/observingconditions/", kDeviceNumber,
                                 "/", ascom_method));
    req.SetParameter("ClientID", std::to_string(client_id_));
    req.SetParameter("ClientTransactionID",
                     std::to_string(++client_transaction_id_));
    return req;
  }

  HttpRequest GenerateDeviceSetupRequest() {
    HttpRequest req(absl::StrCat("/setup/v1/observingconditions/",
                                 kDeviceNumber, "/setup"));
    req.SetParameter("ClientID", std::to_string(client_id_));
    req.SetParameter("ClientTransactionID",
                     std::to_string(++client_transaction_id_));
    return req;
  }

  ServerDescription server_description_;
  Literal supported_actions_[1] = {MakeItRain()};
  DeviceInfo device_info_;
  ObservingConditionsAdapter device_interface_;
  DeviceInterface* device_interfaces_[1] = {&device_interface_};
  TestTinyAlpacaServer server_;
  int client_id_ = 3;
  int client_transaction_id_ = 131;
};

TEST_F(ObservingConditionsAdapterTest, InitializeAndMaintain) {
  EXPECT_TRUE(server_.Initialize());
  server_.MaintainDevices();
}

TEST_F(ObservingConditionsAdapterTest, ConfiguredDevices) {
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
  ASSERT_TRUE(json_body.HasKey("Value"));
  ASSERT_TRUE(json_body.GetValue("Value").HasIndex(0));
  auto json_device_info = json_body.GetValue("Value").GetElement(0);
  ASSERT_EQ(json_device_info.type(), JsonValue::kObject);
  EXPECT_THAT(
      json_device_info.as_object(),
      UnorderedElementsAre(Pair("DeviceName", JsonValue(DEVICE_NAME)),
                           Pair("DeviceType", JsonValue("ObservingConditions")),
                           Pair("DeviceNumber", JsonValue(kDeviceNumber)),
                           Pair("UniqueID", JsonValue("0"))));
}

TEST_F(ObservingConditionsAdapterTest, SetupDevice) {
  EXPECT_TRUE(server_.Initialize());

  auto req = GenerateDeviceSetupRequest();
  req.SetHeader("Connection", "close");

  auto result = server_.AnnounceConnect(req.ToString());
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_FALSE(result.output.empty());
  EXPECT_TRUE(result.connection_closed);

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(result.output));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_TRUE(response.HasHeader("content-length"));
  EXPECT_TRUE(response.HasHeaderValue("CONTENT-TYPE", "text/html"));
  EXPECT_THAT(response.body_and_beyond, StartsWith("<html>"));
  EXPECT_THAT(response.body_and_beyond, HasSubstr("ObservingConditions"));
  EXPECT_THAT(response.body_and_beyond, HasSubstr(DEVICE_NAME));
}

TEST_F(ObservingConditionsAdapterTest, Connected) {
  EXPECT_TRUE(server_.Initialize());

  auto req = GenerateDeviceApiRequest("connected");

  auto result = server_.AnnounceConnect(req.ToString());
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_FALSE(result.output.empty());
  EXPECT_FALSE(result.connection_closed);

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(result.output));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_TRUE(response.HasHeader("content-length"));
  EXPECT_TRUE(response.HasHeaderValue("content-type", "application/json"));
  EXPECT_THAT(response.body_and_beyond, IsEmpty());
  EXPECT_EQ(response.json_value.type(), JsonValue::kObject);
  EXPECT_EQ(response.json_value.GetValue("Value"), JsonValue(true));
}

TEST_F(ObservingConditionsAdapterTest, Description) {
  EXPECT_TRUE(server_.Initialize());

  auto req = GenerateDeviceApiRequest("description");

  auto result = server_.AnnounceConnect(req.ToString());
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_FALSE(result.output.empty());
  EXPECT_FALSE(result.connection_closed);

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(result.output));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_TRUE(response.HasHeader("content-length"));
  EXPECT_TRUE(response.HasHeaderValue("content-type", "application/json"));
  EXPECT_THAT(response.body_and_beyond, IsEmpty());
  EXPECT_EQ(response.json_value.type(), JsonValue::kObject);
  EXPECT_EQ(response.json_value.GetValue("Value"),
            JsonValue(DEVICE_DESCRIPTION));
}

TEST_F(ObservingConditionsAdapterTest, DriverInfo) {
  EXPECT_TRUE(server_.Initialize());

  auto req = GenerateDeviceApiRequest("driverinfo");

  auto result = server_.AnnounceConnect(req.ToString());
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_FALSE(result.output.empty());
  EXPECT_FALSE(result.connection_closed);

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(result.output));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_TRUE(response.HasHeader("content-length"));
  EXPECT_TRUE(response.HasHeaderValue("content-type", "application/json"));
  EXPECT_THAT(response.body_and_beyond, IsEmpty());
  EXPECT_EQ(response.json_value.type(), JsonValue::kObject);
  EXPECT_EQ(response.json_value.GetValue("Value"), JsonValue(GITHUB_LINK));
}

TEST_F(ObservingConditionsAdapterTest, DriverVersion) {
  EXPECT_TRUE(server_.Initialize());

  auto req = GenerateDeviceApiRequest("driverversion");

  auto result = server_.AnnounceConnect(req.ToString());
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_FALSE(result.output.empty());
  EXPECT_FALSE(result.connection_closed);

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(result.output));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_TRUE(response.HasHeader("content-length"));
  EXPECT_TRUE(response.HasHeaderValue("content-type", "application/json"));
  EXPECT_THAT(response.body_and_beyond, IsEmpty());
  EXPECT_EQ(response.json_value.type(), JsonValue::kObject);
  EXPECT_EQ(response.json_value.GetValue("Value"),
            JsonValue(DEVICE_DRIVER_VERSION));
}

TEST_F(ObservingConditionsAdapterTest, InterfaceVersion) {
  EXPECT_TRUE(server_.Initialize());

  auto req = GenerateDeviceApiRequest("interfaceversion");

  auto result = server_.AnnounceConnect(req.ToString());
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_FALSE(result.output.empty());
  EXPECT_FALSE(result.connection_closed);

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(result.output));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_TRUE(response.HasHeader("content-length"));
  EXPECT_TRUE(response.HasHeaderValue("content-type", "application/json"));
  EXPECT_THAT(response.body_and_beyond, IsEmpty());
  EXPECT_EQ(response.json_value.type(), JsonValue::kObject);
  EXPECT_EQ(response.json_value.GetValue("Value"), JsonValue(1));
}

TEST_F(ObservingConditionsAdapterTest, Name) {
  EXPECT_TRUE(server_.Initialize());

  auto req = GenerateDeviceApiRequest("name");

  auto result = server_.AnnounceConnect(req.ToString());
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_FALSE(result.output.empty());
  EXPECT_FALSE(result.connection_closed);

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(result.output));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_TRUE(response.HasHeader("content-length"));
  EXPECT_TRUE(response.HasHeaderValue("content-type", "application/json"));
  EXPECT_THAT(response.body_and_beyond, IsEmpty());
  EXPECT_EQ(response.json_value.type(), JsonValue::kObject);
  EXPECT_EQ(response.json_value.GetValue("Value"), JsonValue(DEVICE_NAME));
}

TEST_F(ObservingConditionsAdapterTest, SupportedActions) {
  EXPECT_TRUE(server_.Initialize());

  auto req = GenerateDeviceApiRequest("supportedactions");
  req.SetHeader("Connection", "keepalive");

  auto result = server_.AnnounceConnect(req.ToString());
  EXPECT_THAT(result.remaining_input, IsEmpty());
  EXPECT_FALSE(result.output.empty());
  EXPECT_FALSE(result.connection_closed);

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(result.output));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_TRUE(response.HasHeader("content-length"));
  EXPECT_TRUE(response.HasHeaderValue("content-type", "application/json"));
  EXPECT_THAT(response.body_and_beyond, IsEmpty());
  EXPECT_EQ(response.json_value.type(), JsonValue::kObject);
  EXPECT_EQ(response.json_value.GetValue("Value"),
            JsonArray().Add(SUPPORTED_ACTION));
}

}  // namespace
}  // namespace test
}  // namespace alpaca
