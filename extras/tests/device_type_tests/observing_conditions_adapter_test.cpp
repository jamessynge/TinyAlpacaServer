#include "device_types/observing_conditions/observing_conditions_adapter.h"

#include <McuCore.h>
#include <McuNet.h>

#include <iostream>
#include <memory>
#include <string>

#include "constants.h"
#include "device_info.h"
#include "device_interface.h"
#include "extras/test_tools/decode_and_dispatch_test_base.h"
#include "extras/test_tools/mock_observing_conditions.h"
#include "extras/test_tools/test_tiny_alpaca_server.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "literals.h"
#include "mcucore/extras/test_tools/http_request.h"
#include "mcucore/extras/test_tools/http_response.h"
#include "mcucore/extras/test_tools/json_decoder.h"
#include "mcucore/extras/test_tools/uuid_utils.h"

MCU_DEFINE_NAMED_DOMAIN(FakeDevice, 123);

namespace alpaca {
namespace test {
namespace {

using ::mcucore::test::HttpRequest;
using ::mcucore::test::HttpResponse;
using ::mcucore::test::JsonArray;
using ::mcucore::test::JsonValue;
using ::mcucore::test::JsonValueIsUuid;
using ::testing::ContainsRegex;
using ::testing::HasSubstr;
using ::testing::Pair;
using ::testing::Return;
using ::testing::SizeIs;
using ::testing::StartsWith;
using ::testing::UnorderedElementsAre;
using ::testing::status::IsOkAndHolds;

constexpr int kAscomNotImplementedError = 1024;
constexpr int kAscomValueNotSetError = 1026;
constexpr int kDeviceNumber = 331;

#define DEVICE_NAME "WeatherOracle"
#define GITHUB_LINK "https://github/jamessynge/TinyAlpacaServer"
#define DEVICE_DESCRIPTION "Super Weather"
#define SUPPORTED_ACTION "MakeItRain"
#define DEVICE_DRIVER_VERSION "9.0"

class ObservingConditionsAdapterTest : public DecodeAndDispatchTestBase {
 protected:
  ObservingConditionsAdapterTest()
      : device_info_({
            .device_type = EDeviceType::kObservingConditions,
            .device_number = kDeviceNumber,
            .domain = MCU_DOMAIN(FakeDevice),
            .name = MCU_PSD(DEVICE_NAME),
            .description = MCU_PSD(DEVICE_DESCRIPTION),
            .driver_info = MCU_PSD(GITHUB_LINK),
            .driver_version = MCU_PSD(DEVICE_DRIVER_VERSION),
            .supported_actions =
                mcucore::ProgmemStringArray{supported_actions_},
            .interface_version = 1,
        }),
        device_(device_info_) {
    AddDeviceInterface(device_);
  }

  const mcucore::ProgmemString supported_actions_[1] = {
      MCU_PSD(SUPPORTED_ACTION)};
  const DeviceInfo device_info_;
  ObservingConditionsAdapter device_;
};

////////////////////////////////////////////////////////////////////////////////
//
// TODO(jamessynge): Move the common tests (i.e. those that are almost identical
// for every device type) into a Value-Parameterized Abstract Test (web search
// for googletest/docs/advanced.md).

TEST_F(ObservingConditionsAdapterTest, ConfiguredDevicesMinimalRequest) {
  HttpRequest request("/management/v1/configureddevices");

  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kArray);
  ASSERT_THAT(value_jv, SizeIs(1));
  auto json_device_info = value_jv.GetElement(0);
  ASSERT_EQ(json_device_info.type(), JsonValue::kObject);
  EXPECT_THAT(
      json_device_info.as_object(),
      UnorderedElementsAre(Pair("DeviceName", JsonValue(DEVICE_NAME)),
                           Pair("DeviceType", JsonValue("ObservingConditions")),
                           Pair("DeviceNumber", JsonValue(kDeviceNumber)),
                           Pair("UniqueID", JsonValueIsUuid())));
}

TEST_F(ObservingConditionsAdapterTest, SetupDevice) {
  // If we ask for the conneciton to be closed after the request is processed,
  // it should be at the end of the round-trip.
  auto request = GenerateDeviceSetupRequest();
  request.SetHeader("Connection", "close");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  EXPECT_FALSE(server_->connection_is_open());

  // The response should be an HTML page.
  ASSERT_OK_AND_ASSIGN(auto response, HttpResponse::Make(response_message));
  ASSERT_OK(response.IsOk());

  // Case of the header names in these queries shouldn't matter.
  EXPECT_TRUE(response.HasHeader("content-length"));
  EXPECT_TRUE(response.HasHeaderValue("CONTENT-TYPE", "text/html"));

  EXPECT_THAT(response.body_and_beyond, StartsWith("<html>"));
  EXPECT_THAT(response.body_and_beyond, HasSubstr("ObservingConditions"));
  EXPECT_THAT(response.body_and_beyond, HasSubstr(DEVICE_NAME));
  EXPECT_THAT(response.GetContentLength(),
              IsOkAndHolds(response.body_and_beyond.size()));
  EXPECT_THAT(response.body_and_beyond, ContainsRegex("</body></html>\\s*$"));
}

TEST_F(ObservingConditionsAdapterTest, Method_Get_Connected) {
  EXPECT_FALSE(server_->connection_is_open());
  auto request = GenerateDeviceApiRequest("connected");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripRequestWithValueResponse(request));
  EXPECT_TRUE(server_->connection_is_open());
  EXPECT_EQ(value_jv, true);
}

TEST_F(ObservingConditionsAdapterTest, Method_Description) {
  auto request = GenerateDeviceApiRequest("description");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  EXPECT_FALSE(server_->connection_is_open());
  EXPECT_EQ(value_jv, DEVICE_DESCRIPTION);
}

TEST_F(ObservingConditionsAdapterTest, Method_DriverInfo) {
  auto request = GenerateDeviceApiRequest("driverinfo");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripRequestWithValueResponse(request));
  EXPECT_TRUE(server_->connection_is_open());
  EXPECT_EQ(value_jv, GITHUB_LINK);
}

