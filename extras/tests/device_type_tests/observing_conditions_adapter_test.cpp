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
#include "extras/test_tools/decode_and_dispatch_test_base.h"
#include "extras/test_tools/http_request.h"
#include "extras/test_tools/http_response.h"
#include "extras/test_tools/json_decoder.h"
#include "extras/test_tools/mock_device_interface.h"
#include "extras/test_tools/mock_observing_conditions.h"
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

using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::Return;
using ::testing::SizeIs;
using ::testing::StartsWith;
using ::testing::UnorderedElementsAre;
using ::testing::status::IsOkAndHolds;

constexpr int kDeviceNumber = 331;

#define DEVICE_NAME "WeatherOracle"
#define GITHUB_LINK "https://github/jamessynge/TinyAlpacaServer"
#define DEVICE_DESCRIPTION "Super Weather"
#define SUPPORTED_ACTION "MakeItRain"
#define DEVICE_DRIVER_VERSION "99"

TAS_DEFINE_LITERAL(GithubRepoLink, GITHUB_LINK);
TAS_DEFINE_LITERAL(DeviceDriverVersion, DEVICE_DRIVER_VERSION);
TAS_DEFINE_LITERAL(DeviceName, DEVICE_NAME);
TAS_DEFINE_LITERAL(DeviceDescription, DEVICE_DESCRIPTION);
TAS_DEFINE_LITERAL(DeviceUniqueId, "0");
TAS_DEFINE_LITERAL(MakeItRain, SUPPORTED_ACTION);

class ObservingConditionsAdapterTest : public DecodeAndDispatchTestBase {
 protected:
  ObservingConditionsAdapterTest()
      : device_info_({
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
        device_(device_info_) {
    AddDeviceInterface(device_);
  }

  Literal supported_actions_[1] = {MakeItRain()};
  DeviceInfo device_info_;
  ObservingConditionsAdapter device_;
  DeviceInterface* device_interfaces_[1] = {&device_};
};

////////////////////////////////////////////////////////////////////////////////
//
// TODO(jamessynge): Move the common tests (i.e. those that are almost identical
// for every device type) into a Value-Parameterized Abstract Test (web search
// for googletest/docs/advanced.md).

TEST_F(ObservingConditionsAdapterTest, ConfiguredDevicesMinimalRequest) {
  HttpRequest request("/management/v1/configureddevices");

  ASSERT_OK_AND_ASSIGN(auto resp, RoundTripSoleRequest(request));
  ASSERT_OK_AND_ASSIGN(auto devices_jv,
                       ValidateArrayValueResponse(request, resp));
  ASSERT_THAT(devices_jv, SizeIs(1));
  auto json_device_info = devices_jv.GetElement(0);
  ASSERT_EQ(json_device_info.type(), JsonValue::kObject);
  EXPECT_THAT(
      json_device_info.as_object(),
      UnorderedElementsAre(Pair("DeviceName", JsonValue(DEVICE_NAME)),
                           Pair("DeviceType", JsonValue("ObservingConditions")),
                           Pair("DeviceNumber", JsonValue(kDeviceNumber)),
                           Pair("UniqueID", JsonValue("0"))));
}

TEST_F(ObservingConditionsAdapterTest, SetupDevice) {
  auto req = GenerateDeviceSetupRequest();
  req.SetHeader("Connection", "close");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  EXPECT_FALSE(server_->connection_is_open());
  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(response_str));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 200);
  EXPECT_EQ(response.status_message, "OK");
  EXPECT_TRUE(response.HasHeader("content-length"));
  EXPECT_TRUE(response.HasHeaderValue("CONTENT-TYPE", "text/html"));
  EXPECT_THAT(response.body_and_beyond, StartsWith("<html>"));
  EXPECT_THAT(response.body_and_beyond, HasSubstr("ObservingConditions"));
  EXPECT_THAT(response.body_and_beyond, HasSubstr(DEVICE_NAME));
  EXPECT_THAT(response.GetContentLength(),
              IsOkAndHolds(response.body_and_beyond.size()));
}

TEST_F(ObservingConditionsAdapterTest, Method_Get_Connected) {
  EXPECT_FALSE(server_->connection_is_open());
  auto req = GenerateDeviceApiRequest("connected");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  EXPECT_TRUE(server_->connection_is_open());
  ASSERT_OK_AND_ASSIGN(auto value_jv, ValidateValueResponse(req, response_str));
  EXPECT_EQ(value_jv, true);
}

