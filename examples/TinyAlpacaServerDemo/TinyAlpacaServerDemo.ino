// This Arduino Sketch (which does not yet compile or work) demonstrates the API
// that I aim to provide to folks creating Alpaca compliant devices.
//
// The goal is for the alpaca::Server to support these one-time (or rare)
// operations:
//
// * On first execution, will generate a random MAC address, and store in
//   EEPROM. This is based on my earlier SimpleWebServer.
//
// * Similarly, will generate a UniqueID for each device that does not have
//   one, and store in EEPROM. Some devices, such as a DS18B20, can return a
//   unique identifier, which might be able to use as is, or use as the seed to
//   a pseudo-random-number-generator. Note that from run to run, the set of
//   devices statically configured might change, which we should attempt to deal
//   with in some way.
//
// * If DHCP isn't able to provide an IP address, generate one from a
//   non-routable range, and store in EEPROM so that it can be re-used in the
//   future.
//
// And on each run:
//
// * Use DHCP to get an IP address, falling back to the address stored in EEPROM
//   if necessary.
//
// * If assigned an address via DHCP, keep the DHCP lease alive.
//
// * Handle Alpaca Discovery Protocol messages.
//
// * Accept new HTTP connections, decode the requests, including decoding all
//   Alpaca requests (/setup*, /management/*, /api/v1/*), responding with an
//   error automatically for other paths, and for many kinds of malformed
//   requests (e.g. POST instead of PUT, unknown device type, device number
//   or ASCOM method, etc.).
//
// * Automatically handle /management/* requests using statically provided
//   information about the server and the devices.
//
// * Delegate to registered handlers for valid /api/v1/* requests.
//
// * Provide methods for handlers for sending OK and error responses, including
//   measuring the size of the JSON body without having to allocate memory for
//   it.
//
// The code below doesn't include any indication of how the /setup paths will be
// handled.
//
// Note that there isn't a need for the static configuration data to be valid
// constexprs as we're not storing them in PROGMEM.
//
// Author: james.synge@gmail.com

#include <Arduino.h>
#include <TinyAlpacaServer.h>

#include "dht22_handler.h"

using ::alpaca::DeviceInterface;

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
TAS_DEFINE_LITERAL(ServerName, "Our Spiffy Weather Box");
TAS_DEFINE_LITERAL(Manufacturer, "Friends of AAVSO & ATMoB");
TAS_DEFINE_LITERAL(ManufacturerVersion,
                   "9099c8af5796a80137ce334713a67a718fd0cd3f");

// TODO(jamessynge): Add support for storing in EEPROM.
TAS_DEFINE_LITERAL(DeviceLocation, "Mittleman Observatory, Westford, MA");

// For responding to /management/v1/description
const alpaca::ServerDescription kServerDescription(ServerName(), Manufacturer(),
                                                   Manufacturer(),
                                                   DeviceLocation());

static Dht22Handler dht_handler;  // NOLINT

static DeviceInterface* kDevices[] = {&dht_handler};

static constexpr uint16_t kHttpPort = 80;
static DhcpClass dhcp;
static alpaca::IpDevice ip_device;
static alpaca::TinyAlpacaServer tiny_alpaca_server(  // NOLINT
    kHttpPort, kServerDescription, kDevices);

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

void setup() {
  // Setup serial, wait for it to be ready so that our logging messages can be
  // read.
  Serial.begin(9600);
  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost.
  while (!Serial) {
  }

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
  tiny_alpaca_server.PerformIO();
}
