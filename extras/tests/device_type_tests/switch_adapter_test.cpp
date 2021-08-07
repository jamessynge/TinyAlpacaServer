#include "device_types/switch/switch_adapter.h"

#include <stdint.h>

#include <string>

#include "absl/strings/str_cat.h"
#include "alpaca_request.h"
#include "constants.h"
#include "device_info.h"
#include "extras/test_tools/mock_switch_group.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "inline_literal.h"
#include "mcucore/extrastest_tools/print_to_std_string.h"
#include "utils/literal.h"
#include "utils/status.h"
#include "utils/status_or.h"

namespace alpaca {
namespace test {
namespace {

using ::testing::ContainsRegex;
using ::testing::HasSubstr;
using ::testing::Mock;
using ::testing::Not;
using ::testing::Return;
using ::testing::StartsWith;

constexpr int kDeviceNumber = 87405;
constexpr int kClientId = 91240;
constexpr int kClientTransactionId = 42050;
constexpr int kServerTransactionId = 54981;

class SwitchGroupTest : public testing::Test {
 protected:
  SwitchGroupTest() : switch_group_(device_info_) {}
  void SetUp() override {
    InitializeRequest();

    EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(0));
    switch_group_.Initialize();
    Mock::VerifyAndClearExpectations(&switch_group_);
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

  void VerifyResponseIsOK(const std::string& response) {
    EXPECT_THAT(response, StartsWith("HTTP/1.1 200 OK\r\n"));
    EXPECT_THAT(response, HasSubstr("\r\nContent-Type: application/json\r\n"));
    EXPECT_THAT(response, HasSubstr(absl::StrCat(R"("ClientTransactionID": )",
                                                 kClientTransactionId)));
    EXPECT_THAT(response, HasSubstr(absl::StrCat(R"("ServerTransactionID": )",
                                                 kServerTransactionId)));
  }

  void VerifyResponseIsGood(const std::string& response) {
    VerifyResponseIsOK(response);
    EXPECT_THAT(response, Not(HasSubstr(R"("ErrorMessage":)")));
    EXPECT_THAT(response, Not(HasSubstr(R"("ErrorNumber":)")));
  }

  void VerifyResponseHasError(const std::string& response) {
    VerifyResponseIsOK(response);
    EXPECT_THAT(response, HasSubstr(R"("ErrorMessage":)"));
    EXPECT_THAT(response, HasSubstr(R"("ErrorNumber":)"));
    EXPECT_THAT(response, Not(HasSubstr(R"("Value":)")));
  }

  const alpaca::DeviceInfo device_info_{
      .device_type = alpaca::EDeviceType::kSwitch,
      .device_number = kDeviceNumber,
      .name = TAS_FLASHSTR("Switch Name"),
      .unique_id = TAS_FLASHSTR("Switch Unique Id"),
      .description = TAS_FLASHSTR("Switch Description"),
      .driver_info = TAS_FLASHSTR("Switch Driver Info"),
      .driver_version = TAS_FLASHSTR("Switch Driver Version"),
      .supported_actions = {},
      .interface_version = 1,
  };

  MockSwitchGroup switch_group_;
  AlpacaRequest request_;
};

TEST_F(SwitchGroupTest, MaxSwitch) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(9));

  request_.device_method = EDeviceMethod::kMaxSwitch;
  mcucore::test::PrintToStdString out;
  EXPECT_TRUE(switch_group_.HandleGetRequest(request_, out));
  const std::string response = out.str();

  VerifyResponseIsGood(response);
  EXPECT_THAT(response, ContainsRegex(R"("Value":\s*9[^0-9.])"));
}

TEST_F(SwitchGroupTest, DetectsMissingSwitchId) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(9));

  const auto kExpectedErrorMessage =
      R"("ErrorMessage": "Missing parameter: Id")";

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
    request_.device_method = device_method;

    mcucore::test::PrintToStdString out;
    switch_group_.HandleGetRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseHasError(response);
    EXPECT_THAT(response, HasSubstr(kExpectedErrorMessage));
  }

  for (const auto device_method : {
           EDeviceMethod::kSetSwitch,
           EDeviceMethod::kSetSwitchName,
           EDeviceMethod::kSetSwitchValue,
       }) {
    request_.device_method = device_method;

    mcucore::test::PrintToStdString out;
    switch_group_.HandlePutRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseHasError(response);
    EXPECT_THAT(response, HasSubstr(kExpectedErrorMessage));
  }
}

