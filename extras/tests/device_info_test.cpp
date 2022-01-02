#include "device_info.h"

#include "constants.h"
#include "gtest/gtest.h"
#include "json_encoder.h"
#include "json_encoder_helpers.h"
#include "mcucore/extrastest_tools/print_to_std_string.h"
#include "progmem_string_data.h"

// Defining everything outside of the alpaca namespace as is expected for a real
// device; i.e. we reserve the alpaca namespace for the generic ASCOM Alpaca
// features provided by Tiny Alpaca Server, and use some other namespace (e.g.
// the global namespace, or "my_device") for code specific to a concrete ASCOM
// server.

constexpr mcucore::ProgmemString kActionLiterals[] = {MCU_PSD("ActionA"),
                                                      MCU_PSD("Action2")};
constexpr auto kSupportedActions = mcucore::ProgmemStringArray(kActionLiterals);

constexpr alpaca::DeviceInfo kDeviceInfo{
    .device_type = alpaca::EDeviceType::kCamera,
    .device_number = 312,
    .name = MCU_PSD("AbcDeviceName"),
    .unique_id = MCU_PSD("123456"),
    .description = MCU_PSD("The Device Desc."),
    .driver_info = MCU_PSD("https://github/jamessynge/TinyAlpacaServer"),
    .driver_version = MCU_PSD("1"),
    .supported_actions = kSupportedActions,
    .interface_version = 1,
};

namespace alpaca {
namespace test {
namespace {

TEST(DeviceInfoTest, Output) {
  mcucore::test::PrintToStdString out;
  mcucore::JsonPropertySourceAdapter<DeviceInfo> adapter(kDeviceInfo);
  mcucore::JsonObjectEncoder::Encode(adapter, out);
  EXPECT_EQ(out.str(), R"({"DeviceName": "AbcDeviceName", )"
                       R"("DeviceType": "Camera", )"
                       R"("DeviceNumber": 312, )"
                       R"("UniqueID": "123456"})");
}

TEST(DeviceInfoTest, CopyCtor) {
  // I don't expect it to be necessary to copy DeviceInfo instances, but...

  alpaca::DeviceInfo copy = kDeviceInfo;

  mcucore::test::PrintToStdString out;
  mcucore::JsonPropertySourceAdapter<DeviceInfo> adapter(copy);
  mcucore::JsonObjectEncoder::Encode(adapter, out);
  EXPECT_EQ(out.str(), R"({"DeviceName": "AbcDeviceName", )"
                       R"("DeviceType": "Camera", )"
                       R"("DeviceNumber": 312, )"
                       R"("UniqueID": "123456"})");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