TEST_F(ObservingConditionsAdapterTest, Method_DriverVersion) {
  auto request = GenerateDeviceApiRequest("driverversion");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  EXPECT_FALSE(server_->connection_is_open());
  EXPECT_EQ(value_jv, DEVICE_DRIVER_VERSION);
}

TEST_F(ObservingConditionsAdapterTest, Method_InterfaceVersion) {
  auto request = GenerateDeviceApiRequest("interfaceversion");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripRequestWithValueResponse(request));
  EXPECT_TRUE(server_->connection_is_open());
  EXPECT_EQ(value_jv, 1);
}

TEST_F(ObservingConditionsAdapterTest, Method_Name) {
  auto request = GenerateDeviceApiRequest("name");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  EXPECT_FALSE(server_->connection_is_open());
  EXPECT_EQ(value_jv, DEVICE_NAME);
}

TEST_F(ObservingConditionsAdapterTest, Method_SupportedActions) {
  // If the client requests the connection to be kept alive, but also closes the
  // connection for writing, that should result in the connection being closed
  // by the server.
  auto request = GenerateDeviceApiRequest("supportedactions");
  request.SetHeader("Connection", "keepalive");
  request.SetHeader("Keep-Alive", "timeout=5");

  // Note that we said keepalive above, but shutdown the client side for writing
  // (i.e. half-close the connection).
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripRequestWithValueResponse(request, true));
  EXPECT_FALSE(server_->connection_is_open());
  EXPECT_EQ(value_jv, JsonArray().Add(SUPPORTED_ACTION));
}

////////////////////////////////////////////////////////////////////////////////
// The AveragePeriod methods are implemented to indicate that there is no
// averaging supported.

TEST_F(ObservingConditionsAdapterTest, Method_Get_AveragePeriod) {
  auto request = GenerateDeviceApiRequest("averageperiod");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripRequestWithValueResponse(request, false));
  EXPECT_TRUE(server_->connection_is_open());
  EXPECT_EQ(value_jv, 0);
}

TEST_F(ObservingConditionsAdapterTest, Method_Set_AveragePeriod) {
  // OK to set the average period to zero, but not anything else.
  EXPECT_FALSE(server_->connection_is_open());
  auto request = GenerateDeviceApiPutRequest("averageperiod");
  request.SetParameter("averagePERIOD", "0");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  EXPECT_TRUE(server_->connection_is_open());
  ASSERT_OK(response_validator_.ValidateValuelessResponse(response_message));

  request = GenerateDeviceApiPutRequest("averageperiod");
  request.SetParameter("averagePERIOD", "1");
  ASSERT_OK_AND_ASSIGN(response_message, RoundTripRequest(request, false));
  const int kInvalidParameter = 1025;
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kInvalidParameter));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("AveragePeriod"));

  // I chose to have the server close the connection after writing an error
  // response.
  EXPECT_FALSE(server_->connection_is_open());
}

////////////////////////////////////////////////////////////////////////////////
//
// The following methods are defined for observing conditions, but without a
// concrete-device (or mock), they return an unimplemented error.

TEST_F(ObservingConditionsAdapterTest, Method_CloudCover_Unimplemented) {
  auto request = GenerateDeviceApiRequest("cloudcover");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("CloudCover"));
}

