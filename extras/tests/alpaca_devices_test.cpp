#include "alpaca_devices.h"

#include <McuCore.h>

#include <cstdint>
#include <string>

#include "absl/log/log.h"
#include "alpaca_request.h"
#include "constants.h"
#include "device_description.h"
#include "device_interface.h"
#include "extras/test_tools/alpaca_response_validator.h"
#include "extras/test_tools/minimal_device.h"
#include "extras/test_tools/mock_device_interface.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mcucore/extras/test_tools/http_response.h"
#include "mcucore/extras/test_tools/print_to_std_string.h"
#include "mcucore/extras/test_tools/status_test_utils.h"
#include "mcucore/extras/test_tools/uuid_utils.h"
#include "server_context.h"

MCU_DEFINE_NAMED_DOMAIN(SomeDeviceDomain, 33);
MCU_DEFINE_NAMED_DOMAIN(SecondDeviceDomain, 34);
MCU_DEFINE_DOMAIN(35);

namespace alpaca {
namespace test {
namespace {

using ::mcucore::MakeArrayView;
using ::mcucore::test::JsonValue;
using ::mcucore::test::kUuidRegex;
using ::testing::EndsWith;
using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::MatchesRegex;
using ::testing::NiceMock;
using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnRef;

TEST(AlpacaDevicesNoFixtureTest, NoDevices) {
  ServerContext server_context;
  ASSERT_STATUS_OK(server_context.Initialize());
  DeviceInterface* device_ptrs[] = {nullptr};
  mcucore::ArrayView<DeviceInterface*> view(device_ptrs, 0);
  AlpacaDevices devices(server_context, view);
  devices.ValidateDevices();
  devices.ResetHardware();
  devices.InitializeDevices();
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

void AddDefaultBehavior(const alpaca::DeviceDescription& device_description,
                        MockDeviceInterface& device_mock) {
  ON_CALL(device_mock, device_description)
      .WillByDefault(ReturnRef(device_description));
  ON_CALL(device_mock, MaintainDevice).WillByDefault(Return());
}

void ExpectInitialization(MockDeviceInterface& device_mock, int times = 1) {
  EXPECT_CALL(device_mock, ValidateConfiguration).Times(times);
  EXPECT_CALL(device_mock, ResetHardware).Times(times);
  EXPECT_CALL(device_mock, InitializeDevice).Times(times);
}

void ExpectNoInitialization(MockDeviceInterface& device_mock) {
  ExpectInitialization(device_mock, /*times=*/0);
}

class AlpacaDevicesTest : public testing::Test {
 protected:
  void SetUp() override {
    EXPECT_STATUS_OK(server_context_.Initialize());
    AddDefaultBehavior(mock_camera0_description_, mock_camera0_);
    AddDefaultBehavior(mock_camera1_description_, mock_camera1_);
    AddDefaultBehavior(mock_observing_conditions0_description_,
                       mock_observing_conditions0_);
    mcucore::EepromTlv::ClearAndInitializeEeprom();
  }

  const alpaca::DeviceDescription mock_camera0_description_{
      .device_type = alpaca::EDeviceType::kCamera,
      .device_number = 0,
      .domain = MCU_DOMAIN(SomeDeviceDomain),
      .name = MCU_FLASHSTR("Camera Name"),
      .description = MCU_FLASHSTR("Camera Description"),
      .driver_info = MCU_FLASHSTR("Camera Driver Info"),
      .driver_version = MCU_FLASHSTR("Camera Driver Version"),
      .supported_actions = {},
  };

  const alpaca::DeviceDescription mock_camera1_description_{
      .device_type = alpaca::EDeviceType::kCamera,
      .device_number = 1,
      .domain = MCU_DOMAIN(SecondDeviceDomain),
      .name = MCU_FLASHSTR("Camera1 Name"),
      .description = MCU_FLASHSTR("Camera1 Description"),
      .driver_info = MCU_FLASHSTR("Camera1 Driver Info"),
      .driver_version = MCU_FLASHSTR("Camera1 Driver Version"),
      .supported_actions = {},
  };

  const alpaca::DeviceDescription mock_observing_conditions0_description_{
      .device_type = alpaca::EDeviceType::kObservingConditions,
      .device_number = 0,
      .domain = MCU_DOMAIN(35),
      .name = MCU_FLASHSTR("Weather"),
      .description = MCU_FLASHSTR("Environment"),
      .driver_info = MCU_FLASHSTR("WeatherStation"),
      .driver_version = MCU_FLASHSTR("Ver."),
      .supported_actions = {},
  };

  NiceMock<MockDeviceInterface> mock_camera0_;
  NiceMock<MockDeviceInterface> mock_camera1_;
  NiceMock<MockDeviceInterface> mock_observing_conditions0_;
  DeviceInterface* device_ptrs_[3] = {&mock_camera0_, &mock_camera1_,
                                      &mock_observing_conditions0_};

  ServerContext server_context_;
  AlpacaDevices alpaca_devices_{server_context_, MakeArrayView(device_ptrs_)};
  AlpacaResponseValidator alpaca_response_validator_;
};

TEST_F(AlpacaDevicesTest, Initialize) {
  ExpectInitialization(mock_camera0_);
  ExpectInitialization(mock_camera1_);
  ExpectInitialization(mock_observing_conditions0_);
  alpaca_devices_.ValidateDevices();
  alpaca_devices_.ResetHardware();
  alpaca_devices_.InitializeDevices();
}

TEST_F(AlpacaDevicesTest, MaintainDevices) {
  EXPECT_CALL(mock_camera0_, MaintainDevice);
  EXPECT_CALL(mock_camera1_, MaintainDevice);
  EXPECT_CALL(mock_observing_conditions0_, MaintainDevice);

  alpaca_devices_.MaintainDevices();
}

TEST_F(AlpacaDevicesTest, OneConfiguredDevice) {
  MinimalDevice minimal_camera0_{server_context_, mock_camera0_description_};
  DeviceInterface* device_ptrs[] = {&minimal_camera0_};
  AlpacaDevices devices(server_context_, MakeArrayView(device_ptrs));

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
  MinimalDevice minimal_camera0{server_context_, mock_camera0_description_};
  MinimalDevice minimal_camera1{server_context_, mock_camera1_description_};
  MinimalDevice minimal_observing_conditions0{
      server_context_, mock_observing_conditions0_description_};
  DeviceInterface* device_ptrs[] = {&minimal_camera0, &minimal_camera1,
                                    &minimal_observing_conditions0};
  AlpacaDevices devices(server_context_, MakeArrayView(device_ptrs));

  AlpacaRequest request;
  request.http_method = EHttpMethod::GET;
  request.set_client_transaction_id(222);
  request.set_server_transaction_id(111);
  request.api_group = EApiGroup::kManagement;
  request.api = EAlpacaApi::kManagementConfiguredDevices;

  alpaca_response_validator_.expected_client_transaction_id = 222;
  alpaca_response_validator_.expected_server_transaction_id = 111;

  mcucore::test::PrintToStdString out;
  EXPECT_TRUE(devices.HandleManagementConfiguredDevices(request, out));
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
  AlpacaDevices devices(server_context_, MakeArrayView(device_ptrs));
  ExpectNoInitialization(mock_camera0_);
  EXPECT_DEATH(devices.ValidateDevices(), "\\[1\\] is null");
}

TEST_F(AlpacaDevicesDeathTest, SameDeviceTwice) {
  DeviceInterface* device_ptrs[] = {&mock_camera0_, &mock_camera0_};
  AlpacaDevices devices(server_context_, MakeArrayView(device_ptrs));
  ExpectNoInitialization(mock_camera0_);
  EXPECT_DEATH(devices.ValidateDevices(), "Device appears twice");
}

TEST_F(AlpacaDevicesDeathTest, SameDeviceTypeAndNumber) {
  DeviceDescription copy = mock_camera1_description_;
  copy.device_type = mock_camera0_description_.device_type;
  copy.device_number = mock_camera0_description_.device_number;
  AddDefaultBehavior(copy, mock_camera1_);

  DeviceInterface* device_ptrs[] = {&mock_camera0_, &mock_camera1_};
  AlpacaDevices devices(server_context_, MakeArrayView(device_ptrs));

  ExpectNoInitialization(mock_camera0_);
  ExpectNoInitialization(mock_camera1_);

  EXPECT_DEATH(devices.ValidateDevices(), "same type and number");
}

TEST_F(AlpacaDevicesDeathTest, SameDomain) {
  DeviceDescription copy = mock_observing_conditions0_description_;
  copy.domain = mock_camera1_description_.domain;
  AddDefaultBehavior(copy, mock_observing_conditions0_);

  DeviceInterface* device_ptrs[] = {&mock_camera1_,
                                    &mock_observing_conditions0_};
  AlpacaDevices devices(server_context_, MakeArrayView(device_ptrs));

  ExpectNoInitialization(mock_camera1_);
  ExpectNoInitialization(mock_observing_conditions0_);

  EXPECT_DEATH(devices.ValidateDevices(), "have the same domain");
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
    mock_camera0_description_.SetUuidForTest(tlv, uuid);
    mock_camera1_description_.SetUuidForTest(tlv, uuid);
  }

  DeviceInterface* device_ptrs[] = {&mock_camera0_, &mock_camera1_};
  AlpacaDevices devices(server_context_, MakeArrayView(device_ptrs));

  ExpectNoInitialization(mock_camera0_);
  ExpectNoInitialization(mock_camera1_);

  EXPECT_DEATH(devices.ValidateDevices(), "have the same UUID");
}

TEST_F(AlpacaDevicesDeathTest, DeviceNumberGap) {
  DeviceDescription mock_camera2_description_ = mock_camera1_description_;
  mock_camera2_description_.device_number = 2;
  AddDefaultBehavior(mock_camera2_description_, mock_camera1_);

  DeviceInterface* device_ptrs[] = {&mock_camera0_, &mock_camera1_};
  AlpacaDevices devices(server_context_, MakeArrayView(device_ptrs));

  // Expect NO initialization.
  ExpectNoInitialization(mock_camera0_);
  ExpectNoInitialization(mock_camera1_);

  EXPECT_DEATH(devices.ValidateDevices(),
               "FindDevice.*without any gap.*Camera#2");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
