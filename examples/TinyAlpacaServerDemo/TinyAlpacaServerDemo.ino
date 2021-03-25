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

#ifdef ARDUINO
#include <Arduino.h>
#include <TinyAlpacaServer.h>

#include "pretend_devices.h"  // NOLINT
#else
#include "TinyAlpacaServer.h"
#include "examples/TinyAlpacaServerDemo/pretend_devices.h"
#endif

using ::alpaca::AlpacaRequest;
using ::alpaca::DeviceApiHandlerBase;
using ::alpaca::EDeviceMethod;
using ::alpaca::Literal;
using ::alpaca::ObservingConditionsAdapter;
using ::alpaca::StatusOr;
using ::alpaca::StringView;

// Just one simple device, used to report Observing Conditions.
static Dht22Device dht22;

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
TAS_DEFINE_LITERAL(ServerName, "Our Spiffy Weather Box");
TAS_DEFINE_LITERAL(Manufacturer, "Friends of AAVSO & ATMoB");
TAS_DEFINE_LITERAL(ManufacturerVersion,
                   "9099c8af5796a80137ce334713a67a718fd0cd3f");
TAS_DEFINE_LITERAL(DHT22Name, "DHT22");
TAS_DEFINE_LITERAL(DHT22Description, "DHT22 Humidity and Temperature Sensor");
TAS_DEFINE_LITERAL(DHT22DriverInfo, "https://github/aavso/...");
TAS_DEFINE_LITERAL(DHT22DriverVersion, "https://github/aavso/...");

// TODO(jamessynge): Add support for storing in EEPROM.
TAS_DEFINE_LITERAL(DeviceLocation, "Mittleman Observatory, Westford, MA");

// For responding to /management/v1/description
const alpaca::ServerDescription kServerDescription(ServerName(), Manufacturer(),
                                                   Manufacturer(),
                                                   DeviceLocation());

// No extra actions.
const auto kSupportedActions = alpaca::LiteralArray({});  // None

const alpaca::DeviceInfo kDht22DeviceInfo{
    .device_type = alpaca::EDeviceType::kObservingConditions,
    .device_number = 1,
    .name = DHT22Name(),
    .description = DHT22Description(),
    .driver_info = DHT22DriverInfo(),
    .driver_version = DHT22DriverVersion(),
    .interface_version = 1,
    .supported_actions = kSupportedActions,

    // The config_id is a random number generated when a device is added,
    // when the *type(s)* of device(s) used changes, or perhaps when
    // calibration parameters have been changed such that the values shouldn't
    // be compared with prior values from this device.
    // The config_id can be used, along with other info, to generate a UUID
    // for the device, for use as its UniqueId.
    .config_id = 179122466,
};

class Dht22Handler : public ObservingConditionsAdapter {
 public:
  Dht22Handler() : ObservingConditionsAdapter(kDht22DeviceInfo) {}

  bool HandleGetRequest(const AlpacaRequest& request, Print& out) override {
    switch (request.device_method) {
      case EDeviceMethod::kHumidity:
        return alpaca::WriteResponse::DoubleResponse(
            request, dht22.get_relative_humidity(), out);

      case EDeviceMethod::kTemperature:
        return alpaca::WriteResponse::DoubleResponse(
            request, dht22.get_temperature(), out);

      default:
        // For common methods, this will delegate to overrideable methods such
        // as HandleGetDescription, and for unsupported methods (e.g.
        // "cloudcover"), and will delegate to methods such
        return ObservingConditionsAdapter::HandleGetRequest(request, out);
    }
  }

  StatusOr<double> GetHumidity() override {
    return dht22.get_relative_humidity();
  }

  StatusOr<double> GetTemperature() override { return dht22.get_temperature(); }

  bool GetConnected() override {
    return true;  // XXX: Return true if able to talk to the device.
  }

  StatusOr<Literal> GetSensorDescription(StringView sensor_name) override {
    if (CaseEqual(sensor_name, alpaca::Literals::humidity()) ||
        CaseEqual(sensor_name, alpaca::Literals::temperature())) {
      return DHT22Description();
    }
    return alpaca::ErrorCodes::InvalidValue();
  }
};

static Dht22Handler dht_handler;  // NOLINT

constexpr DeviceApiHandlerBase* kDeviceHandlers[] = {&dht_handler};
// alpaca::MakeArray<DeviceApiHandlerBase*>(&dht_handler);

DhcpClass dhcp;
static alpaca::IpDevice ip_device;

static alpaca::TinyAlpacaServer tiny_alpaca_server(  // NOLINT
    80, kServerDescription, kDeviceHandlers);

void announceAddresses() {
  Serial.println();
  alpaca::MacAddress mac;
  Ethernet.macAddress(mac.mac);
  Serial.print("MAC: ");
  Serial.println(mac);
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(Ethernet.dnsServerIP());
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
  alpaca::Mega2560Eth::setup_w5500();

  // Provide an "Organizationally Unique Identifier" which will be used as the
  // first 3 bytes of the MAC addresses generated; this means that all boards
  // running this sketch will share the first 3 bytes of their MAC addresses,
  // which may help with locating them.
  alpaca::OuiPrefix oui_prefix(0x53, 0x57, 0x76);
  if (!ip_device.setup(&oui_prefix)) {
    announceFailure("Unable to initialize networking!");
  }
  announceAddresses();

  tiny_alpaca_server.begin();

  //   // Ask alpaca::Server to verify settings.
  //   if (!alpaca_server.VerifyConfig()) {
  //     announceFailure("Unable to verify Tiny Alpaca Server configuration");
  //   }

  //   // Do hardware setup (e.g. init connection to sensors).
  //   // TBD

  //   // The microcontroller may be ready before the network chip (based on
  //   docs and
  //   // experience), so we wait a bit for hardware to be ready.
  //   delay(200);

  //   // Initialize networking. Provide an "Organizationally Unique Identifier"
  //   // that will be the first 3 bytes of the MAC addresses generated; this
  //   means
  //   // that all boards running this sketch will share the first 3 bytes of
  //   their
  //   // MAC addresses, which may help with locating them if other discovery
  //   means
  //   // are failing.
  //   OuiPrefix oui_prefix(0x52, 0xC4, 0x55);  // TODO Choose a value.
  //   if (!alpaca_server.SetupNetworking(&oui_prefix)) {
  //     announceFailure("Unable to initialize networking!");
  //   }
}

// For now only supporting one request at a time. Unless there are multiple
// clients, and some clients are slow to write requests or read responses,
// this shouldn't be a problem.
// static AlpacaRequest request;

void loop() {
  auto dhcp_check = ip_device.maintain_dhcp_lease();
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
      Serial.print("Unexpected result from maintain_dhcp_lease: ");
      Serial.println(dhcp_check);
  }

  tiny_alpaca_server.loop();
}
