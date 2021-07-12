#include "server.h"

#include <Arduino.h>
#include <TinyAlpacaServer.h>

#include "am_weather_box.h"

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
TAS_DEFINE_LITERAL(
    ServerName, "AstroMakers Weather Box Server, based on Tiny Alpaca Server");
TAS_DEFINE_LITERAL(Manufacturer, "Friends of AAVSO & ATMoB");
TAS_DEFINE_LITERAL(ManufacturerVersion, "0.1");
TAS_DEFINE_LITERAL(DeviceLocation, "Earth Bound");

namespace astro_makers {
namespace {
using ::alpaca::DeviceInfo;
using ::alpaca::EDeviceType;
using ::alpaca::LiteralArray;

// No extra actions.
const auto kSupportedActions = LiteralArray();

TAS_DEFINE_LITERAL(GithubRepoLink,
                   "https://github/jamessynge/TinyAlpacaServer");
TAS_DEFINE_LITERAL(DriverVersion, "0.1");

TAS_DEFINE_LITERAL(WeatherBoxName, "AM_WeatherBox");
TAS_DEFINE_LITERAL(WeatherBoxDescription, "AstroMakers Weather Box");
TAS_DEFINE_LITERAL(WeatherBoxUniqueId, "7bc0548a-fbc7-4e77-8f66-a0a21854ff48");

const DeviceInfo kAMWeatherBoxDeviceInfo{
    .device_type = EDeviceType::kObservingConditions,
    .device_number = 1,
    .name = WeatherBoxName(),
    .unique_id = WeatherBoxUniqueId(),
    .description = WeatherBoxDescription(),
    .driver_info = GithubRepoLink(),
    .driver_version = DriverVersion(),
    .supported_actions = kSupportedActions,
    .interface_version = 1,
};

AMWeatherBox weather_box(kAMWeatherBoxDeviceInfo);  // NOLINT

constexpr alpaca::ServerDescription kServerDescription{
    .server_name = ServerName(),
    .manufacturer = Manufacturer(),
    .manufacturer_version = ManufacturerVersion(),
    .location = DeviceLocation(),
};

alpaca::DeviceInterface* kDevices[] = {&weather_box};

alpaca::TinyAlpacaServer tiny_alpaca_server(  // NOLINT
    /*tcp_port=*/80, kServerDescription, kDevices);

// We allocate a DhcpClass at file scope so that it isn't dynamically allocated
// at setup time (i.e. so we're in better control of memory consumption).
::DhcpClass dhcp;

alpaca::IpDevice ip_device;

void announceAddresses() {
  Serial.println();
  alpaca::IpDevice::PrintNetworkAddresses();
  Serial.println();
}

}  // namespace

void setup() {
  alpaca::LogSink() << TAS_FLASHSTR("Initializing networking");
  Ethernet.setDhcp(&dhcp);
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