TEST_F(ObservingConditionsAdapterTest, Method_DewPoint_Unimplemented) {
  auto request = GenerateDeviceApiRequest("dewpoint");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("DewPoint"));
}

TEST_F(ObservingConditionsAdapterTest, Method_Humidity_Unimplemented) {
  auto request = GenerateDeviceApiRequest("humidity");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("Humidity"));
}

TEST_F(ObservingConditionsAdapterTest, Method_Pressure_Unimplemented) {
  auto request = GenerateDeviceApiRequest("pressure");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("Pressure"));
}

TEST_F(ObservingConditionsAdapterTest, Method_RainRate_Unimplemented) {
  auto request = GenerateDeviceApiRequest("rainrate");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("RainRate"));
}

TEST_F(ObservingConditionsAdapterTest, Method_SkyBrightness_Unimplemented) {
  auto request = GenerateDeviceApiRequest("skybrightness");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SkyBrightness"));
}

TEST_F(ObservingConditionsAdapterTest, Method_SkyQuality_Unimplemented) {
  auto request = GenerateDeviceApiRequest("skyquality");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SkyQuality"));
}

TEST_F(ObservingConditionsAdapterTest, Method_SkyTemperature_Unimplemented) {
  auto request = GenerateDeviceApiRequest("skytemperature");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SkyTemperature"));
}

TEST_F(ObservingConditionsAdapterTest, Method_StarFWHM_Unimplemented) {
  auto request = GenerateDeviceApiRequest("starfwhm");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("StarFWHM"));
}

TEST_F(ObservingConditionsAdapterTest, Method_Temperature_Unimplemented) {
  auto request = GenerateDeviceApiRequest("temperature");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("Temperature"));
}

TEST_F(ObservingConditionsAdapterTest, Method_WindDirection_Unimplemented) {
  auto request = GenerateDeviceApiRequest("winddirection");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("WindDirection"));
}

TEST_F(ObservingConditionsAdapterTest, Method_WindGust_Unimplemented) {
  auto request = GenerateDeviceApiRequest("windgust");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("WindGust"));
}

TEST_F(ObservingConditionsAdapterTest, Method_WindSpeed_Unimplemented) {
  auto request = GenerateDeviceApiRequest("windspeed");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("WindSpeed"));
}

TEST_F(ObservingConditionsAdapterTest,
       Method_SensorDescription_MissingParameter) {
  auto request = GenerateDeviceApiRequest("sensordescription");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomValueNotSetError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SensorName"));
}

TEST_F(ObservingConditionsAdapterTest, Method_SensorDescription_Unimplemented) {
  auto request = GenerateDeviceApiRequest("sensordescription");
  request.SetParameter("SensorName", "Temperature");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SensorDescription"));
}

TEST_F(ObservingConditionsAdapterTest,
       Method_TimeSinceLastUpdate_MissingParameter) {
  auto request = GenerateDeviceApiRequest("timesincelastupdate");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomValueNotSetError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SensorName"));
}

TEST_F(ObservingConditionsAdapterTest,
       Method_TimeSinceLastUpdate_Unimplemented) {
  auto request = GenerateDeviceApiRequest("timesincelastupdate");
  request.SetParameter("SensorName", "Temperature");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomNotImplementedError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("Temperature"));
}

////////////////////////////////////////////////////////////////////////////////
// This isn't really a test of the MockObservingConditions, but rather that the
// base ObservingConditions class delegates to its subclass appropriately.

class MockObservingConditionsTest : public DecodeAndDispatchTestBase {
 protected:
  MockObservingConditionsTest()
      : device_info_({
            .device_type = EDeviceType::kObservingConditions,
            .device_number = kDeviceNumber,
            .domain = MCU_DOMAIN(FakeDevice),
            .name = MCU_FLASHSTR(DEVICE_NAME),
            .description = MCU_FLASHSTR(DEVICE_DESCRIPTION),
            .driver_info = MCU_FLASHSTR(GITHUB_LINK),
            .driver_version = MCU_FLASHSTR(DEVICE_DRIVER_VERSION),
            .supported_actions =
                mcucore::ProgmemStringArray{supported_actions_},
            .interface_version = 1,
        }),
        device_(device_info_) {
    AddDeviceInterface(device_);
  }

  const mcucore::ProgmemString supported_actions_[1] = {
      MCU_PSD(SUPPORTED_ACTION)};
  const DeviceInfo device_info_;
  MockObservingConditions device_;
};

TEST_F(MockObservingConditionsTest, Method_CloudCover) {
  EXPECT_CALL(device_, GetCloudCover).WillOnce(Return(1.1));
  auto request = GenerateDeviceApiRequest("cloudcover");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 1.1);
}

