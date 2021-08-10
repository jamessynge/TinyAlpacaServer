#include "server_description.h"

#include "gtest/gtest.h"
#include "json_encoder.h"
#include "json_encoder_helpers.h"
#include "literal.h"
#include "mcucore/extrastest_tools/print_to_std_string.h"

#define kServerName "HAL 9000"
#define kManufacturer "HAL Laboratories, Urbana, Illinois"
#define kManufacturerVersion "9000.0"
#define kDeviceLocation "Jupiter Orbit"

// For responding to /management/v1/description
const alpaca::ServerDescription kServerDescription{
    .server_name = TAS_FLASHSTR(kServerName),
    .manufacturer = TAS_FLASHSTR(kManufacturer),
    .manufacturer_version = TAS_FLASHSTR(kManufacturerVersion),
    .location = TAS_FLASHSTR(kDeviceLocation),
};

namespace alpaca {
namespace test {
namespace {

TEST(ServerDescriptionTest, Output) {
  mcucore::test::PrintToStdString out;
  mcucore::JsonPropertySourceAdapter<ServerDescription> adapter(
      kServerDescription);
  mcucore::JsonObjectEncoder::Encode(adapter, out);
  EXPECT_EQ(out.str(),
            R"({"ServerName": "HAL 9000", )"
            R"("Manufacturer": "HAL Laboratories, Urbana, Illinois", )"
            R"("ManufacturerVersion": "9000.0", )"
            R"("Location": "Jupiter Orbit"})");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
