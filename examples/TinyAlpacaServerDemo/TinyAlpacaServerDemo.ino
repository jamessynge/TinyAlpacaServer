// This Arduino Sketch demonstrates how to use Tiny Alpaca Server to respond to
// ASCOM Alpaca requests.
//
// TODO(jamessynge): Add more details about how this demo works.
//
// * On first execution mcunet::IpDevice will generate a random MAC address and
//   a default link-local IP address, and store those in EEPROM for later use.
//
// And on each run:
//
// * Uses DHCP to get an IP address, and falls back to using the IP address
//   stored in EEPROM if necessary.
//
// * If assigned an address via DHCP, keeps the DHCP lease alive over time.
//
// * Handles Alpaca Discovery Protocol messages.
//
// * Provides an HTTP server at the specified port (80 in this demo) that
//   decodes ASCOM Alpaca (HTTP) management and device API requests; the server
//   detects malformed and unsupported requests, for which it returns errors.
//
// * Handles /management/* requests using statically provided information about
//   the server and the devices.
//
// * Handles /setup requests with a simple fixed HTML response.
//   TODO(jamessynge): Improve on this.
//
// * Delegates to registered handlers for valid requests of the form:
//
//       /api/v1/{device_type}/{device_number}/{method}
//       /setup/v1/{device_type}
//
// * Provides methods for handlers for sending OK and error responses, including
//   measuring the size of the JSON body without having to allocate memory for
//   it.
//
// Author: james.synge@gmail.com

#include <Arduino.h>
#include <McuCore.h>
#include <McuNet.h>
#include <TinyAlpacaServer.h>

#include "src/dht22_handler.h"

using ::alpaca::DeviceInterface;

// For responding to /management/v1/description.
// Note that this is using C++ 20's designated initializers, which shouldn't be
// available for Arduino, which claims to use C++ 11, but it works.
const alpaca::ServerDescription kServerDescription{
    .server_name =
        MCU_FLASHSTR("Our Spiffy Weather Box, based on Tiny Alpaca Server"),
    .manufacturer = MCU_FLASHSTR("Friends of AAVSO & ATMoB"),
    .manufacturer_version =
        MCU_FLASHSTR("9099c8af5796a80137ce334713a67a718fd0cd3f"),
    .location = MCU_FLASHSTR("Mittleman Observatory, Westford, MA"),
};

static Dht22Handler dht_handler;  // NOLINT

static DeviceInterface* kDevices[] = {&dht_handler};

static constexpr uint16_t kHttpPort = 80;
static mcunet::IpDevice ip_device;
static alpaca::TinyAlpacaDeviceServer device_server(  // NOLINT
    kServerDescription, kDevices);
static alpaca::TinyAlpacaNetworkServer network_server(  // NOLINT
    device_server, kHttpPort);

void announceAddresses() {
  Serial.println();
  mcunet::IpDevice::PrintNetworkAddresses();
  Serial.println();
}

void announceFailure(const char* message) {
  while (true) {
    Serial.println(message);
    delay(1000);
  }
}

void setup() {
  // Setup serial with the fastest baud rate supported by the SoftwareSerial
  // class. Note that the baud rate is meaningful on boards with
  // microcontrollers that do 'true' serial (e.g. Arduino Uno and Mega), while
  // those boards with microcontrollers that have builtin USB (e.g. Arduino
  // Micro) likely don't rate limit because there isn't a need.
  Serial.begin(115200);

  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost. Note that this isn't true for all
  // Arduino-like boards: some reset when the Serial Monitor connects, so we
  // almost always get the initial output. Note though that a software reset
  // such as that may not reset all of the hardware features, leading to hard
  // to diagnose bugs (experience speaking).
  while (!Serial) {
  }

  //////////////////////////////////////////////////////////////////////////////
  // Initialize networking.
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
  mcunet::OuiPrefix oui_prefix(0x53, 0x75, 0x76);
  MCU_CHECK_OK(ip_device.InitializeNetworking(eeprom_tlv, &oui_prefix));
  announceAddresses();

  // Initialize Tiny Alpaca Device Server, which will initialize sensors, etc.
  device_server.Initialize();
  // Initialize Tiny Alpaca Network Server, which will initialize TCP listeners.
  network_server.Initialize();
}

// For now only supporting one request at a time. Unless there are multiple
// clients, and some clients are slow to write requests or read responses,
// this shouldn't be a problem.
// static AlpacaRequest request;

void loop() {
  auto dhcp_check = ip_device.MaintainDhcpLease();
  switch (dhcp_check) {
    case DHCP_CHECK_NONE:
    case DHCP_CHECK_RENEW_OK:
      break;
    case DHCP_CHECK_RENEW_FAIL:
      Serial.println("DHCP_CHECK_RENEW_FAIL: Unable to renew the DHCP lease.");
      delay(1000);
      return;
    case DHCP_CHECK_REBIND_FAIL:
      Serial.println("DHCP_CHECK_REBIND_FAIL: Unable to get a new DHCP lease.");
      delay(1000);
      return;
    case DHCP_CHECK_REBIND_OK:
      announceAddresses();
      break;
    default:
      Serial.print("Unexpected result from MaintainDhcpLease: ");
      Serial.println(dhcp_check);
  }
  device_server.MaintainDevices();
  network_server.PerformIO();
}
