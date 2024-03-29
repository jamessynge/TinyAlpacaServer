#include "server.h"

#include <Arduino.h>
#include <McuCore.h>
#include <McuNet.h>
#include <TinyAlpacaServer.h>

#include "fake_weather.h"

// Used to identify EepromTlv entries, i.e. for storing data in the EEPROM .
MCU_DEFINE_NAMED_DOMAIN(FakeWeather, 17);

namespace fake_weather_service {
namespace {
using ::alpaca::DeviceDescription;
using ::alpaca::EDeviceType;

// Shared context provided to devices.
alpaca::ServerContext server_context;  // NOLINT

const DeviceDescription kFakeWeatherDeviceDescription{
    .device_type = EDeviceType::kObservingConditions,
    .device_number = 0,
    .domain = MCU_DOMAIN(FakeWeather),
    .name = MCU_FLASHSTR("FakeWeather"),
    .description = MCU_FLASHSTR("Totally Fake Weather, Now and Always"),
    .driver_info = MCU_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
    .driver_version = MCU_FLASHSTR("1.0"),  // Perfection has been achieved.
    .supported_actions = {},                // No extra actions.
};

FakeWeather fake_weather_device(server_context,  // NOLINT
                                kFakeWeatherDeviceDescription);

// For responding to /management/v1/description.
// Note that this is using C++ 20's designated initializers, which shouldn't be
// available for Arduino, which claims to use C++ 11, but it works.
const alpaca::ServerDescription kServerDescription{
    .server_name =
        MCU_FLASHSTR("Fake Weather Station, based on Tiny Alpaca Server"),
    .manufacturer = MCU_FLASHSTR("Camelot Weather Service"),
    .manufacturer_version = MCU_FLASHSTR("1.0"),
    .location = MCU_FLASHSTR("Atlantis"),
};

// The array of devices that this server will have.
alpaca::DeviceInterface* kDevices[] = {&fake_weather_device};

alpaca::TinyAlpacaDeviceServer device_server(  // NOLINT
    server_context, kServerDescription, kDevices);

alpaca::TinyAlpacaNetworkServer network_server(device_server);  // NOLINT

mcunet::IpDevice ip_device;

void announceAddresses() {
  Serial.println();
  mcunet::IpDevice::PrintNetworkAddresses();
  Serial.println();
}

}  // namespace

void setup() {
  // This is first so we can (if necessary) turn off any device that turns on
  // automatically when the microcontroller is reset.
  device_server.ValidateAndReset();

  mcucore::LogSink() << MCU_PSD("\n\n#####################################\n")
                     << kServerDescription.server_name << '\n'
                     << kServerDescription.manufacturer << MCU_PSD(", version ")
                     << kServerDescription.manufacturer_version;

  mcucore::LogSink() << MCU_PSD("\nInitializing networking");
  mcunet::Mega2560Eth::SetupW5500();

  // Initialize the pseudo-random number generator with a random number
  // generated based on clock jitter.
  mcucore::JitterRandom::setRandomSeed();

  // Provide an "Organizationally Unique Identifier" which will be used as the
  // first 3 bytes of the MAC addresses generated; this means that all boards
  // running this sketch will share the first 3 bytes of their MAC addresses,
  // which may help with locating them.
  mcunet::OuiPrefix oui_prefix(0x53, 0x76, 0x67);
  MCU_CHECK_OK(
      ip_device.InitializeNetworking(server_context.eeprom_tlv(), &oui_prefix));
  announceAddresses();

  // Initialize Tiny Alpaca Device Server, which will initialize sensors, etc.
  device_server.InitializeForServing();

  // Initialize Tiny Alpaca Network Server, which will initialize TCP listeners
  // and the Alpaca Discovery Server.
  network_server.Initialize();
}

void loop() {
  ip_device.MaintainDhcpLease();
  device_server.MaintainDevices();
  network_server.PerformIO();
}

}  // namespace fake_weather_service
