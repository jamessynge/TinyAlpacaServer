#include "alpaca_devices.h"

#include <McuCore.h>

#include <cstdint>
#include <string>

#include "alpaca_request.h"
#include "constants.h"
#include "device_info.h"
#include "device_interface.h"
#include "extras/test_tools/alpaca_response_validator.h"
#include "extras/test_tools/mock_device_interface.h"
#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mcucore/extras/test_tools/http_response.h"
#include "mcucore/extras/test_tools/print_to_std_string.h"

MCU_DEFINE_NAMED_DOMAIN(SomeDeviceDomain, 33);
MCU_DEFINE_NAMED_DOMAIN(SecondDeviceDomain, 34);
MCU_DEFINE_DOMAIN(35);

namespace alpaca {
namespace test {
namespace {

using ::mcucore::MakeArrayView;
using ::mcucore::test::JsonValue;
using ::testing::EndsWith;
using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::MatchesRegex;
using ::testing::NiceMock;
using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnRef;

const char kUuidRegex[] = R"re([0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-)re"
                          R"re([0-9A-Fa-f]{4}-[0-9A-Fa-f]{12})re";

TEST(AlpacaDevicesNoFixtureTest, NoDevices) {
  DeviceInterface* device_ptrs[] = {nullptr};
  mcucore::ArrayView<DeviceInterface*> view(device_ptrs, 0);
  AlpacaDevices devices(view);
  EXPECT_TRUE(devices.Initialize());
  devices.MaintainDevices();
  {
    AlpacaRequest request;
    request.http_method = EHttpMethod::GET;
    request.api_group = EApiGroup::kManagement;
    request.api = EAlpacaApi::kManagementConfiguredDevices;
    request.set_server_transaction_id(123);

    mcucore::test::PrintToStdString out;
    EXPECT_TRUE(devices.HandleManagementConfiguredDevices(request, out));
    VLOG(1) << "out:\n\n" << out.str() << "\n\n";

    AlpacaResponseValidator alpaca_response_validator;
    alpaca_response_validator.SetTransactionIdsFromAlpacaRequest(request);
    ASSERT_OK_AND_ASSIGN(
        auto configured_devices_jv,
        alpaca_response_validator.ValidateArrayValueResponse(out.str()));
    ASSERT_EQ(configured_devices_jv.size(), 0);
  }
  {
    AlpacaRequest request;
    request.http_method = EHttpMethod::GET;
    request.api_group = EApiGroup::kSetup;
    request.api = EAlpacaApi::kDeviceSetup;
    request.device_type = EDeviceType::kCamera;
    request.device_number = 0;
    request.device_method = EDeviceMethod::kConnected;
    request.set_client_transaction_id(0);
    request.set_server_transaction_id(200);

    mcucore::test::PrintToStdString out;
    EXPECT_FALSE(devices.DispatchDeviceRequest(request, out));
    VLOG(1) << "out:\n\n" << out.str() << "\n\n";

    ASSERT_OK_AND_ASSIGN(auto response,
                         mcucore::test::HttpResponse::Make(out.str()));
    EXPECT_EQ(response.http_version, "HTTP/1.1");
    EXPECT_EQ(response.status_code, 400);
    EXPECT_EQ(response.status_message, "Bad Request");
    EXPECT_TRUE(response.HasHeaderValue("Connection", "close"));
    EXPECT_TRUE(response.HasHeaderValue("Content-Type", "text/plain"));
    EXPECT_THAT(response.body_and_beyond, HasSubstr("Not found: api="));
    EXPECT_THAT(response.body_and_beyond, EndsWith("number=0"));
  }
}

void AddDefaultBehavior(const alpaca::DeviceInfo& device_info,
                        MockDeviceInterface* device_mock) {
  ON_CALL(*device_mock, device_info).WillByDefault(ReturnRef(device_info));
  ON_CALL(*device_mock, Initialize).WillByDefault(Return());
  ON_CALL(*device_mock, MaintainDevice).WillByDefault(Return());
}

class AlpacaDevicesTest : public testing::Test {
 protected:
  void SetUp() override {
    AddDefaultBehavior(mock_camera0_info_, &mock_camera0_);
    AddDefaultBehavior(mock_camera1_info_, &mock_camera1_);
    AddDefaultBehavior(mock_observing_conditions0_info_,
                       &mock_observing_conditions0_);
    mcucore::EepromTlv::ClearAndInitializeEeprom();
  }