TEST_F(ObservingConditionsAdapterTest, Method_Description) {
  auto req = GenerateDeviceApiRequest("description");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  EXPECT_TRUE(server_->connection_is_open());
  ASSERT_OK_AND_ASSIGN(auto value_jv, ValidateValueResponse(req, response_str));
  EXPECT_EQ(value_jv, DEVICE_DESCRIPTION);
}

TEST_F(ObservingConditionsAdapterTest, Method_DriverInfo) {
  auto req = GenerateDeviceApiRequest("driverinfo");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  EXPECT_TRUE(server_->connection_is_open());
  ASSERT_OK_AND_ASSIGN(auto value_jv, ValidateValueResponse(req, response_str));
  EXPECT_EQ(value_jv, GITHUB_LINK);
}

TEST_F(ObservingConditionsAdapterTest, Method_DriverVersion) {
  auto req = GenerateDeviceApiRequest("driverversion");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  EXPECT_TRUE(server_->connection_is_open());
  ASSERT_OK_AND_ASSIGN(auto value_jv, ValidateValueResponse(req, response_str));
  EXPECT_EQ(value_jv, DEVICE_DRIVER_VERSION);
}

TEST_F(ObservingConditionsAdapterTest, Method_InterfaceVersion) {
  auto req = GenerateDeviceApiRequest("interfaceversion");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  EXPECT_TRUE(server_->connection_is_open());
  ASSERT_OK_AND_ASSIGN(auto value_jv, ValidateValueResponse(req, response_str));
  EXPECT_EQ(value_jv, 1);
}

TEST_F(ObservingConditionsAdapterTest, Method_Name) {
  auto req = GenerateDeviceApiRequest("name");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  EXPECT_TRUE(server_->connection_is_open());
  ASSERT_OK_AND_ASSIGN(auto value_jv, ValidateValueResponse(req, response_str));
  EXPECT_EQ(value_jv, DEVICE_NAME);
}

TEST_F(ObservingConditionsAdapterTest, Method_SupportedActions) {
  auto req = GenerateDeviceApiRequest("supportedactions");
  req.SetHeader("Connection", "keepalive");
  req.SetHeader("Keep-Alive", "timeout=5");

  // Note that we said keepalive above, but shutdown the client side for writing
  // (i.e. half-close the connection).
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, true));
  EXPECT_FALSE(server_->connection_is_open());

  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(response_str));
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

////////////////////////////////////////////////////////////////////////////////

TEST_F(ObservingConditionsAdapterTest, Method_Get_AveragePeriod) {
  EXPECT_FALSE(server_->connection_is_open());
  auto req = GenerateDeviceApiRequest("averageperiod");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  EXPECT_TRUE(server_->connection_is_open());
  ASSERT_OK_AND_ASSIGN(auto value_jv, ValidateValueResponse(req, response_str));
  EXPECT_EQ(value_jv, 0);
}

TEST_F(ObservingConditionsAdapterTest, Method_Set_AveragePeriod) {
  EXPECT_FALSE(server_->connection_is_open());
  auto req = GenerateDeviceApiPutRequest("averageperiod");
  req.SetParameter("averagePERIOD", "0");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  EXPECT_TRUE(server_->connection_is_open());
  ASSERT_OK(ValidateValuelessResponse(req, response_str));

  req = GenerateDeviceApiPutRequest("averageperiod");
  req.SetParameter("averagePERIOD", "1");
  ASSERT_OK_AND_ASSIGN(response_str, RoundTripRequest(req, false));
  const int kInvalidParameter = 1025;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kInvalidParameter));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("AveragePeriod"));

  // I chose to have the server close the connection after writing an error
  // response.
  EXPECT_FALSE(server_->connection_is_open());
}

////////////////////////////////////////////////////////////////////////////////
//
// The following methods are defined for observing conditions, but without a
// concrete-device (or mock), they're unimplemented.

TEST_F(ObservingConditionsAdapterTest, Method_CloudCover_Unimplemented) {
  auto req = GenerateDeviceApiRequest("cloudcover");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("CloudCover"));
}

TEST_F(ObservingConditionsAdapterTest, Method_DewPoint_Unimplemented) {
  auto req = GenerateDeviceApiRequest("dewpoint");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("DewPoint"));
}

TEST_F(ObservingConditionsAdapterTest, Method_Humidity_Unimplemented) {
  auto req = GenerateDeviceApiRequest("humidity");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("Humidity"));
}

