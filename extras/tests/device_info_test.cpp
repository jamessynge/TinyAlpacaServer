#include "device_info.h"

#include <McuCore.h>

#include <string>

#include "constants.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mcucore/extras/test_tools/json_decoder.h"
#include "mcucore/extras/test_tools/print_to_std_string.h"

// Defining everything outside of the alpaca namespace as is expected for a real
// device; i.e. we reserve the alpaca namespace for the generic ASCOM Alpaca
// features provided by Tiny Alpaca Server, and use some other namespace (e.g.
// the global namespace, or "my_device") for code specific to a concrete ASCOM
// server.

MCU_DEFINE_CONSTEXPR_DOMAIN(35);

constexpr mcucore::ProgmemString kActionLiterals[] = {MCU_PSD("ActionA"),
                                                      MCU_PSD("Action2")};
constexpr auto kSupportedActions = mcucore::ProgmemStringArray(kActionLiterals);

constexpr alpaca::DeviceInfo kDeviceInfo{
    .device_type = alpaca::EDeviceType::kCamera,
    .device_number = 312,
    .domain = MCU_DOMAIN(35),
    .name = MCU_PSD("AbcDeviceName"),
    .description = MCU_PSD("The Device Desc."),
    .driver_info = MCU_PSD("https://github/jamessynge/TinyAlpacaServer"),
    .driver_version = MCU_PSD("1"),
    .supported_actions = kSupportedActions,
    .interface_version = 1,
};

namespace alpaca {
namespace test {
namespace {
using ::mcucore::test::JsonValue;

std::string DeviceInfoToJsonText(const DeviceInfo& device_info) {
  mcucore::test::PrintToStdString out;
  mcucore::JsonPropertySourceAdapter<DeviceInfo> adapter(kDeviceInfo);
  mcucore::JsonObjectEncoder::Encode(adapter, out);
  return out.str();
}

TEST(DeviceInfoTest, Output) {
  mcucore::EepromTlv::ClearAndInitializeEeprom();

  const char kUuidRegex[] =
      R"re([0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-)re"
      R"re([0-9A-Fa-f]{4}-[0-9A-Fa-f]{12})re";

  const std::string first_output = DeviceInfoToJsonText(kDeviceInfo);
  ASSERT_OK_AND_ASSIGN(auto json_value, JsonValue::Parse(first_output));
  ASSERT_TRUE(json_value.is_object()) << json_value.ToDebugString();
  auto json_object = json_value.as_object();
  EXPECT_EQ(json_object.GetValue("DeviceName"), "AbcDeviceName");
  EXPECT_EQ(json_object.GetValue("DeviceType"), "Camera");
  EXPECT_EQ(json_object.GetValue("DeviceNumber"), 312);

  const std::string first_uuid = json_object.GetValue("UniqueID").as_string();
  EXPECT_THAT(first_uuid, testing::MatchesRegex(kUuidRegex));

  // If we output again, the output should be the same.
  EXPECT_EQ(first_output, DeviceInfoToJsonText(kDeviceInfo));

  // If we clear the EEPROM, a new and different UniqueId should be generated.
  mcucore::EepromTlv::ClearAndInitializeEeprom();

  const std::string second_output = DeviceInfoToJsonText(kDeviceInfo);
  ASSERT_OK_AND_ASSIGN(auto json_value2, JsonValue::Parse(second_output));
  ASSERT_TRUE(json_value2.is_object()) << json_value2.ToDebugString();
  auto json_object2 = json_value2.as_object();
  EXPECT_EQ(json_object2.GetValue("DeviceName"), "AbcDeviceName");
  EXPECT_EQ(json_object2.GetValue("DeviceType"), "Camera");
  EXPECT_EQ(json_object2.GetValue("DeviceNumber"), 312);

  const std::string second_uuid = json_object2.GetValue("UniqueID").as_string();
  EXPECT_NE(second_uuid, first_uuid);
  EXPECT_THAT(second_uuid, testing::MatchesRegex(kUuidRegex));
}

// I don't expect it to be necessary to copy DeviceInfo instances, but let's
// make sure that it doesn't mess things up.
TEST(DeviceInfoTest, CopyCtor) {
  mcucore::EepromTlv::ClearAndInitializeEeprom();
  const std::string first_output = DeviceInfoToJsonText(kDeviceInfo);
  alpaca::DeviceInfo copy = kDeviceInfo;
  EXPECT_EQ(first_output, DeviceInfoToJsonText(copy));

  // There shouldn't have been a change to the output of the original.
  EXPECT_EQ(first_output, DeviceInfoToJsonText(kDeviceInfo));
}

TEST(DeviceInfoTest, ReadsUuid) {
  mcucore::EepromTlv::ClearAndInitializeEeprom();
  {
    constexpr uint8_t bytes[16] = {
        0x46, 0xb4, 0xd9, 0x5c, 0x53, 0x88, 0x43, 0x38,
        0x91, 0xb9, 0x05, 0x1c, 0x5d, 0xab, 0xc0, 0x9d,
    };
    mcucore::Uuid uuid;
    uuid.SetForTest(bytes);
    auto tlv = mcucore::EepromTlv::GetOrDie();
    kDeviceInfo.SetUuidForTest(tlv, uuid);
  }
  EXPECT_EQ(DeviceInfoToJsonText(kDeviceInfo),
            R"({"DeviceName": "AbcDeviceName", )"
            R"("DeviceType": "Camera", )"
            R"("DeviceNumber": 312, )"
            R"("UniqueID": "46B4D95C-5388-4338-91B9-051C5DABC09D"})");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