  const alpaca::DeviceInfo mock_camera0_info_{
      .device_type = alpaca::EDeviceType::kCamera,
      .device_number = 0,
      .domain = MCU_DOMAIN(SomeDeviceDomain),
      .name = MCU_FLASHSTR("Camera Name"),
      .description = MCU_FLASHSTR("Camera Description"),
      .driver_info = MCU_FLASHSTR("Camera Driver Info"),
      .driver_version = MCU_FLASHSTR("Camera Driver Version"),
      .supported_actions = {},
      .interface_version = 1,
  };
  const std::string kConfiguredDeviceCamera0 =
      R"({"DeviceName": "Camera Name", "DeviceType": "Camera", )"
      R"("DeviceNumber": 0, "UniqueID": "Camera Unique Id"})";

  const alpaca::DeviceInfo mock_camera1_info_{
      .device_type = alpaca::EDeviceType::kCamera,
      .device_number = 1,
      .domain = MCU_DOMAIN(SecondDeviceDomain),
      .name = MCU_FLASHSTR("Camera1 Name"),
      .description = MCU_FLASHSTR("Camera1 Description"),
      .driver_info = MCU_FLASHSTR("Camera1 Driver Info"),
      .driver_version = MCU_FLASHSTR("Camera1 Driver Version"),
      .supported_actions = {},
      .interface_version = 1,
  };
  const std::string kConfiguredDeviceCamera22 =
      R"({"DeviceName": "Camera1 Name", "DeviceType": "Camera", )"
      R"("DeviceNumber": 22, "UniqueID": "Camera1 Unique Id"})";

  const alpaca::DeviceInfo mock_observing_conditions0_info_{
      .device_type = alpaca::EDeviceType::kObservingConditions,
      .device_number = 0,
      .domain = MCU_DOMAIN(35),
      .name = MCU_FLASHSTR("Weather"),
      .description = MCU_FLASHSTR("Environment"),
      .driver_info = MCU_FLASHSTR("WeatherStation"),
      .driver_version = MCU_FLASHSTR("Ver."),
      .supported_actions = {},
      .interface_version = 1,
  };
  const std::string kConfiguredDeviceObservingConditions1 =
      R"({"DeviceName": "Weather", "DeviceType": "ObservingConditions", )"
      R"("DeviceNumber": 1, "UniqueID": "Weather UUID"})";

  NiceMock<MockDeviceInterface> mock_camera0_;
  NiceMock<MockDeviceInterface> mock_camera1_;
  NiceMock<MockDeviceInterface> mock_observing_conditions0_;