TEST_F(ObservingConditionsAdapterTest, Method_Pressure_Unimplemented) {
  auto req = GenerateDeviceApiRequest("pressure");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("Pressure"));
}

TEST_F(ObservingConditionsAdapterTest, Method_RainRate_Unimplemented) {
  auto req = GenerateDeviceApiRequest("rainrate");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("RainRate"));
}

TEST_F(ObservingConditionsAdapterTest, Method_SkyBrightness_Unimplemented) {
  auto req = GenerateDeviceApiRequest("skybrightness");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SkyBrightness"));
}

TEST_F(ObservingConditionsAdapterTest, Method_SkyQuality_Unimplemented) {
  auto req = GenerateDeviceApiRequest("skyquality");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SkyQuality"));
}

TEST_F(ObservingConditionsAdapterTest, Method_SkyTemperature_Unimplemented) {
  auto req = GenerateDeviceApiRequest("skytemperature");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SkyTemperature"));
}

TEST_F(ObservingConditionsAdapterTest, Method_StarFWHM_Unimplemented) {
  auto req = GenerateDeviceApiRequest("starfwhm");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("StarFWHM"));
}

TEST_F(ObservingConditionsAdapterTest, Method_Temperature_Unimplemented) {
  auto req = GenerateDeviceApiRequest("temperature");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("Temperature"));
}

TEST_F(ObservingConditionsAdapterTest, Method_WindDirection_Unimplemented) {
  auto req = GenerateDeviceApiRequest("winddirection");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("WindDirection"));
}

TEST_F(ObservingConditionsAdapterTest, Method_WindGust_Unimplemented) {
  auto req = GenerateDeviceApiRequest("windgust");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("WindGust"));
}

TEST_F(ObservingConditionsAdapterTest, Method_WindSpeed_Unimplemented) {
  auto req = GenerateDeviceApiRequest("windspeed");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("WindSpeed"));
}

TEST_F(ObservingConditionsAdapterTest,
       Method_SensorDescription_MissingParameter) {
  auto req = GenerateDeviceApiRequest("sensordescription");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kValueNotSet = 1026;
  ASSERT_OK_AND_ASSIGN(auto response, ValidateJsonResponseHasError(
                                          req, response_str, kValueNotSet));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SensorName"));
}

TEST_F(ObservingConditionsAdapterTest, Method_SensorDescription_Unimplemented) {
  auto req = GenerateDeviceApiRequest("sensordescription");
  req.SetParameter("SensorName", "Temperature");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SensorDescription"));
}

TEST_F(ObservingConditionsAdapterTest,
       Method_TimeSinceLastUpdate_MissingParameter) {
  auto req = GenerateDeviceApiRequest("timesincelastupdate");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kValueNotSet = 1026;
  ASSERT_OK_AND_ASSIGN(auto response, ValidateJsonResponseHasError(
                                          req, response_str, kValueNotSet));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SensorName"));
}

TEST_F(ObservingConditionsAdapterTest,
       Method_TimeSinceLastUpdate_Unimplemented) {
  auto req = GenerateDeviceApiRequest("timesincelastupdate");
  req.SetParameter("SensorName", "Temperature");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kMethodNotImplemented = 1024;
  ASSERT_OK_AND_ASSIGN(
      auto response,
      ValidateJsonResponseHasError(req, response_str, kMethodNotImplemented));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("Temperature"));
}

////////////////////////////////////////////////////////////////////////////////

class MockObservingConditionsTest : public DecodeAndDispatchTestBase {
 protected:
  MockObservingConditionsTest()
      : device_info_({
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
        device_(device_info_) {
    AddDeviceInterface(device_);
  }

  Literal supported_actions_[1] = {MakeItRain()};
  DeviceInfo device_info_;
  MockObservingConditions device_;
  DeviceInterface* device_interfaces_[1] = {&device_};
};

TEST_F(MockObservingConditionsTest, Method_CloudCover) {
  EXPECT_CALL(device_, GetCloudCover).WillOnce(Return(1.1));
  auto req = GenerateDeviceApiRequest("cloudcover");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 1.1);
}

TEST_F(MockObservingConditionsTest, Method_DewPoint) {
  EXPECT_CALL(device_, GetDewPoint).WillOnce(Return(1.2));
  auto req = GenerateDeviceApiRequest("dewpoint");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 1.2);
}

TEST_F(MockObservingConditionsTest, Method_Humidity) {
  EXPECT_CALL(device_, GetHumidity).WillOnce(Return(1.3));
  auto req = GenerateDeviceApiRequest("humidity");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 1.3);
}

