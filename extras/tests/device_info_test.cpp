#include "device_info.h"

#include "constants.h"
#include "gtest/gtest.h"
#include "inline_literal.h"
#include "mcucore/extrastest_tools/print_to_std_string.h"
#include "utils/json_encoder.h"
#include "utils/json_encoder_helpers.h"
#include "utils/literal.h"

// Defining everything outside of the alpaca namespace as is expected for a real
// device.

const alpaca::Literal kActionLiterals[] = {TASLIT("ActionA"),
                                           TASLIT("Action2")};
const auto kSupportedActions = alpaca::LiteralArray(kActionLiterals);

TAS_DEFINE_LITERAL(DeviceName, "AbcDeviceName");
TAS_DEFINE_LITERAL(DeviceDescription, "The Device Desc.");
TAS_DEFINE_LITERAL(DeviceUniqueId, "123456");
TAS_DEFINE_LITERAL(GithubRepoLink,
                   "https://github/jamessynge/TinyAlpacaServer");
TAS_DEFINE_LITERAL(DriverVersion, "1");

const alpaca::DeviceInfo kDeviceInfo{
    .device_type = alpaca::EDeviceType::kCamera,
    .device_number = 312,
    .name = TAS_FLASHSTR("AbcDeviceName"),
    .unique_id = TAS_FLASHSTR("123456"),
    .description = TAS_FLASHSTR("The Device Desc."),
    .driver_info = TAS_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
    .driver_version = TAS_FLASHSTR("1"),
    .supported_actions = kSupportedActions,
    .interface_version = 1,
};

namespace alpaca {
namespace test {
namespace {

TEST(DeviceInfoTest, Output) {
  mcucore::test::PrintToStdString out;
  JsonPropertySourceAdapter<DeviceInfo> adapter(kDeviceInfo);
  JsonObjectEncoder::Encode(adapter, out);
  EXPECT_EQ(out.str(), R"({"DeviceName": "AbcDeviceName", )"
                       R"("DeviceType": "Camera", )"
                       R"("DeviceNumber": 312, )"
                       R"("UniqueID": "123456"})");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