TEST_F(SwitchGroupTest, DetectsSwitchIdTooHigh) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(9));

  request_.set_id(9);

  const auto kExpectedErrorMessage =
      R"("ErrorMessage": "Invalid parameter: Id")";

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
    request_.device_method = device_method;
    mcucore::test::PrintToStdString out;
    switch_group_.HandleGetRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseHasError(response);
    EXPECT_THAT(response, HasSubstr(kExpectedErrorMessage));
  }

  for (const auto device_method : {
           EDeviceMethod::kSetSwitch,
           EDeviceMethod::kSetSwitchName,
           EDeviceMethod::kSetSwitchValue,
       }) {
    request_.device_method = device_method;
    mcucore::test::PrintToStdString out;
    switch_group_.HandlePutRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseHasError(response);
    EXPECT_THAT(response, HasSubstr(kExpectedErrorMessage));
  }
}

TEST_F(SwitchGroupTest, CanWrite) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(9));

  request_.device_method = EDeviceMethod::kCanWrite;
  request_.set_id(2);
  EXPECT_CALL(switch_group_, GetCanWrite(2)).WillOnce(Return(false));

  {
    mcucore::test::PrintToStdString out;
    switch_group_.HandleGetRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseIsGood(response);
    EXPECT_THAT(response, HasSubstr(R"("Value": false)"));
  }

  InitializeRequest();
  request_.device_method = EDeviceMethod::kCanWrite;
  request_.set_id(3);
  EXPECT_CALL(switch_group_, GetCanWrite(3)).WillOnce(Return(true));

  {
    mcucore::test::PrintToStdString out;
    switch_group_.HandleGetRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseIsGood(response);
    EXPECT_THAT(response, HasSubstr(R"("Value": true)"));
  }
}

TEST_F(SwitchGroupTest, GetSwitch) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(9));

  request_.device_method = EDeviceMethod::kGetSwitch;
  request_.set_id(0);
  EXPECT_CALL(switch_group_, GetSwitch(0)).WillOnce(Return(false));

  {
    mcucore::test::PrintToStdString out;
    switch_group_.HandleGetRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseIsGood(response);
    EXPECT_THAT(response, HasSubstr(R"("Value": false)"));
  }

  InitializeRequest();
  request_.device_method = EDeviceMethod::kGetSwitch;
  request_.set_id(8);
  EXPECT_CALL(switch_group_, GetSwitch(8)).WillOnce(Return(true));

  {
    mcucore::test::PrintToStdString out;
    switch_group_.HandleGetRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseIsGood(response);
    EXPECT_THAT(response, HasSubstr(R"("Value": true)"));
  }

  const char kErrorMessage[] = "Can not get switch as boolean";
  Literal literal_error_message(kErrorMessage);
  Status status(123, literal_error_message);

  InitializeRequest();
  request_.device_method = EDeviceMethod::kGetSwitch;
  request_.set_id(1);
  EXPECT_CALL(switch_group_, GetSwitch(1)).WillOnce(Return(status));

  {
    mcucore::test::PrintToStdString out;
    switch_group_.HandleGetRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseHasError(response);
    EXPECT_THAT(response, HasSubstr(R"("ErrorNumber": 123)"));
    EXPECT_THAT(response, HasSubstr(absl::StrCat(R"("ErrorMessage": ")",
                                                 kErrorMessage, "\"")));
  }
}

TEST_F(SwitchGroupTest, GetSwitchValue) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(1));

  request_.device_method = EDeviceMethod::kGetSwitchValue;
  request_.set_id(0);
  EXPECT_CALL(switch_group_, GetSwitchValue(0)).WillOnce(Return(123.4));

  {
    mcucore::test::PrintToStdString out;
    switch_group_.HandleGetRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseIsGood(response);
    EXPECT_THAT(response, HasSubstr(R"("Value": 123.4)"));
  }

  const char kErrorMessage[] = "Can not get switch as double";
  Literal literal_error_message(kErrorMessage);
  Status status(123, literal_error_message);

  EXPECT_CALL(switch_group_, GetSwitchValue(0)).WillOnce(Return(status));

  {
    mcucore::test::PrintToStdString out;
    switch_group_.HandleGetRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseHasError(response);
    EXPECT_THAT(response, HasSubstr(R"("ErrorNumber": 123)"));
    EXPECT_THAT(response, HasSubstr(absl::StrCat(R"("ErrorMessage": ")",
                                                 kErrorMessage, "\"")));
  }
}

TEST_F(SwitchGroupTest, GetMinSwitchValue) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(1));

  request_.device_method = EDeviceMethod::kMinSwitchValue;
  request_.set_id(0);
  EXPECT_CALL(switch_group_, GetMinSwitchValue(0)).WillOnce(Return(1.23));

  mcucore::test::PrintToStdString out;
  switch_group_.HandleGetRequest(request_, out);
  const std::string response = out.str();
  VerifyResponseIsGood(response);
  EXPECT_THAT(response, HasSubstr(R"("Value": 1.23)"));
}

TEST_F(SwitchGroupTest, GetMaxSwitchValue) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(1));

  request_.device_method = EDeviceMethod::kMaxSwitchValue;
  request_.set_id(0);
  EXPECT_CALL(switch_group_, GetMaxSwitchValue(0)).WillOnce(Return(1000.001));

  mcucore::test::PrintToStdString out;
  switch_group_.HandleGetRequest(request_, out);
  const std::string response = out.str();
  VerifyResponseIsGood(response);
  EXPECT_THAT(response, HasSubstr(R"("Value": 1000.00,)"));
}

