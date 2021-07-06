#include "device_info.h"

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "utils/json_encoder_helpers.h"

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
    .name = DeviceName(),
    .unique_id = DeviceUniqueId(),
    .description = DeviceDescription(),
    .driver_info = GithubRepoLink(),
    .driver_version = DriverVersion(),
    .supported_actions = kSupportedActions,
    .interface_version = 1,
};

namespace alpaca {
namespace test {
namespace {

TEST(DeviceInfoTest, Output) {
  PrintToStdString out;
  JsonPropertySourceAdapter<DeviceInfo> adapter(kDeviceInfo);
  JsonObjectEncoder::Encode(adapter, out);
  EXPECT_EQ(out.str(), R"({"Name": "AbcDeviceName", )"
                       R"("DeviceType": "Camera", )"
                       R"("DeviceNumber": 312, )"
                       R"("UniqueID": "123456"})");
}

}  // namespace
}  // namespace test
}  // namespace alpaca

// // Define some literals, which get stored in PROGMEM (in the case of AVR
// chips). TAS_DEFINE_LITERAL(Dht22DeviceName, "DHT22");
// TAS_DEFINE_LITERAL(Dht22Description, "");
// TAS_DEFINE_LITERAL(ManufacturerVersion,
//                    "9099c8af5796a80137ce334713a67a718fd0cd3f");
// TAS_DEFINE_LITERAL(DeviceLocation, "Mittleman Observatory");

// // For responding to /management/v1/description
// constexpr alpaca::ServerDescription kServerDescription(ServerName(),
//                                                        Manufacturer(),
//                                                        ManufacturerVersion(),
//                                                        DeviceLocation());
// namespace alpaca { namespace test {
// namespace {

// TEST(ServerDescriptionTest, Output) {
//   PrintToStdString out;
//   JsonPropertySourceAdapter<ServerDescription> adapter(kServerDescription);
//   JsonObjectEncoder::Encode(adapter, out);
//   EXPECT_EQ(
//       out.str(),
//       R"({"ServerName": "Our Spiffy Weather Box", )"
//       R"("Manufacturer": "Friends of AAVSO & ATMoB", )"
//       R"("ManufacturerVersion": "9099c8af5796a80137ce334713a67a718fd0cd3f",
//       )" R"("Location": "Mittleman Observatory"})");
// }

// constexpr alpaca::DeviceInfo kDht22DeviceInfo{
//     .device_type = EDeviceType::kObservingConditions,
//     .device_number = 1,
//     .name = "DHT22",
//     .description = "DHT22 Humidity and Temperature Sensor",
//     .driverinfo = "https://github/aavso/...",
//     .driverversion = "2021-05-10 OR git commit SHA=abcdef0123456 OR ?",
//     .interfaceversion = 1,
//     .supported_actions = kObservingConditionsActions,

//     // The config_id is a random number generated when a device is added,
//     // when the *type(s)* of device(s) used changes, or perhaps when
//     // calibration parameters have been changed such that the values
//     shouldn't
//     // be compared with prior values from this device.
//     // The config_id can be used, along with other info, to generate a UUID
//     // for the device, for use as its UniqueId.
//     .config_id = 179122466,
// };

// }  // namespace
// }  // alpaca