TEST_F(MockObservingConditionsTest, Method_DewPoint) {
  EXPECT_CALL(device_, GetDewPoint).WillOnce(Return(1.2));
  auto request = GenerateDeviceApiRequest("dewpoint");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 1.2);
}

TEST_F(MockObservingConditionsTest, Method_Humidity) {
  EXPECT_CALL(device_, GetHumidity).WillOnce(Return(1.3));
  auto request = GenerateDeviceApiRequest("humidity");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 1.3);
}

TEST_F(MockObservingConditionsTest, Method_Pressure) {
  EXPECT_CALL(device_, GetPressure).WillOnce(Return(1.4));
  auto request = GenerateDeviceApiRequest("pressure");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 1.4);
}

TEST_F(MockObservingConditionsTest, Method_RainRate) {
  EXPECT_CALL(device_, GetRainRate).WillOnce(Return(1.5));
  auto request = GenerateDeviceApiRequest("rainrate");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 1.5);
}

TEST_F(MockObservingConditionsTest, Method_SkyBrightness) {
  EXPECT_CALL(device_, GetSkyBrightness).WillOnce(Return(1.6));
  auto request = GenerateDeviceApiRequest("skybrightness");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 1.6);
}

TEST_F(MockObservingConditionsTest, Method_SkyQuality) {
  EXPECT_CALL(device_, GetSkyQuality).WillOnce(Return(1.7));
  auto request = GenerateDeviceApiRequest("skyquality");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 1.7);
}

TEST_F(MockObservingConditionsTest, Method_SkyTemperature) {
  EXPECT_CALL(device_, GetSkyTemperature).WillOnce(Return(1.8));
  auto request = GenerateDeviceApiRequest("skytemperature");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 1.8);
}

TEST_F(MockObservingConditionsTest, Method_StarFWHM) {
  EXPECT_CALL(device_, GetStarFWHM).WillOnce(Return(1.9));
  auto request = GenerateDeviceApiRequest("starfwhm");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 1.9);
}

TEST_F(MockObservingConditionsTest, Method_Temperature) {
  EXPECT_CALL(device_, GetTemperature).WillOnce(Return(2.1));
  auto request = GenerateDeviceApiRequest("temperature");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 2.1);
}

TEST_F(MockObservingConditionsTest, Method_WindDirection) {
  EXPECT_CALL(device_, GetWindDirection).WillOnce(Return(2.2));
  auto request = GenerateDeviceApiRequest("winddirection");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 2.2);
}

TEST_F(MockObservingConditionsTest, Method_WindGust) {
  EXPECT_CALL(device_, GetWindGust).WillOnce(Return(2.3));
  auto request = GenerateDeviceApiRequest("windgust");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 2.3);
}

TEST_F(MockObservingConditionsTest, Method_WindSpeed) {
  EXPECT_CALL(device_, GetWindSpeed).WillOnce(Return(2.4));
  auto request = GenerateDeviceApiRequest("windspeed");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv.type(), JsonValue::kDouble);
  EXPECT_EQ(value_jv.as_double(), 2.4);
}

TEST_F(MockObservingConditionsTest, Method_SensorDescription_MissingParameter) {
  auto request = GenerateDeviceApiRequest("sensordescription");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomValueNotSetError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SensorName"));
}

TEST_F(MockObservingConditionsTest, Method_SensorDescription) {
  EXPECT_CALL(device_, GetSensorDescription)
      .WillOnce(Return(mcucore::ProgmemStringView("MLX90614")));
  auto request = GenerateDeviceApiRequest("sensordescription");
  request.SetParameter("SensorName", "Temperature");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  ASSERT_EQ(value_jv, "MLX90614");
}

TEST_F(MockObservingConditionsTest,
       Method_TimeSinceLastUpdate_MissingParameter) {
  auto request = GenerateDeviceApiRequest("timesincelastupdate");
  ASSERT_OK_AND_ASSIGN(auto response_message, RoundTripRequest(request, false));
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           response_message, kAscomValueNotSetError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_THAT(error_message_jv.as_string(), HasSubstr("SensorName"));
}

TEST_F(MockObservingConditionsTest, Method_TimeSinceLastUpdate) {
  EXPECT_CALL(device_, GetTimeSinceLastUpdate).WillOnce(Return(10));
  auto request = GenerateDeviceApiRequest("timesincelastupdate");
  request.SetParameter("SensorName", "WindSpeed");
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       RoundTripSoleRequestWithValueResponse(request));
  EXPECT_EQ(value_jv, 10);
}

}  // namespace
}  // namespace test
}  // namespace alpaca