TEST_F(MockObservingConditionsTest, Method_Pressure) {
  EXPECT_CALL(device_, GetPressure).WillOnce(Return(1.4));
  auto req = GenerateDeviceApiRequest("pressure");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 1.4);
}

TEST_F(MockObservingConditionsTest, Method_RainRate) {
  EXPECT_CALL(device_, GetRainRate).WillOnce(Return(1.5));
  auto req = GenerateDeviceApiRequest("rainrate");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 1.5);
}

TEST_F(MockObservingConditionsTest, Method_SkyBrightness) {
  EXPECT_CALL(device_, GetSkyBrightness).WillOnce(Return(1.6));
  auto req = GenerateDeviceApiRequest("skybrightness");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 1.6);
}

TEST_F(MockObservingConditionsTest, Method_SkyQuality) {
  EXPECT_CALL(device_, GetSkyQuality).WillOnce(Return(1.7));
  auto req = GenerateDeviceApiRequest("skyquality");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 1.7);
}

TEST_F(MockObservingConditionsTest, Method_SkyTemperature) {
  EXPECT_CALL(device_, GetSkyTemperature).WillOnce(Return(1.8));
  auto req = GenerateDeviceApiRequest("skytemperature");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 1.8);
}

TEST_F(MockObservingConditionsTest, Method_StarFWHM) {
  EXPECT_CALL(device_, GetStarFWHM).WillOnce(Return(1.9));
  auto req = GenerateDeviceApiRequest("starfwhm");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 1.9);
}

TEST_F(MockObservingConditionsTest, Method_Temperature) {
  EXPECT_CALL(device_, GetTemperature).WillOnce(Return(2.1));
  auto req = GenerateDeviceApiRequest("temperature");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 2.1);
}

TEST_F(MockObservingConditionsTest, Method_WindDirection) {
  EXPECT_CALL(device_, GetWindDirection).WillOnce(Return(2.2));
  auto req = GenerateDeviceApiRequest("winddirection");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 2.2);
}

TEST_F(MockObservingConditionsTest, Method_WindGust) {
  EXPECT_CALL(device_, GetWindGust).WillOnce(Return(2.3));
  auto req = GenerateDeviceApiRequest("windgust");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 2.3);
}

TEST_F(MockObservingConditionsTest, Method_WindSpeed) {
  EXPECT_CALL(device_, GetWindSpeed).WillOnce(Return(2.4));
  auto req = GenerateDeviceApiRequest("windspeed");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value.type(), JsonValue::kDouble);
  EXPECT_EQ(value.as_double(), 2.4);
}

TEST_F(MockObservingConditionsTest, Method_SensorDescription_MissingParameter) {
  auto req = GenerateDeviceApiRequest("sensordescription");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kValueNotSet = 1026;
  ASSERT_OK_AND_ASSIGN(auto response, ValidateJsonResponseHasError(
                                          req, response_str, kValueNotSet));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SensorName"));
}

TEST_F(MockObservingConditionsTest, Method_SensorDescription) {
  EXPECT_CALL(device_, GetSensorDescription)
      .WillOnce(Return(Literal("MLX90614")));
  auto req = GenerateDeviceApiRequest("sensordescription");
  req.SetParameter("SensorName", "Temperature");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  ASSERT_EQ(value, "MLX90614");
}

TEST_F(MockObservingConditionsTest,
       Method_TimeSinceLastUpdate_MissingParameter) {
  auto req = GenerateDeviceApiRequest("timesincelastupdate");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  const int kValueNotSet = 1026;
  ASSERT_OK_AND_ASSIGN(auto response, ValidateJsonResponseHasError(
                                          req, response_str, kValueNotSet));
  auto error_message_jv = response.json_value.GetValue("ErrorMessage");
  ASSERT_EQ(error_message_jv.type(), JsonValue::kString);
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SensorName"));
}

TEST_F(MockObservingConditionsTest, Method_TimeSinceLastUpdate) {
  EXPECT_CALL(device_, GetTimeSinceLastUpdate).WillOnce(Return(10));
  auto req = GenerateDeviceApiRequest("timesincelastupdate");
  req.SetParameter("SensorName", "WindSpeed");
  ASSERT_OK_AND_ASSIGN(auto response_str, RoundTripRequest(req, false));
  ASSERT_OK_AND_ASSIGN(auto value, ValidateValueResponse(req, response_str));
  EXPECT_EQ(value, 10);
}

}  // namespace
}  // namespace test
}  // namespace alpaca
