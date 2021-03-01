#include "server_description.h"

#include "extras/tests/test_utils.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
TAS_DEFINE_LITERAL(ServerName, "Our Spiffy Weather Box");
TAS_DEFINE_LITERAL(Manufacturer, "Friends of AAVSO & ATMoB");
TAS_DEFINE_LITERAL(ManufacturerVersion,
                   "9099c8af5796a80137ce334713a67a718fd0cd3f");
TAS_DEFINE_LITERAL(DeviceLocation, "Mittleman Observatory");

// For responding to /management/v1/description
constexpr alpaca::ServerDescription kServerDescription(ServerName(),
                                                       Manufacturer(),
                                                       ManufacturerVersion(),
                                                       DeviceLocation());
namespace alpaca {
namespace {

TEST(ServerDescriptionTest, Output) {
  PrintToString out;
  JsonPropertySourceAdapter<ServerDescription> adapter(kServerDescription);
  JsonObjectEncoder::Encode(adapter, out);
  EXPECT_EQ(
      out.str(),
      R"({"ServerName": "Our Spiffy Weather Box", )"
      R"("Manufacturer": "Friends of AAVSO & ATMoB", )"
      R"("ManufacturerVersion": "9099c8af5796a80137ce334713a67a718fd0cd3f", )"
      R"("Location": "Mittleman Observatory"})");
}

}  // namespace
}  // namespace alpaca
