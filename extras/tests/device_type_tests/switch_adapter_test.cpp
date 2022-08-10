#include "device_types/switch/switch_adapter.h"

#include <McuCore.h>
#include <stdint.h>

#include <string>

#include "alpaca_request.h"
#include "constants.h"
#include "device_description.h"
#include "extras/test_tools/decode_and_dispatch_test_base.h"
#include "extras/test_tools/mock_switch_group.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mcucore/extras/test_tools/print_to_std_string.h"

MCU_DEFINE_DOMAIN(75);

namespace alpaca {
namespace test {
namespace {

using ::mcucore::test::JsonValue;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;

constexpr int kDeviceNumber = 0;
constexpr int kClientId = 91240;
constexpr int kClientTransactionId = 42050;
constexpr int kServerTransactionId = 54981;

constexpr int kAscomInvalidValueError = 1025;
constexpr int kAscomValueNotSetError = 1026;

#define DEVICE_NAME "CircuitsController"
#define GITHUB_LINK "https://github/jamessynge/TinyAlpacaServer"
#define DEVICE_DESCRIPTION "Circuits Controller"
#define SUPPORTED_ACTION "JiggleSwitch"
#define DEVICE_DRIVER_VERSION "0.9"

class SwitchAdapterTest : public DecodeAndDispatchTestBase {
 protected:
  SwitchAdapterTest()
      : device_description_({
            .device_type = alpaca::EDeviceType::kSwitch,
            .device_number = kDeviceNumber,
            .domain = MCU_DOMAIN(75),
            .name = MCU_FLASHSTR("Switch Name"),
            .description = MCU_FLASHSTR("Switch Description"),
            .driver_info = MCU_FLASHSTR("Switch Driver Info"),
            .driver_version = MCU_FLASHSTR("Switch Driver Version"),
            .supported_actions = {},
        }),
        device_(device_description_) {
    // Setting this very early because GetMaxSwitch is called a lot. But the
    // return value isn't cached, so it is OK to change the expectation in
    // individual tests.
    ON_CALL(device_, GetMaxSwitch).WillByDefault(Return(1));
    AddDeviceInterface(device_);
  }

  void SetUp() override {
    DecodeAndDispatchTestBase::SetUp();

    InitializeRequest();

    Mock::VerifyAndClearExpectations(&device_);
  }

  void InitializeRequest() {
    request_.Reset();
    request_.set_server_transaction_id(kServerTransactionId);
    request_.http_method = EHttpMethod::GET;
    request_.api_group = EApiGroup::kDevice;
    request_.api = EAlpacaApi::kDeviceApi;
    request_.device_type = EDeviceType::kSwitch;
    request_.device_number = kDeviceNumber;
    request_.set_client_id(kClientId);
    request_.set_client_transaction_id(kClientTransactionId);
  }

