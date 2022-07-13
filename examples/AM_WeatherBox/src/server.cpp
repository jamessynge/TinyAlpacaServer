#include "server.h"

#include <Arduino.h>
#include <McuCore.h>
#include <McuNet.h>
#include <TinyAlpacaServer.h>

#include "am_weather_box.h"

MCU_DEFINE_CONSTEXPR_NAMED_DOMAIN(ObservingConditions, 70);

namespace astro_makers {
namespace {
using ::alpaca::DeviceInfo;
using ::alpaca::EDeviceType;

const DeviceInfo kAMWeatherBoxDeviceInfo{
    .device_type = EDeviceType::kObservingConditions,
    .device_number = 0,
    .domain = MCU_DOMAIN(ObservingConditions),
    .name = MCU_FLASHSTR("AM_WeatherBox"),
    .description = MCU_FLASHSTR("AstroMakers Weather Box"),
    .driver_info = MCU_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
    .driver_version = MCU_FLASHSTR("0.2"),
    .supported_actions = {},  // No extra actions.
    .interface_version = 1,
};

AMWeatherBox weather_box(kAMWeatherBoxDeviceInfo);  // NOLINT

// For responding to /management/v1/description
const alpaca::ServerDescription kServerDescription{
    .server_name = MCU_FLASHSTR(
        "AstroMakers Weather Box Server, based on Tiny Alpaca Server"),
    .manufacturer = MCU_FLASHSTR("Friends of AAVSO & ATMoB, 2022-05-29"),
    .manufacturer_version = MCU_FLASHSTR("0.2"),
    .location = MCU_FLASHSTR("Earth Bound"),
};

alpaca::DeviceInterface* kDevices[] = {&weather_box};

alpaca::TinyAlpacaDeviceServer device_server(  // NOLINT
    kServerDescription, kDevices);

alpaca::TinyAlpacaNetworkServer network_server(device_server);  // NOLINT

mcunet::IpDevice ip_device;

void announceAddresses() {
  Serial.println();
  mcunet::IpDevice::PrintNetworkAddresses();
  Serial.println();
}

}  // namespace

void setup() {
  // Initialize Tiny Alpaca Device Server, which will initialize sensors, etc.
  // This is first so that pins are initialized promptly.
  device_server.Initialize();

  mcucore::LogSink() << '\n' << kServerDescription.server_name;
  mcucore::LogSink() << kServerDescription.manufacturer
                     << MCU_FLASHSTR(", version ")
                     << kServerDescription.manufacturer_version;

  mcucore::LogSink() << MCU_FLASHSTR("Initializing networking");
  mcunet::Mega2560Eth::SetupW5500();

  // Get an EepromTlv instance, to be used for persistence of settings.
  auto eeprom_tlv = mcucore::EepromTlv::GetOrDie();

  // Initialize the pseudo-random number generator with a random number
  // generated based on clock jitter.
  mcucore::JitterRandom::setRandomSeed();

  // Provide an "Organizationally Unique Identifier" which will be used as the
  // first 3 bytes of the MAC addresses generated; this means that all boards
  // running this sketch will share the first 3 bytes of their MAC addresses,
  // which may help with locating them.
  mcunet::OuiPrefix oui_prefix(0x53, 0x76, 0x77);
  MCU_CHECK_OK(ip_device.InitializeNetworking(eeprom_tlv, &oui_prefix));
  announceAddresses();

  // Initialize Tiny Alpaca Network Server, which will initialize TCP listeners.
  network_server.Initialize();
}

void loop() {
  ip_device.MaintainDhcpLease();
  device_server.MaintainDevices();
  network_server.PerformIO();
}

}  // namespace astro_makers
