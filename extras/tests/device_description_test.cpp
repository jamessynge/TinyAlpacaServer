#include "device_description.h"

#include <McuCore.h>

#include <string>

#include "constants.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mcucore/extras/test_tools/json_decoder.h"
#include "mcucore/extras/test_tools/json_test_utils.h"
#include "mcucore/extras/test_tools/print_to_std_string.h"
#include "mcucore/extras/test_tools/status_or_test_utils.h"
#include "mcucore/extras/test_tools/uuid_utils.h"

// Defining everything outside of the alpaca namespace as is expected for a real
// device; i.e. we reserve the alpaca namespace for the generic ASCOM Alpaca
// features provided by Tiny Alpaca Server, and use some other namespace (e.g.
// the global namespace, or "my_device") for code specific to a concrete ASCOM
// server.

MCU_DEFINE_CONSTEXPR_DOMAIN(35);

constexpr mcucore::ProgmemString kActionLiterals[] = {MCU_PSD("ActionA"),
                                                      MCU_PSD("Action2")};
constexpr auto kSupportedActions = mcucore::ProgmemStringArray(kActionLiterals);

constexpr alpaca::DeviceDescription kDeviceDescription{
    .device_type = alpaca::EDeviceType::kCamera,
    .device_number = 312,
    .domain = MCU_DOMAIN(35),
    .name = MCU_PSD("AbcDeviceName"),
    .description = MCU_PSD("The Device Desc."),
    .driver_info = MCU_PSD("https://github/jamessynge/TinyAlpacaServer"),
    .driver_version = MCU_PSD("1"),
    .supported_actions = kSupportedActions,
};

namespace alpaca {
namespace test {
namespace {

using ::mcucore::EepromTlv;
using ::mcucore::Uuid;
using ::mcucore::test::IsOkAndHolds;
using ::mcucore::test::JsonValue;
using ::mcucore::test::kUuidRegex;
using ::mcucore::test::MakeUuid;

std::string DeviceDescriptionToJsonText(
    const DeviceDescription& device_description, EepromTlv& tlv) {
  mcucore::test::PrintToStdString out;
  auto property_source_function =
      [&](mcucore::JsonObjectEncoder& object_encoder) {
        device_description.AddConfiguredDeviceTo(object_encoder, tlv);
      };
  mcucore::test::JsonEncodeObject(property_source_function, out);
  return out.str();
}

TEST(DeviceDescriptionTest, ReadStoredUniqueIdRepeatedly) {
  const auto kUuid = MakeUuid({0x46, 0xb4, 0xd9, 0x5c, 0x53, 0x88, 0x43, 0x38,
                               0x91, 0xb9, 0x05, 0x1c, 0x5d, 0xab, 0xc0, 0x9d});

  EepromTlv::ClearAndInitializeEeprom();
  auto tlv = EepromTlv::GetOrDie();
  kDeviceDescription.SetUuidForTest(tlv, kUuid);

  ASSERT_THAT(kDeviceDescription.GetOrCreateUniqueId(tlv), IsOkAndHolds(kUuid));
  ASSERT_THAT(kDeviceDescription.GetOrCreateUniqueId(tlv), IsOkAndHolds(kUuid));
}

TEST(DeviceDescriptionTest, CreateUniqueIdRepeatedly) {
  Uuid uuid1;
  {
    EepromTlv::ClearAndInitializeEeprom();
    auto tlv = EepromTlv::GetOrDie();
    ASSERT_STATUS_OK_AND_ASSIGN(uuid1,
                                kDeviceDescription.GetOrCreateUniqueId(tlv));
    // Read again, it will be unchanged.
    ASSERT_THAT(kDeviceDescription.GetOrCreateUniqueId(tlv),
                IsOkAndHolds(uuid1));
  }

  // If the EEPROM is cleared, then GetOrCreateUniqueId will need to generate a
  // new UUID.

  Uuid uuid2;
  {
    EepromTlv::ClearAndInitializeEeprom();
    auto tlv = EepromTlv::GetOrDie();
    ASSERT_STATUS_OK_AND_ASSIGN(uuid2,
                                kDeviceDescription.GetOrCreateUniqueId(tlv));
    // Read again, it will be unchanged.
    ASSERT_THAT(kDeviceDescription.GetOrCreateUniqueId(tlv),
                IsOkAndHolds(uuid2));
  }

  EXPECT_NE(uuid1, uuid2);
}

TEST(DeviceDescriptionTest, Output) {
  EepromTlv::ClearAndInitializeEeprom();
  auto tlv = EepromTlv::GetOrDie();

  const std::string first_output =
      DeviceDescriptionToJsonText(kDeviceDescription, tlv);
  ASSERT_OK_AND_ASSIGN(auto json_value, JsonValue::Parse(first_output));
  ASSERT_TRUE(json_value.is_object()) << json_value.ToDebugString();
  auto json_object = json_value.as_object();
  EXPECT_EQ(json_object.GetValue("DeviceName"), "AbcDeviceName");
  EXPECT_EQ(json_object.GetValue("DeviceType"), "Camera");
  EXPECT_EQ(json_object.GetValue("DeviceNumber"), 312);

  const std::string first_uuid = json_object.GetValue("UniqueID").as_string();
  EXPECT_THAT(first_uuid, testing::MatchesRegex(kUuidRegex));

  // If we output again, the output should be the same.
  EXPECT_EQ(first_output, DeviceDescriptionToJsonText(kDeviceDescription, tlv));

  // If we clear the EEPROM, a new and different UniqueId should be generated.
  EepromTlv::ClearAndInitializeEeprom();

  const std::string second_output =
      DeviceDescriptionToJsonText(kDeviceDescription, tlv);
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

// I don't expect it to be necessary to copy DeviceDescription instances (unless
// I come up with a way to store them in Flash), but let's make sure that it
// doesn't mess things up.
TEST(DeviceDescriptionTest, CopyCtor) {
  EepromTlv::ClearAndInitializeEeprom();
  auto tlv = EepromTlv::GetOrDie();

  const std::string first_output =
      DeviceDescriptionToJsonText(kDeviceDescription, tlv);
  alpaca::DeviceDescription copy = kDeviceDescription;
  EXPECT_EQ(first_output, DeviceDescriptionToJsonText(copy, tlv));

  // There shouldn't have been a change to the output of the original.
  EXPECT_EQ(first_output, DeviceDescriptionToJsonText(kDeviceDescription, tlv));
}

TEST(DeviceDescriptionTest, ReadsUuid) {
  const auto kUuid = MakeUuid({0x46, 0xb4, 0xd9, 0x5c, 0x53, 0x88, 0x43, 0x38,
                               0x91, 0xb9, 0x05, 0x1c, 0x5d, 0xab, 0xc0, 0x9d});

  EepromTlv::ClearAndInitializeEeprom();
  auto tlv = EepromTlv::GetOrDie();
  kDeviceDescription.SetUuidForTest(tlv, kUuid);

  EXPECT_EQ(DeviceDescriptionToJsonText(kDeviceDescription, tlv),
            R"({"DeviceName": "AbcDeviceName", )"
            R"("DeviceType": "Camera", )"
            R"("DeviceNumber": 312, )"
            R"("UniqueID": "46B4D95C-5388-4338-91B9-051C5DABC09D"})");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