  const mcucore::ProgmemString supported_actions_[1] = {
      MCU_PSD(SUPPORTED_ACTION)};
  const DeviceDescription device_description_;
  NiceMock<MockSwitchGroup> device_;
  AlpacaRequest request_;
};

// NOT testing the common methods here. observing_conditions_adapter_test.cc
// does so, and that testing should really be moved to a parameterized test,
// such as common_device_methods_test.cc.

TEST_F(SwitchAdapterTest, MaxSwitch) {
  EXPECT_CALL(device_, GetMaxSwitch).WillRepeatedly(Return(9));
  request_.device_method = EDeviceMethod::kMaxSwitch;
  mcucore::test::PrintToStdString out;
  ASSERT_TRUE(device_.HandleGetRequest(request_, out));
  response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       response_validator_.ValidateValueResponse(out.str()));
  EXPECT_EQ(value_jv, 9);
}

TEST_F(SwitchAdapterTest, DetectsMissingSwitchId) {
  EXPECT_CALL(device_, GetMaxSwitch).WillRepeatedly(Return(9));

  for (const auto device_method : {
           EDeviceMethod::kCanWrite,
           EDeviceMethod::kGetSwitch,
           EDeviceMethod::kGetSwitch,
           EDeviceMethod::kGetSwitchDescription,
           EDeviceMethod::kGetSwitchName,
           EDeviceMethod::kGetSwitchValue,
           EDeviceMethod::kMinSwitchValue,
           EDeviceMethod::kMaxSwitchValue,
           EDeviceMethod::kSwitchStep,
       }) {
    InitializeRequest();
    request_.device_method = device_method;

    mcucore::test::PrintToStdString out;
    device_.HandleGetRequest(request_, out);
    response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
    ASSERT_OK_AND_ASSIGN(auto response,
                         response_validator_.ValidateJsonResponseHasError(
                             out.str(), kAscomValueNotSetError));
    ASSERT_OK_AND_ASSIGN(
        auto error_message_jv,
        response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
    EXPECT_EQ(error_message_jv, "Missing parameter: Id");
  }

  for (const auto device_method : {
           EDeviceMethod::kSetSwitch,
           EDeviceMethod::kSetSwitchName,
           EDeviceMethod::kSetSwitchValue,
       }) {
    InitializeRequest();
    request_.device_method = device_method;

    mcucore::test::PrintToStdString out;
    device_.HandlePutRequest(request_, out);
    response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
    ASSERT_OK_AND_ASSIGN(auto response,
                         response_validator_.ValidateJsonResponseHasError(
                             out.str(), kAscomValueNotSetError));
    ASSERT_OK_AND_ASSIGN(
        auto error_message_jv,
        response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
    EXPECT_EQ(error_message_jv, "Missing parameter: Id");
  }
}

TEST_F(SwitchAdapterTest, DetectsSwitchIdTooHigh) {
  EXPECT_CALL(device_, GetMaxSwitch).WillRepeatedly(Return(9));

  for (const auto device_method : {
           EDeviceMethod::kCanWrite,
           EDeviceMethod::kGetSwitch,
           EDeviceMethod::kGetSwitch,
           EDeviceMethod::kGetSwitchDescription,
           EDeviceMethod::kGetSwitchName,
           EDeviceMethod::kGetSwitchValue,
           EDeviceMethod::kMinSwitchValue,
           EDeviceMethod::kMaxSwitchValue,
           EDeviceMethod::kSwitchStep,
       }) {
    InitializeRequest();
    request_.device_method = device_method;
    request_.set_id(9);
    mcucore::test::PrintToStdString out;
    // We choose to close connections if an error is generated, partly as a
    // defense against ill-behaved clients causing denial of service for others;
    // given how slow the Arduino Mega is, this has at least some merit.
    EXPECT_FALSE(device_.HandleGetRequest(request_, out));
    response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
    ASSERT_OK_AND_ASSIGN(auto response,
                         response_validator_.ValidateJsonResponseHasError(
                             out.str(), kAscomInvalidValueError));
    ASSERT_OK_AND_ASSIGN(
        auto error_message_jv,
        response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
    EXPECT_EQ(error_message_jv, "Invalid parameter: Id");
  }

  for (const auto device_method : {
           EDeviceMethod::kSetSwitch,
           EDeviceMethod::kSetSwitchName,
           EDeviceMethod::kSetSwitchValue,
       }) {
    InitializeRequest();
    request_.device_method = device_method;
    request_.set_id(9);
    mcucore::test::PrintToStdString out;
    EXPECT_FALSE(device_.HandlePutRequest(request_, out));
    response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
    ASSERT_OK_AND_ASSIGN(auto response,
                         response_validator_.ValidateJsonResponseHasError(
                             out.str(), kAscomInvalidValueError));
    ASSERT_OK_AND_ASSIGN(
        auto error_message_jv,
        response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
    EXPECT_EQ(error_message_jv, "Invalid parameter: Id");
  }
}

TEST_F(SwitchAdapterTest, CanWrite) {
  EXPECT_CALL(device_, GetMaxSwitch).WillRepeatedly(Return(9));

  request_.device_method = EDeviceMethod::kCanWrite;
  request_.set_id(2);
  EXPECT_CALL(device_, GetCanWrite(2)).WillOnce(Return(false));

  {
    mcucore::test::PrintToStdString out;
    EXPECT_TRUE(device_.HandleGetRequest(request_, out));
    response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
    ASSERT_OK_AND_ASSIGN(auto value_jv,
                         response_validator_.ValidateValueResponse(out.str()));
    EXPECT_EQ(value_jv, false);
  }

  InitializeRequest();
  request_.device_method = EDeviceMethod::kCanWrite;
  request_.set_id(3);
  EXPECT_CALL(device_, GetCanWrite(3)).WillOnce(Return(true));

  {
    mcucore::test::PrintToStdString out;
    EXPECT_TRUE(device_.HandleGetRequest(request_, out));
    response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
    ASSERT_OK_AND_ASSIGN(auto value_jv,
                         response_validator_.ValidateValueResponse(out.str()));
    EXPECT_EQ(value_jv, true);
  }
}

TEST_F(SwitchAdapterTest, GetSwitch) {
  EXPECT_CALL(device_, GetMaxSwitch).WillRepeatedly(Return(9));

  request_.device_method = EDeviceMethod::kGetSwitch;
  request_.set_id(0);
  EXPECT_CALL(device_, GetSwitch(0)).WillOnce(Return(false));

  {
    mcucore::test::PrintToStdString out;
    device_.HandleGetRequest(request_, out);
    response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
    ASSERT_OK_AND_ASSIGN(auto value_jv,
                         response_validator_.ValidateValueResponse(out.str()));
    EXPECT_EQ(value_jv, false);
  }

  InitializeRequest();
  request_.device_method = EDeviceMethod::kGetSwitch;
  request_.set_id(8);
  EXPECT_CALL(device_, GetSwitch(8)).WillOnce(Return(true));

  {
    mcucore::test::PrintToStdString out;
    device_.HandleGetRequest(request_, out);
    response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
    ASSERT_OK_AND_ASSIGN(auto value_jv,
                         response_validator_.ValidateValueResponse(out.str()));
    EXPECT_EQ(value_jv, true);
  }

  const char kErrorMessage[] = "Can not get switch as boolean";
  mcucore::ProgmemStringView literal_error_message(kErrorMessage);
  mcucore::Status status(static_cast<mcucore::StatusCode>(999),
                         literal_error_message);

  InitializeRequest();
  request_.device_method = EDeviceMethod::kGetSwitch;
  request_.set_id(1);
  EXPECT_CALL(device_, GetSwitch(1)).WillOnce(Return(status));

  {
    mcucore::test::PrintToStdString out;
    device_.HandleGetRequest(request_, out);
    response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
    ASSERT_OK_AND_ASSIGN(
        auto response,
        response_validator_.ValidateJsonResponseHasError(out.str(), 999));
    ASSERT_OK_AND_ASSIGN(
        auto error_message_jv,
        response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
    EXPECT_EQ(error_message_jv, std::string(kErrorMessage));
  }
}

TEST_F(SwitchAdapterTest, GetSwitchValue) {
  request_.device_method = EDeviceMethod::kGetSwitchValue;
  request_.set_id(0);
  EXPECT_CALL(device_, GetSwitchValue(0)).WillOnce(Return(123.4));

  {
    mcucore::test::PrintToStdString out;
    device_.HandleGetRequest(request_, out);
    response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
    ASSERT_OK_AND_ASSIGN(auto value_jv,
                         response_validator_.ValidateValueResponse(out.str()));
    EXPECT_EQ(value_jv, 123.4);
  }

  const char kErrorMessage[] = "Can not get switch as double";
  mcucore::ProgmemStringView literal_error_message(kErrorMessage);
  mcucore::Status status(mcucore::StatusCode::kNotFound, literal_error_message);

  EXPECT_CALL(device_, GetSwitchValue(0)).WillOnce(Return(status));

  {
    mcucore::test::PrintToStdString out;
    device_.HandleGetRequest(request_, out);
    response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
    ASSERT_OK_AND_ASSIGN(
        auto response,
        response_validator_.ValidateJsonResponseHasError(
            out.str(), static_cast<int>(mcucore::StatusCode::kNotFound)));
    ASSERT_OK_AND_ASSIGN(
        auto error_message_jv,
        response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
    EXPECT_EQ(error_message_jv, std::string(kErrorMessage));
  }
}

TEST_F(SwitchAdapterTest, GetMinSwitchValue) {
  request_.device_method = EDeviceMethod::kMinSwitchValue;
  request_.set_id(0);
  EXPECT_CALL(device_, GetMinSwitchValue(0)).WillOnce(Return(1.23));

  mcucore::test::PrintToStdString out;
  device_.HandleGetRequest(request_, out);
  response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       response_validator_.ValidateValueResponse(out.str()));
  EXPECT_EQ(value_jv, 1.23);
}

TEST_F(SwitchAdapterTest, GetMaxSwitchValue) {
  request_.device_method = EDeviceMethod::kMaxSwitchValue;
  request_.set_id(0);
  EXPECT_CALL(device_, GetMaxSwitchValue(0)).WillOnce(Return(1000.001));

  mcucore::test::PrintToStdString out;
  device_.HandleGetRequest(request_, out);
  response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       response_validator_.ValidateValueResponse(out.str()));
  EXPECT_EQ(value_jv, 1000.0);
}

TEST_F(SwitchAdapterTest, GetSwitchStep) {
  request_.device_method = EDeviceMethod::kSwitchStep;
  request_.set_id(0);
  EXPECT_CALL(device_, GetSwitchStep(0)).WillOnce(Return(1));

  mcucore::test::PrintToStdString out;
  device_.HandleGetRequest(request_, out);
  response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
  ASSERT_OK_AND_ASSIGN(auto value_jv,
                       response_validator_.ValidateValueResponse(out.str()));
  EXPECT_EQ(value_jv, 1.0);
}

TEST_F(SwitchAdapterTest, SetSwitch_MissingState) {
  request_.device_method = EDeviceMethod::kSetSwitch;
  request_.set_id(0);
  request_.set_value(1.23);  // Should be ignored.
  EXPECT_CALL(device_, SetSwitch).Times(0);

  mcucore::test::PrintToStdString out;
  EXPECT_FALSE(device_.HandlePutRequest(request_, out));

  response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           out.str(), kAscomValueNotSetError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_EQ(error_message_jv, "Missing parameter: State");
}

TEST_F(SwitchAdapterTest, SetSwitch) {
  request_.device_method = EDeviceMethod::kSetSwitch;
  request_.set_id(0);
  request_.set_state(false);
  EXPECT_CALL(device_, SetSwitch(0, false))
      .WillOnce(Return(mcucore::OkStatus()));

  mcucore::test::PrintToStdString out;
  EXPECT_TRUE(device_.HandlePutRequest(request_, out));
  response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
  ASSERT_OK(response_validator_.ValidateValuelessResponse(out.str()));
}

TEST_F(SwitchAdapterTest, SetSwitchValue_MissingValue) {
  request_.device_method = EDeviceMethod::kSetSwitchValue;
  request_.set_id(0);
  request_.set_state(false);  // Should be ignored.
  EXPECT_CALL(device_, SetSwitchValue).Times(0);

  mcucore::test::PrintToStdString out;
  EXPECT_FALSE(device_.HandlePutRequest(request_, out));

  response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
  ASSERT_OK_AND_ASSIGN(auto response,
                       response_validator_.ValidateJsonResponseHasError(
                           out.str(), kAscomValueNotSetError));
  ASSERT_OK_AND_ASSIGN(
      auto error_message_jv,
      response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
  EXPECT_EQ(error_message_jv, "Missing parameter: Value");
}

TEST_F(SwitchAdapterTest, SetSwitchValue_InvalidValue) {
  EXPECT_CALL(device_, GetMinSwitchValue).WillRepeatedly(Return(0.0));
  EXPECT_CALL(device_, GetMaxSwitchValue).WillRepeatedly(Return(1.0));

  for (const double value : {-0.000001, 1.000001}) {
    InitializeRequest();
    request_.device_method = EDeviceMethod::kSetSwitchValue;
    request_.set_id(0);
    request_.set_value(value);
    mcucore::test::PrintToStdString out;
    EXPECT_FALSE(device_.HandlePutRequest(request_, out));
    response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
    ASSERT_OK_AND_ASSIGN(auto response,
                         response_validator_.ValidateJsonResponseHasError(
                             out.str(), kAscomInvalidValueError));
    ASSERT_OK_AND_ASSIGN(
        auto error_message_jv,
        response.json_value.GetValueOfType("ErrorMessage", JsonValue::kString));
    EXPECT_EQ(error_message_jv, "Invalid parameter: Value");
  }
}

TEST_F(SwitchAdapterTest, SetSwitchValue) {
  EXPECT_CALL(device_, GetMaxSwitch).WillRepeatedly(Return(1));
  EXPECT_CALL(device_, GetMinSwitchValue).WillRepeatedly(Return(0));
  EXPECT_CALL(device_, GetMaxSwitchValue).WillRepeatedly(Return(2));

  request_.device_method = EDeviceMethod::kSetSwitchValue;
  request_.set_id(0);
  request_.set_value(1.23);
  EXPECT_CALL(device_, SetSwitchValue(0, 1.23))
      .WillOnce(Return(mcucore::OkStatus()));

  mcucore::test::PrintToStdString out;
  EXPECT_TRUE(device_.HandlePutRequest(request_, out));
  response_validator_.SetTransactionIdsFromAlpacaRequest(request_);
  ASSERT_OK(response_validator_.ValidateValuelessResponse(out.str()));
}

}  // namespace
}  // namespace test
}  // namespace alpaca
