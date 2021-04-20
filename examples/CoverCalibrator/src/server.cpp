#include "src/server.h"

#include <Arduino.h>
#include <TinyAlpacaServer.h>

#include "src/cover_calibrator.h"

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
TAS_DEFINE_LITERAL(
    ServerName,
    "AstroMakers Cover Calibrator Server, based on Tiny Alpaca Server");
TAS_DEFINE_LITERAL(Manufacturer, "Friends of AAVSO & ATMoB");
TAS_DEFINE_LITERAL(ManufacturerVersion, "0.1");
TAS_DEFINE_LITERAL(DeviceLocation, "Earth Bound");

namespace astro_makers {
namespace {

constexpr alpaca::ServerDescription kServerDescription{
    .server_name = ServerName(),
    .manufacturer = Manufacturer(),
    .manufacturer_version = ManufacturerVersion(),
    .location = DeviceLocation(),
};

CoverCalibrator cover_calibrator;  // NOLINT

alpaca::DeviceInterface* kDevices[] = {&cover_calibrator};

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

void announceFailure(const char* message) {
  while (true) {
    Serial.println(message);
    delay(1000);
  }
}

}  // namespace

void setup() {
  //////////////////////////////////////////////////////////////////////////////
  // Initialize networking.
  Ethernet.setDhcp(&dhcp);
  alpaca::Mega2560Eth::SetupW5500();

  // Provide an "Organizationally Unique Identifier" which will be used as the
  // first 3 bytes of the MAC addresses generated; this means that all boards
  // running this sketch will share the first 3 bytes of their MAC addresses,
  // which may help with locating them.
  alpaca::OuiPrefix oui_prefix(0x53, 0x75, 0x76);
  if (!ip_device.InitializeNetworking(&oui_prefix)) {
    announceFailure("Unable to initialize networking!");
  }
  announceAddresses();
  tiny_alpaca_server.Initialize();
}

void loop() {
  ip_device.MaintainDhcpLease();
  tiny_alpaca_server.PerformIO();
}

}  // namespace astro_makers
