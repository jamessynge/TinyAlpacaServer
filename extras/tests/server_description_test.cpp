#include "server_description.h"

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "utils/json_encoder_helpers.h"

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
TAS_DEFINE_LITERAL(ServerName, "HAL 9000");
TAS_DEFINE_LITERAL(Manufacturer, "HAL Laboratories, Urbana, Illinois");
TAS_DEFINE_LITERAL(ManufacturerVersion, "9000.0");
TAS_DEFINE_LITERAL(DeviceLocation, "Jupiter Orbit");

// For responding to /management/v1/description
constexpr alpaca::ServerDescription kServerDescription{
    .server_name = ServerName(),
    .manufacturer = Manufacturer(),
    .manufacturer_version = ManufacturerVersion(),
    .location = DeviceLocation(),
};

namespace alpaca {
namespace test {
namespace {

TEST(ServerDescriptionTest, Output) {
  PrintToStdString out;
  JsonPropertySourceAdapter<ServerDescription> adapter(kServerDescription);
  JsonObjectEncoder::Encode(adapter, out);
  EXPECT_EQ(out.str(),
            R"({"ServerName": "HAL 9000", )"
            R"("Manufacturer": "HAL Laboratories, Urbana, Illinois", )"
            R"("ManufacturerVersion": "9000.0", )"
            R"("Location": "Jupiter Orbit"})");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