TEST_F(SwitchGroupTest, GetSwitchStep) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(1));

  request_.device_method = EDeviceMethod::kSwitchStep;
  request_.set_id(0);
  EXPECT_CALL(switch_group_, GetSwitchStep(0)).WillOnce(Return(1));

  mcucore::test::PrintToStdString out;
  switch_group_.HandleGetRequest(request_, out);
  const std::string response = out.str();
  VerifyResponseIsGood(response);
  EXPECT_THAT(response, HasSubstr(R"("Value": 1.00,)"));
}

TEST_F(SwitchGroupTest, SetSwitch_MissingState) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(1));

  request_.device_method = EDeviceMethod::kSetSwitch;
  request_.set_id(0);
  request_.set_value(1.23);  // Should be ignored.
  EXPECT_CALL(switch_group_, SetSwitch).Times(0);

  mcucore::test::PrintToStdString out;
  switch_group_.HandlePutRequest(request_, out);
  const std::string response = out.str();
  VerifyResponseHasError(response);

  EXPECT_THAT(response,
              HasSubstr(R"("ErrorMessage": "Missing parameter: State")"));
  EXPECT_THAT(response, Not(HasSubstr(R"("Value":)")));
}

TEST_F(SwitchGroupTest, SetSwitch) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(1));

  request_.device_method = EDeviceMethod::kSetSwitch;
  request_.set_id(0);
  request_.set_state(false);
  EXPECT_CALL(switch_group_, SetSwitch(0, false)).WillOnce(Return(OkStatus()));

  mcucore::test::PrintToStdString out;
  switch_group_.HandlePutRequest(request_, out);
  const std::string response = out.str();
  VerifyResponseIsGood(response);
  EXPECT_THAT(response, Not(HasSubstr(R"("Value":)")));
}

TEST_F(SwitchGroupTest, SetSwitchValue_MissingValue) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(1));

  request_.device_method = EDeviceMethod::kSetSwitchValue;
  request_.set_id(0);
  request_.set_state(false);  // Should be ignored.
  EXPECT_CALL(switch_group_, SetSwitchValue).Times(0);

  mcucore::test::PrintToStdString out;
  switch_group_.HandlePutRequest(request_, out);
  const std::string response = out.str();
  VerifyResponseHasError(response);

  EXPECT_THAT(response,
              HasSubstr(R"("ErrorMessage": "Missing parameter: Value")"));
  EXPECT_THAT(response, Not(HasSubstr(R"("Value":)")));
}

TEST_F(SwitchGroupTest, SetSwitchValue_InvalidValue) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(1));
  EXPECT_CALL(switch_group_, GetMinSwitchValue).WillRepeatedly(Return(0.0));
  EXPECT_CALL(switch_group_, GetMaxSwitchValue).WillRepeatedly(Return(1.0));

  request_.device_method = EDeviceMethod::kSetSwitchValue;
  request_.set_id(0);
  request_.set_value(1.001);
  {
    mcucore::test::PrintToStdString out;
    switch_group_.HandlePutRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseHasError(response);

    EXPECT_THAT(response,
                HasSubstr(R"("ErrorMessage": "Invalid parameter: Value")"));
  }

  InitializeRequest();
  request_.device_method = EDeviceMethod::kSetSwitchValue;
  request_.set_id(0);
  request_.set_value(-0.001);
  {
    mcucore::test::PrintToStdString out;
    switch_group_.HandlePutRequest(request_, out);
    const std::string response = out.str();
    VerifyResponseHasError(response);

    EXPECT_THAT(response,
                HasSubstr(R"("ErrorMessage": "Invalid parameter: Value")"));
  }
}

TEST_F(SwitchGroupTest, SetSwitchValue) {
  EXPECT_CALL(switch_group_, GetMaxSwitch).WillRepeatedly(Return(1));
  EXPECT_CALL(switch_group_, GetMinSwitchValue).WillRepeatedly(Return(0));
  EXPECT_CALL(switch_group_, GetMaxSwitchValue).WillRepeatedly(Return(2));

  request_.device_method = EDeviceMethod::kSetSwitchValue;
  request_.set_id(0);
  request_.set_value(1.23);
  EXPECT_CALL(switch_group_, SetSwitchValue(0, 1.23))
      .WillOnce(Return(OkStatus()));

  mcucore::test::PrintToStdString out;
  switch_group_.HandlePutRequest(request_, out);
  const std::string response = out.str();
  VerifyResponseIsGood(response);
  EXPECT_THAT(response, Not(HasSubstr(R"("Value":)")));
}

}  // namespace
}  // namespace test
}  // namespace alpaca
