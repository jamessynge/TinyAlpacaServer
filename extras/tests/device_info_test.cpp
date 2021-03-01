#include "device_info.h"

#include "extras/tests/test_utils.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
TAS_DEFINE_LITERAL(Dht22DeviceName, "DHT22");
TAS_DEFINE_LITERAL(Dht22Description, "");
TAS_DEFINE_LITERAL(ManufacturerVersion,
                   "9099c8af5796a80137ce334713a67a718fd0cd3f");
TAS_DEFINE_LITERAL(DeviceLocation, "Mittleman Observatory");

// // For responding to /management/v1/description
// constexpr alpaca::ServerDescription kServerDescription(ServerName(),
//                                                        Manufacturer(),
//                                                        ManufacturerVersion(),
//                                                        DeviceLocation());
// namespace alpaca {
// namespace {

// TEST(ServerDescriptionTest, Output) {
//   PrintToString out;
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
