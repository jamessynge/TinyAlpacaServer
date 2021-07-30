#include "server.h"

#include <Arduino.h>
#include <TinyAlpacaServer.h>

#include "am_weather_box.h"

namespace astro_makers {
namespace {
using ::alpaca::DeviceInfo;
using ::alpaca::EDeviceType;
using ::alpaca::LiteralArray;

// No extra actions.
const auto kSupportedActions = LiteralArray();

const DeviceInfo kAMWeatherBoxDeviceInfo{
    .device_type = EDeviceType::kObservingConditions,
    .device_number = 1,
    .name = TAS_FLASHSTR("AM_WeatherBox"),
    // TODO(jamessynge): Replace this UUID with a value that feeds into a UUID
    // generator, along with the MAC address, and EDeviceType of the device.
    .unique_id = TAS_FLASHSTR("7bc0548a-fbc7-4e77-8f66-a0a21854ff48"),
    .description = TAS_FLASHSTR("AstroMakers Weather Box"),
    .driver_info = TAS_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
    .driver_version = TAS_FLASHSTR("0.1"),
    .supported_actions = kSupportedActions,
    .interface_version = 1,
};

AMWeatherBox weather_box(kAMWeatherBoxDeviceInfo);  // NOLINT

// For responding to /management/v1/description
const alpaca::ServerDescription kServerDescription{
    .server_name = TAS_FLASHSTR(
        "AstroMakers Weather Box Server, based on Tiny Alpaca Server"),
    .manufacturer = TAS_FLASHSTR("Friends of AAVSO & ATMoB"),
    .manufacturer_version = TAS_FLASHSTR("0.1"),
    .location = TAS_FLASHSTR("Earth Bound"),
};

alpaca::DeviceInterface* kDevices[] = {&weather_box};

alpaca::TinyAlpacaServer tiny_alpaca_server(  // NOLINT
    /*tcp_port=*/80, kServerDescription, kDevices);

alpaca::IpDevice ip_device;

void announceAddresses() {
  Serial.println();
  alpaca::IpDevice::PrintNetworkAddresses();
  Serial.println();
}

}  // namespace

void setup() {
  alpaca::LogSink() << kServerDescription.server_name;
  alpaca::LogSink() << TAS_FLASHSTR("Initializing networking");
  alpaca::Mega2560Eth::SetupW5500();

  // Provide an "Organizationally Unique Identifier" which will be used as the
  // first 3 bytes of the MAC addresses generated; this means that all boards
  // running this sketch will share the first 3 bytes of their MAC addresses,
  // which may help with locating them.
  alpaca::OuiPrefix oui_prefix(0x53, 0x75, 0x76);
  TAS_CHECK(ip_device.InitializeNetworking(&oui_prefix))
      << TAS_FLASHSTR("Unable to initialize networking!");
  announceAddresses();

  // Initialize Tiny Alpaca Server, which will initialize TCP listeners.
  tiny_alpaca_server.Initialize();

  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(nullptr): ") << sizeof(nullptr);
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(char*): ") << sizeof(char*);
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(short): ") << sizeof(short);  // NOLINT
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(int): ") << sizeof(int);
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(long): ") << sizeof(long);  // NOLINT
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(float): ") << sizeof(float);
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(double): ") << sizeof(double);
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(&setup): ") << sizeof(&setup);
}

void loop() {
  ip_device.MaintainDhcpLease();
  tiny_alpaca_server.PerformIO();
}

}  // namespace astro_makers