  DeviceInterface* device_ptrs_[3] = {&mock_camera0_, &mock_camera1_,
                                      &mock_observing_conditions0_};
  AlpacaDevices alpaca_devices_{MakeArrayView(device_ptrs_)};
  AlpacaResponseValidator alpaca_response_validator_;
};

TEST_F(AlpacaDevicesTest, Initialize) {
  EXPECT_CALL(mock_camera0_, Initialize);
  EXPECT_CALL(mock_camera1_, Initialize);
  EXPECT_CALL(mock_observing_conditions0_, Initialize);
  EXPECT_TRUE(alpaca_devices_.Initialize());
}

TEST_F(AlpacaDevicesTest, MaintainDevices) {
  EXPECT_CALL(mock_camera0_, MaintainDevice);
  EXPECT_CALL(mock_camera1_, MaintainDevice);
  EXPECT_CALL(mock_observing_conditions0_, MaintainDevice);

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
  request.set_client_transaction_id(17);
  request.set_server_transaction_id(1000001);
  request.api_group = EApiGroup::kManagement;
  request.api = EAlpacaApi::kManagementConfiguredDevices;

  alpaca_response_validator_.expected_client_transaction_id =
      request.client_transaction_id;
  alpaca_response_validator_.expected_server_transaction_id =
      request.server_transaction_id;

  mcucore::test::PrintToStdString out;
  EXPECT_TRUE(devices.HandleManagementConfiguredDevices(request, out));
  VLOG(1) << "out:\n\n" << out.str() << "\n\n";

  ASSERT_OK_AND_ASSIGN(
      auto configured_devices_jv,
      alpaca_response_validator_.ValidateArrayValueResponse(out.str()));
  ASSERT_EQ(configured_devices_jv.size(), 1);

  auto configured_device_jv = configured_devices_jv.GetElement(0);
  ASSERT_TRUE(configured_device_jv.is_object());
  EXPECT_EQ(configured_device_jv.GetValue("DeviceName"), "Camera Name");
  EXPECT_EQ(configured_device_jv.GetValue("DeviceType"), "Camera");
  EXPECT_EQ(configured_device_jv.GetValue("DeviceNumber"), 0);
  ASSERT_OK(configured_device_jv.HasKeyOfType("UniqueID", JsonValue::kString));
  EXPECT_THAT(configured_device_jv.GetValue("UniqueID").as_string(),
              MatchesRegex(kUuidRegex));
}

TEST_F(AlpacaDevicesTest, ThreeConfiguredDevices) {
  AlpacaRequest request;
  request.http_method = EHttpMethod::GET;
  request.set_client_transaction_id(222);
  request.set_server_transaction_id(111);
  request.api_group = EApiGroup::kManagement;
  request.api = EAlpacaApi::kManagementConfiguredDevices;

  alpaca_response_validator_.expected_client_transaction_id = 222;
  alpaca_response_validator_.expected_server_transaction_id = 111;

  mcucore::test::PrintToStdString out;
  EXPECT_TRUE(alpaca_devices_.HandleManagementConfiguredDevices(request, out));
  VLOG(1) << "out:\n\n" << out.str() << "\n\n";

  ASSERT_OK_AND_ASSIGN(
      auto configured_devices_jv,
      alpaca_response_validator_.ValidateArrayValueResponse(out.str()));
  ASSERT_EQ(configured_devices_jv.size(), 3);

  {
    auto configured_device_jv = configured_devices_jv.GetElement(0);
    ASSERT_TRUE(configured_device_jv.is_object());
    EXPECT_EQ(configured_device_jv.GetValue("DeviceName"), "Camera Name");
    EXPECT_EQ(configured_device_jv.GetValue("DeviceType"), "Camera");
    EXPECT_EQ(configured_device_jv.GetValue("DeviceNumber"), 0);
    ASSERT_OK(
        configured_device_jv.HasKeyOfType("UniqueID", JsonValue::kString));
    EXPECT_THAT(configured_device_jv.GetValue("UniqueID").as_string(),
                MatchesRegex(kUuidRegex));
  }

  {
    auto configured_device_jv = configured_devices_jv.GetElement(1);
    ASSERT_TRUE(configured_device_jv.is_object());
    EXPECT_EQ(configured_device_jv.GetValue("DeviceName"), "Camera1 Name");
    EXPECT_EQ(configured_device_jv.GetValue("DeviceType"), "Camera");
    EXPECT_EQ(configured_device_jv.GetValue("DeviceNumber"), 1);
    ASSERT_OK(
        configured_device_jv.HasKeyOfType("UniqueID", JsonValue::kString));
    EXPECT_THAT(configured_device_jv.GetValue("UniqueID").as_string(),
                MatchesRegex(kUuidRegex));
  }

  {
    auto configured_device_jv = configured_devices_jv.GetElement(2);
    ASSERT_TRUE(configured_device_jv.is_object());
    EXPECT_EQ(configured_device_jv.GetValue("DeviceName"), "Weather");
    EXPECT_EQ(configured_device_jv.GetValue("DeviceType"),
              "ObservingConditions");
    EXPECT_EQ(configured_device_jv.GetValue("DeviceNumber"), 0);
    ASSERT_OK(
        configured_device_jv.HasKeyOfType("UniqueID", JsonValue::kString));
    EXPECT_THAT(configured_device_jv.GetValue("UniqueID").as_string(),
                MatchesRegex(kUuidRegex));
  }
}

TEST_F(AlpacaDevicesTest, StatusPageHead) {
  AlpacaRequest request;
  request.http_method = EHttpMethod::GET;
  request.set_client_transaction_id(222);
  request.set_server_transaction_id(111);
  request.api_group = EApiGroup::kServerStatus;
  request.api = EAlpacaApi::kServerStatus;

  mcucore::test::PrintToStdString out;
  mcucore::OPrintStream strm(out);

  EXPECT_CALL(
      mock_camera0_,
      AddToHomePageHtml(Ref(request), EHtmlPageSection::kHead, Ref(strm)));
  EXPECT_CALL(
      mock_camera1_,
      AddToHomePageHtml(Ref(request), EHtmlPageSection::kHead, Ref(strm)));
  EXPECT_CALL(
      mock_observing_conditions0_,
      AddToHomePageHtml(Ref(request), EHtmlPageSection::kHead, Ref(strm)));

  alpaca_devices_.AddToHomePageHtml(request, EHtmlPageSection::kHead, strm);
  LOG(INFO) << "out:\n\n" << out.str() << "\n\n";
}

TEST_F(AlpacaDevicesTest, StatusPageBody) {
  AlpacaRequest request;
  request.http_method = EHttpMethod::GET;
  request.set_client_transaction_id(222);
  request.set_server_transaction_id(111);
  request.api_group = EApiGroup::kServerStatus;
  request.api = EAlpacaApi::kServerStatus;

  mcucore::test::PrintToStdString out;
  mcucore::OPrintStream strm(out);

  EXPECT_CALL(
      mock_camera0_,
      AddToHomePageHtml(Ref(request), EHtmlPageSection::kBody, Ref(strm)));
  EXPECT_CALL(
      mock_camera1_,
      AddToHomePageHtml(Ref(request), EHtmlPageSection::kBody, Ref(strm)));
  EXPECT_CALL(
      mock_observing_conditions0_,
      AddToHomePageHtml(Ref(request), EHtmlPageSection::kBody, Ref(strm)));

  alpaca_devices_.AddToHomePageHtml(request, EHtmlPageSection::kBody, strm);
  LOG(INFO) << "out:\n\n" << out.str() << "\n\n";
}

TEST_F(AlpacaDevicesTest, StatusPageTrailer) {
  AlpacaRequest request;
  request.http_method = EHttpMethod::GET;
  request.set_client_transaction_id(222);
  request.set_server_transaction_id(111);
  request.api_group = EApiGroup::kServerStatus;
  request.api = EAlpacaApi::kServerStatus;

  mcucore::test::PrintToStdString out;
  mcucore::OPrintStream strm(out);

  EXPECT_CALL(
      mock_camera0_,
      AddToHomePageHtml(Ref(request), EHtmlPageSection::kTrailer, Ref(strm)));
  EXPECT_CALL(
      mock_camera1_,
      AddToHomePageHtml(Ref(request), EHtmlPageSection::kTrailer, Ref(strm)));
  EXPECT_CALL(
      mock_observing_conditions0_,
      AddToHomePageHtml(Ref(request), EHtmlPageSection::kTrailer, Ref(strm)));

  alpaca_devices_.AddToHomePageHtml(request, EHtmlPageSection::kTrailer, strm);
  LOG(INFO) << "out:\n\n" << out.str() << "\n\n";
}

TEST_F(AlpacaDevicesTest, SetupUnknownDevice) {
  AlpacaRequest request;
  request.http_method = EHttpMethod::GET;
  request.api_group = EApiGroup::kSetup;
  request.api = EAlpacaApi::kDeviceSetup;
  request.device_type = EDeviceType::kCamera;
  request.device_number = 99999999;
  request.device_method = EDeviceMethod::kConnected;

  mcucore::test::PrintToStdString out;
  EXPECT_FALSE(alpaca_devices_.DispatchDeviceRequest(request, out));

  ASSERT_OK_AND_ASSIGN(auto response,
                       mcucore::test::HttpResponse::Make(out.str()));
  EXPECT_EQ(response.http_version, "HTTP/1.1");
  EXPECT_EQ(response.status_code, 400);
  EXPECT_EQ(response.status_message, "Bad Request");
  EXPECT_TRUE(response.HasHeaderValue("Connection", "close"));
  EXPECT_TRUE(response.HasHeaderValue("Content-Type", "text/plain"));
  EXPECT_THAT(response.body_and_beyond, HasSubstr("Not found: api="));
  EXPECT_THAT(response.body_and_beyond, EndsWith("number=99999999"));
}

TEST_F(AlpacaDevicesTest, DispatchDeviceRequest) {
  AlpacaRequest request;
  request.http_method = EHttpMethod::GET;
  request.api_group = EApiGroup::kDevice;
  request.api = EAlpacaApi::kDeviceApi;

  request.device_type = EDeviceType::kCamera;
  request.device_number = 1;
  request.device_method = EDeviceMethod::kConnected;
  mcucore::test::PrintToStdString out;

  EXPECT_CALL(mock_camera1_, HandleDeviceApiRequest(Ref(request), Ref(out)))
      .WillOnce(Return(true));

  EXPECT_TRUE(alpaca_devices_.DispatchDeviceRequest(request, out));
  EXPECT_THAT(out.str(), IsEmpty());
}

using AlpacaDevicesDeathTest = AlpacaDevicesTest;

TEST_F(AlpacaDevicesDeathTest, NullDevice) {
  DeviceInterface* device_ptrs[] = {&mock_camera0_, nullptr};
  AlpacaDevices devices(MakeArrayView(device_ptrs));

  EXPECT_CALL(mock_camera0_, Initialize).Times(0);

  EXPECT_DEATH({ EXPECT_FALSE(devices.Initialize()); }, "\\[1\\] is null");
}

TEST_F(AlpacaDevicesDeathTest, SameDeviceTwice) {
  DeviceInterface* device_ptrs[] = {&mock_camera0_, &mock_camera0_};
  AlpacaDevices devices(MakeArrayView(device_ptrs));

  EXPECT_CALL(mock_camera0_, Initialize).Times(0);

  EXPECT_DEATH({ EXPECT_FALSE(devices.Initialize()); }, "Device appears twice");
}

TEST_F(AlpacaDevicesDeathTest, SameDeviceTypeAndNumber) {
  DeviceInfo copy = mock_camera0_info_;
  copy.domain = MCU_DOMAIN(35);
  AddDefaultBehavior(copy, &mock_camera1_);

  DeviceInterface* device_ptrs[] = {&mock_camera0_, &mock_camera1_};
  AlpacaDevices devices(MakeArrayView(device_ptrs));

  EXPECT_CALL(mock_camera0_, Initialize).Times(0);
  EXPECT_CALL(mock_camera1_, Initialize).Times(0);

  EXPECT_DEATH({ EXPECT_FALSE(devices.Initialize()); }, "same type and number");
}

TEST_F(AlpacaDevicesDeathTest, SameDomain) {
  DeviceInfo copy = mock_observing_conditions0_info_;
  copy.domain = mock_camera1_info_.domain;
  AddDefaultBehavior(copy, &mock_observing_conditions0_);

  DeviceInterface* device_ptrs[] = {&mock_camera1_,
                                    &mock_observing_conditions0_};
  AlpacaDevices devices(MakeArrayView(device_ptrs));

  EXPECT_CALL(mock_camera1_, Initialize).Times(0);
  EXPECT_CALL(mock_observing_conditions0_, Initialize).Times(0);

  EXPECT_DEATH({ EXPECT_FALSE(devices.Initialize()); }, "have the same domain");
}

TEST_F(AlpacaDevicesDeathTest, SameUuid) {
  // Set the UUID for both devices to the same value, even though they have
  // different domains.
  {
    constexpr uint8_t bytes[16] = {
        0x46, 0xb4, 0xd9, 0x5c, 0x53, 0x88, 0x43, 0x38,
        0x91, 0xb9, 0x05, 0x1c, 0x5d, 0xab, 0xc0, 0x9e,
    };
    mcucore::Uuid uuid;
    uuid.SetForTest(bytes);
    auto tlv = mcucore::EepromTlv::GetOrDie();
    mock_camera0_info_.SetUuidForTest(tlv, uuid);
    mock_camera1_info_.SetUuidForTest(tlv, uuid);
  }

  DeviceInterface* device_ptrs[] = {&mock_camera0_, &mock_camera1_};
  AlpacaDevices devices(MakeArrayView(device_ptrs));

  EXPECT_CALL(mock_camera0_, Initialize).Times(0);
  EXPECT_CALL(mock_camera1_, Initialize).Times(0);

  EXPECT_DEATH({ EXPECT_FALSE(devices.Initialize()); }, "have the same UUID");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
