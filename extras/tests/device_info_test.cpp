#include "device_info.h"

#include "constants.h"
#include "gtest/gtest.h"
#include "inline_literal.h"
#include "json_encoder.h"
#include "json_encoder_helpers.h"
#include "literal.h"
#include "mcucore/extrastest_tools/print_to_std_string.h"
#include "progmem_string_data.h"

// Defining everything outside of the alpaca namespace as is expected for a real
// device.

const mcucore::Literal kActionLiterals[] = {MCU_PSV("ActionA"),
                                            MCU_PSV("Action2")};
const auto kSupportedActions = mcucore::LiteralArray(kActionLiterals);

TAS_DEFINE_LITERAL(DeviceName, "AbcDeviceName");
TAS_DEFINE_LITERAL(DeviceDescription, "The Device Desc.");
TAS_DEFINE_LITERAL(DeviceUniqueId, "123456");
TAS_DEFINE_LITERAL(GithubRepoLink,
                   "https://github/jamessynge/TinyAlpacaServer");
TAS_DEFINE_LITERAL(DriverVersion, "1");

const alpaca::DeviceInfo kDeviceInfo{
    .device_type = alpaca::EDeviceType::kCamera,
    .device_number = 312,
    .name = MCU_FLASHSTR("AbcDeviceName"),
    .unique_id = MCU_FLASHSTR("123456"),
    .description = MCU_FLASHSTR("The Device Desc."),
    .driver_info = MCU_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
    .driver_version = MCU_FLASHSTR("1"),
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

}  // namespace
}  // namespace test
}  // namespace alpaca
