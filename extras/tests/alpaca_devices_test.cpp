#include "alpaca_devices.h"

#include <string>

#include "absl/strings/str_cat.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "device_interface.h"
#include "extras/test_tools/mock_device_interface.h"
#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "utils/json_encoder_helpers.h"

namespace alpaca {
namespace test {
namespace {

using ::testing::EndsWith;
using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::NiceMock;
using ::testing::Not;
using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::StartsWith;

TEST(AlpacaDevicesNoFixtureTest, NoDevices) {
  DeviceInterface* device_ptrs[] = {nullptr};
  ArrayView<DeviceInterface*> view(device_ptrs, 0);
  AlpacaDevices devices(view);
  EXPECT_TRUE(devices.Initialize());
  devices.MaintainDevices();
  {
    AlpacaRequest request;
    request.http_method = EHttpMethod::GET;
    request.api_group = EApiGroup::kManagement;
    request.api = EAlpacaApi::kManagementConfiguredDevices;
    request.set_server_transaction_id(123);

    PrintToStdString out;
    EXPECT_TRUE(devices.HandleManagementConfiguredDevices(request, out));
    EXPECT_THAT(out.str(), StartsWith("HTTP/1.1 200 OK"));
    EXPECT_THAT(out.str(), Not(HasSubstr("Connection: close")));
    EXPECT_THAT(out.str(), HasSubstr("Content-Type: application/json"));
    EXPECT_THAT(out.str(), HasSubstr("\r\n\r\n{\"Value\": [], "));
    EXPECT_THAT(out.str(), HasSubstr("\"ServerTransactionID\": 123"));
  }
  {
    AlpacaRequest request;
    request.http_method = EHttpMethod::GET;
    request.api_group = EApiGroup::kSetup;
    request.api = EAlpacaApi::kDeviceSetup;
    request.device_type = EDeviceType::kCamera;
    request.device_number = 0;
    request.device_method = EDeviceMethod::kConnected;

    PrintToStdString out;
    EXPECT_FALSE(devices.DispatchDeviceRequest(request, out));
    EXPECT_THAT(out.str(), StartsWith("HTTP/1.1 400 Bad Request"));
    EXPECT_THAT(out.str(), HasSubstr("Connection: close"));
    EXPECT_THAT(out.str(), HasSubstr("Content-Type: text/plain"));
    EXPECT_THAT(out.str(), EndsWith("Unknown device"));
  }
}

void AddDefaultBehavior(const alpaca::DeviceInfo& device_info,
                        MockDeviceInterface* device_mock) {
  ON_CALL(*device_mock, device_info).WillByDefault(ReturnRef(device_info));
  ON_CALL(*device_mock, device_type)
      .WillByDefault(Return(device_info.device_type));
  ON_CALL(*device_mock, device_number)
      .WillByDefault(Return(device_info.device_number));

  ON_CALL(*device_mock, Initialize).WillByDefault(Return());
  ON_CALL(*device_mock, MaintainDevice).WillByDefault(Return());
}

class AlpacaDevicesTest : public testing::Test {
 protected:
  void SetUp() override {
    AddDefaultBehavior(mock_camera0_info_, &mock_camera0_);
    AddDefaultBehavior(mock_camera22_info_, &mock_camera22_);
    AddDefaultBehavior(mock_observing_conditions1_info_,
                       &mock_observing_conditions1_);
  }

  const alpaca::DeviceInfo mock_camera0_info_{
      .device_type = alpaca::EDeviceType::kCamera,
      .device_number = 0,
      .name = TASLIT("Camera Name"),
      .unique_id = TASLIT("Camera Unique Id"),
      .description = TASLIT("Camera Description"),
      .driver_info = TASLIT("Camera Driver Info"),
      .driver_version = TASLIT("Camera Driver Version"),
      .supported_actions = {},
      .interface_version = 1,
  };
  const std::string kConfiguredDeviceCamera0 =
      R"({"DeviceName": "Camera Name", "DeviceType": "Camera", )"
      R"("DeviceNumber": 0, "UniqueID": "Camera Unique Id"})";

  const alpaca::DeviceInfo mock_camera22_info_{
      .device_type = alpaca::EDeviceType::kCamera,
      .device_number = 22,
      .name = TASLIT("Camera22 Name"),
      .unique_id = TASLIT("Camera22 Unique Id"),
      .description = TASLIT("Camera22 Description"),
      .driver_info = TASLIT("Camera22 Driver Info"),
      .driver_version = TASLIT("Camera22 Driver Version"),
      .supported_actions = {},
      .interface_version = 1,
  };
  const std::string kConfiguredDeviceCamera22 =
      R"({"DeviceName": "Camera22 Name", "DeviceType": "Camera", )"
      R"("DeviceNumber": 22, "UniqueID": "Camera22 Unique Id"})";

  const alpaca::DeviceInfo mock_observing_conditions1_info_{
      .device_type = alpaca::EDeviceType::kObservingConditions,
      .device_number = 1,
      .name = TASLIT("Weather"),
      .unique_id = TASLIT("Weather UUID"),
      .description = TASLIT("Environment"),
      .driver_info = TASLIT("WeatherStation"),
      .driver_version = TASLIT("Ver."),
      .supported_actions = {},
      .interface_version = 1,
  };
  const std::string kConfiguredDeviceObservingConditions1 =
      R"({"DeviceName": "Weather", "DeviceType": "ObservingConditions", )"
      R"("DeviceNumber": 1, "UniqueID": "Weather UUID"})";

  NiceMock<MockDeviceInterface> mock_camera0_;
  NiceMock<MockDeviceInterface> mock_camera22_;
  NiceMock<MockDeviceInterface> mock_observing_conditions1_;

  DeviceInterface* device_ptrs_[3] = {&mock_camera0_, &mock_camera22_,
                                      &mock_observing_conditions1_};
  AlpacaDevices alpaca_devices_{MakeArrayView(device_ptrs_)};
};

TEST_F(AlpacaDevicesTest, Initialize) {
  EXPECT_CALL(mock_camera0_, Initialize);
  EXPECT_CALL(mock_camera22_, Initialize);
  EXPECT_CALL(mock_observing_conditions1_, Initialize);
  EXPECT_TRUE(alpaca_devices_.Initialize());
}

TEST_F(AlpacaDevicesTest, MaintainDevices) {
  EXPECT_CALL(mock_camera0_, MaintainDevice);
  EXPECT_CALL(mock_camera22_, MaintainDevice);
  EXPECT_CALL(mock_observing_conditions1_, MaintainDevice);

  alpaca_devices_.MaintainDevices();
}

TEST_F(AlpacaDevicesTest, OneConfiguredDevice) {
  EXPECT_CALL(mock_camera0_, device_info)
      .Times(2)
      .WillRepeatedly(ReturnRef(mock_camera0_info_));

  DeviceInterface* device_ptrs[] = {&mock_camera0_};
  AlpacaDevices devices(MakeArrayView(device_ptrs));

  AlpacaRequest request;
  request.http_method = EHttpMethod::GET;
  request.set_client_transaction_id(222);
  request.set_server_transaction_id(111);
  request.api_group = EApiGroup::kManagement;
  request.api = EAlpacaApi::kManagementConfiguredDevices;

  PrintToStdString out;
  EXPECT_TRUE(devices.HandleManagementConfiguredDevices(request, out));
  VLOG(1) << "out:\n\n" << out.str() << "\n\n";

  EXPECT_THAT(out.str(), StartsWith("HTTP/1.1 200 OK"));
  EXPECT_THAT(out.str(), Not(HasSubstr("Connection: close")));
  EXPECT_THAT(out.str(), HasSubstr("Content-Type: application/json"));
  EXPECT_THAT(out.str(), HasSubstr("\r\n\r\n{\"Value\": ["));
  EXPECT_THAT(out.str(),
              HasSubstr(absl::StrCat(R"("Value": [)", kConfiguredDeviceCamera0,
                                     "], ")));
  EXPECT_THAT(
      out.str(),
      HasSubstr("\"ClientTransactionID\": 222, \"ServerTransactionID\": 111"));
  EXPECT_THAT(out.str(), EndsWith("}\r\n"));
  EXPECT_THAT(out.str(), Not(HasSubstr("\"ErrorNumber\":")));
  EXPECT_THAT(out.str(), Not(HasSubstr("\"ErrorMessage\":")));
}

TEST_F(AlpacaDevicesTest, ThreeConfiguredDevices) {
  AlpacaRequest request;
  request.http_method = EHttpMethod::GET;
  request.set_client_transaction_id(222);
  request.set_server_transaction_id(111);
  request.api_group = EApiGroup::kManagement;
  request.api = EAlpacaApi::kManagementConfiguredDevices;

  PrintToStdString out;
  EXPECT_TRUE(alpaca_devices_.HandleManagementConfiguredDevices(request, out));
  VLOG(1) << "out:\n\n" << out.str() << "\n\n";

  EXPECT_THAT(out.str(), StartsWith("HTTP/1.1 200 OK"));
  EXPECT_THAT(out.str(), Not(HasSubstr("Connection: close")));
  EXPECT_THAT(out.str(), HasSubstr("Content-Type: application/json"));
  EXPECT_THAT(out.str(), HasSubstr("\r\n\r\n{\"Value\": ["));
  EXPECT_THAT(out.str(), HasSubstr(absl::StrCat(
                             R"("Value": [)", kConfiguredDeviceCamera0, ", ",
                             kConfiguredDeviceCamera22, ", ",
                             kConfiguredDeviceObservingConditions1, "], ")));
  EXPECT_THAT(
      out.str(),
      HasSubstr("\"ClientTransactionID\": 222, \"ServerTransactionID\": 111"));
  EXPECT_THAT(out.str(), EndsWith("}\r\n"));
}

TEST_F(AlpacaDevicesTest, SetupUnknownDevice) {
  AlpacaRequest request;
  request.http_method = EHttpMethod::GET;
  request.api_group = EApiGroup::kSetup;
  request.api = EAlpacaApi::kDeviceSetup;
  request.device_type = EDeviceType::kCamera;
  request.device_number = 99999999;
  request.device_method = EDeviceMethod::kConnected;

  PrintToStdString out;
  EXPECT_FALSE(alpaca_devices_.DispatchDeviceRequest(request, out));
  EXPECT_THAT(out.str(), StartsWith("HTTP/1.1 400 Bad Request"));
  EXPECT_THAT(out.str(), HasSubstr("Connection: close"));
  EXPECT_THAT(out.str(), HasSubstr("Content-Type: text/plain"));
  EXPECT_THAT(out.str(), EndsWith("Unknown device"));
}

TEST_F(AlpacaDevicesTest, DispatchDeviceRequest) {
  AlpacaRequest request;
  request.http_method = EHttpMethod::GET;
  request.api_group = EApiGroup::kDevice;
  request.api = EAlpacaApi::kDeviceApi;

  request.device_type = EDeviceType::kCamera;
  request.device_number = 22;
  request.device_method = EDeviceMethod::kConnected;
  PrintToStdString out;

  EXPECT_CALL(mock_camera22_, HandleDeviceApiRequest(Ref(request), Ref(out)))
      .WillOnce(Return(true));

  EXPECT_TRUE(alpaca_devices_.DispatchDeviceRequest(request, out));
  EXPECT_THAT(out.str(), IsEmpty());
}

using AlpacaDevicesDeathTest = AlpacaDevicesTest;

TEST_F(AlpacaDevicesDeathTest, NullDevice) {
  DeviceInterface* device_ptrs[] = {&mock_camera0_, nullptr};
  AlpacaDevices devices(MakeArrayView(device_ptrs));

  EXPECT_CALL(mock_camera0_, Initialize).Times(0);

  EXPECT_DEBUG_DEATH({ EXPECT_FALSE(devices.Initialize()); },
                     "\\[1\\] is null");
}

TEST_F(AlpacaDevicesDeathTest, SameDeviceTwice) {
  DeviceInterface* device_ptrs[] = {&mock_camera0_, &mock_camera0_};
  AlpacaDevices devices(MakeArrayView(device_ptrs));

  EXPECT_CALL(mock_camera0_, Initialize).Times(0);

  EXPECT_DEBUG_DEATH({ EXPECT_FALSE(devices.Initialize()); },
                     "Device appears twice");
}

TEST_F(AlpacaDevicesDeathTest, SameUniqueId) {
  MockDeviceInterface second_mock_camera0;
  AddDefaultBehavior(mock_camera0_info_, &second_mock_camera0);

  DeviceInterface* device_ptrs[] = {&mock_camera0_, &second_mock_camera0};
  AlpacaDevices devices(MakeArrayView(device_ptrs));

  EXPECT_CALL(mock_camera0_, Initialize).Times(0);
  EXPECT_CALL(second_mock_camera0, Initialize).Times(0);

  EXPECT_DEBUG_DEATH({ EXPECT_FALSE(devices.Initialize()); }, "same unique_id");
}

TEST_F(AlpacaDevicesDeathTest, SameDeviceTypeAndNumber) {
  MockDeviceInterface second_mock_camera0;
  DeviceInfo alt_info = mock_camera0_info_;
  alt_info.unique_id = TASLIT("alt id");
  AddDefaultBehavior(alt_info, &second_mock_camera0);

  DeviceInterface* device_ptrs[] = {&mock_camera0_, &second_mock_camera0};
  AlpacaDevices devices(MakeArrayView(device_ptrs));

  EXPECT_CALL(mock_camera0_, Initialize).Times(0);
  EXPECT_CALL(second_mock_camera0, Initialize).Times(0);

  EXPECT_DEBUG_DEATH({ EXPECT_FALSE(devices.Initialize()); },
                     "same type and number");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
