// This (non-compiling) Arduino Sketch demonstrates the API that I aim to
// provide to folks creating Alpaca compliant devices.
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

#include "literal_token.h"
#ifdef ARDUINO
#include <Arduino.h>
#include <TinyAlpacaServer.h>
#else
#include "TinyAlpacaServer.h"
#include "extras/futures/pretend_devices.h"
#endif

using ::alpaca::AlpacaRequest;
using ::alpaca::DeviceApiHandlerBase;
using ::alpaca::Literal;
using ::alpaca::StringView;

// Two devices supported by the software. This sketch assumes that they can fail
// independently, so are represented as two Observing Conditions devices.
static Dht22Device dht22;
static AagDevice aag;

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
TAS_DEFINE_LITERAL(ServerName, "Our Spiffy Weather Box");
TAS_DEFINE_LITERAL(Manufacturer, "Friends of AAVSO & ATMoB");
TAS_DEFINE_LITERAL(ManufacturerVersion,
                   "9099c8af5796a80137ce334713a67a718fd0cd3f");
TAS_DEFINE_LITERAL(DHT22Name, "DHT22");
TAS_DEFINE_LITERAL(DHT22Description, "DHT22 Humidity and Temperature Sensor");
TAS_DEFINE_LITERAL(DHT22DriverInfo, "https://github/aavso/...");

// TODO(jamessynge): Add support for storing in EEPROM.
TAS_DEFINE_LITERAL(DeviceLocation, "Mittleman Observatory");

// For responding to /management/v1/description
const alpaca::ServerDescription kServerDescription(ServerName(), Manufacturer(),
                                                   Manufacturer(),
                                                   DeviceLocation());

// Neither sensor supports extra actions (maybe not true), so they can
// share the list of no action names.
const auto kSupportedActions = alpaca::LiteralArray({});  // None

const alpaca::DeviceInfo kDht22DeviceInfo{
    .device_type = alpaca::EDeviceType::kObservingConditions,
    .device_number = 1,
    .name = DHT22Name(),
    .description = DHT22Description(),
    .driverinfo = DHT22DriverInfo(),
    .driverversion = "2021-05-10 OR git commit SHA=abcdef0123456 OR ?",
    .interfaceversion = 1,
    .supported_actions = kSupportedActions,

    // The config_id is a random number generated when a device is added,
    // when the *type(s)* of device(s) used changes, or perhaps when
    // calibration parameters have been changed such that the values shouldn't
    // be compared with prior values from this device.
    // The config_id can be used, along with other info, to generate a UUID
    // for the device, for use as its UniqueId.
    .config_id = 179122466,
};

class Dht22Handler : DeviceApiHandlerBase {
 public:
  Dht22Handler() : DeviceApiHandlerBase(kDht22DeviceInfo) {}

  // Fill buffer with up to buffer_size unique bytes from the hardware, return
  // the number of bytes copied. Return 0 (or don't override) if the hardware
  // can not provide this.
  // Ideally, we'd provide a /setup page allowing the UniqueId generated for
  // a device to be cleared, i.e. when we have had to replace a sensor with
  // another of the same type. Perhaps we should do this via the "actions"
  // ASCOM method?
  size_t GetUniqueBytes(uint8_t* buffer, size_t buffer_size) override {
    return 0;
  }

  void HandleGetRequest(const AlpacaRequest& request, Print& out) override {
    switch (request.device_method) {
      case EDeviceMethod::kHumidity:
        return SendJsonDoubleResponse(dht22.get_relative_humidity());

      case EDeviceMethod::kTemperature:
        return SendJsonDoubleResponse(dht22.get_temperature());

      case EDeviceMethod::kSensorDescription:
        return HandleSensorDescriptionRequest(request, out);

      default:
        // For common methods, this will delegate to overrideable methods such
        // as HandleGetDescription, and for unsupported methods (e.g.
        // "cloudcover"), and will delegate to methods such
        return CommonDeviceHandler::HandleGetRequest(request, out);
    }
  }

  bool GetConnected() override {
    return ...;  // Return true if able to talk to the device.
  }

  bool SetConnected(bool value) override {
    // do something...
    return value == GetConnected();
  }

 private:
  // Non-virtual support methods.

  void HandleSensorDescriptionRequest(const AlpacaRequest& request,
                                      Print& out) {
    if (!request.extra_parameters.contains(EParameter::kSensorName)) {
      return SendMissingParameter(EParameter::kSensorName);
    }
    StringView sensor_name =
        request.extra_parameters.find(EParameter::kSensorName);
    if (sensor_name.case_equals(alpaca::Literals::kHumidity) ||
        sensor_name.case_equals(alpaca::Literals::kTemperature)) {
      return SendJsonStringResponse("DHT22 Temperature-Humidity Sensor");
    }
    return SendInvalidParameterValue(request, out, EParameter::kSensorName,
                                     sensor_name);
  }
};

constexpr alpaca::DeviceInfo kAagDeviceInfo{
    .device_type = EDeviceType::kObservingConditions,
    .device_number = 2,
    .name = "AAG CloudWatcher",
    .description = "Lunatico Astro AAG CloudWatcher",
    .driverinfo = "https://github/aavso/...",
    .driverversion =
        "2021-05-10, git commit SHA=abcdef0123456".interfaceversion = 1,
    .supported_actions = kObservingConditionsActions,
};

class AagCloudWatcherHandler : CommonDeviceHandler {
 public:
  AagCloudWatcherHandler()
      : CommonDeviceHandler(kObservingConditionsCommonInfo) {}

  void HandleGetRequest(const AlpacaRequest& request, Print& out) override {
    switch (request.device_method) {
      case EDeviceMethod::kHumidity:
        return SendJsonDoubleResponse(aag.get_relative_humidity());

      case EDeviceMethod::kRainRate:
        return SendJsonDoubleResponse(aag.get_rain_rate());

      case EDeviceMethod::kSkyBrightness:
        return SendJsonDoubleResponse(aag.get_sky_brightness());

      case EDeviceMethod::kSkyTemperature:
        return SendJsonDoubleResponse(aag.get_sky_temp());

      case EDeviceMethod::kTemperature:
        return SendJsonDoubleResponse(aag.get_ambient_temperature());

      case EDeviceMethod::kSensorDescription:
        return HandleSensorDescriptionRequest(request, out);

      default:
        // For common methods, this will delegate to overrideable methods such
        // as HandleGetDescription, and for unsupported methods (e.g.
        // "cloudcover"), and will delegate to methods such
        return CommonDeviceHandler::HandleGetRequest(request, out);
    };
  }

  bool GetConnected() override {
    return ...;  // Return true if able to talk to the device.
  }

  bool SetConnected(bool value) override {
    // do something, return a value.
    return value == GetConnected();
  }

  void HandlePutRequest(const AlpacaRequest& request, Print& out) {
    // do something, issue a response
  }

 private:
  // Non-virtual support methods.

  void HandleSensorDescriptionRequest(const AlpacaRequest& request,
                                      Print& out) {
    if (!request.extra_parameters.contains(EParameter::kSensorName)) {
      return SendMissingParameter(EParameter::kSensorName);
    }
    StringView sensor_name =
        request.extra_parameters.find(EParameter::kSensorName);
    if (sensor_name.case_equals(alpaca::Literals::kHumidity) ||
        sensor_name.case_equals(alpaca::Literals::kRainRate) ||
        sensor_name.case_equals(alpaca::Literals::kSkyBrightness) ||
        sensor_name.case_equals(alpaca::Literals::kSkyTemperature) ||
        sensor_name.case_equals(alpaca::Literals::kTemperature)) {
      return SendJsonStringResponse("AAG CloudWatcher");
    }
    return SendInvalidParameterValue(request, out, EParameter::kSensorName,
                                     sensor_name);
  }
};

static Dht22Handler dht_handler;
static AagCloudWatcherHandler aag_handler;

constexpr alpaca::CommonDeviceHandler& kDeviceHandlers[]{
    dht_handler,
    aag_handler,
};

static alpaca::Server alpaca_server(kServerDescription, kDeviceHandlers);

void setup() {
  // Setup serial, wait for it to be ready so that our logging messages can be
  // read.
  Serial.begin(9600);
  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost.
  while (!Serial) {
  }

  // Ask alpaca::Server to verify settings.
  if (!alpaca_server.VerifyConfig()) {
    announceFailure("Unable to verify Tiny Alpaca Server configuration");
  }

  // Do hardware setup (e.g. init connection to sensors).
  // TBD

  // The microcontroller may be ready before the network chip (based on docs and
  // experience), so we wait a bit for hardware to be ready.
  delay(200);

  // Initialize networking. Provide an "Organizationally Unique Identifier"
  // that will be the first 3 bytes of the MAC addresses generated; this means
  // that all boards running this sketch will share the first 3 bytes of their
  // MAC addresses, which may help with locating them if other discovery means
  // are failing.
  OuiPrefix oui_prefix(0x52, 0xC4, 0x55);  // TODO Choose a value.
  if (!alpaca_server.SetupNetworking(&oui_prefix)) {
    announceFailure("Unable to initialize networking!");
  }
}

// For now only supporting one request at a time. Unless there are multiple
// clients, and some clients are slow to write requests or read responses,
// this shouldn't be a problem.
static AlpacaRequest request;

void loop() {
  // If there is a new client HTTP connection, start decoding the request;
  // If there is an existing client connection, continue decoding until
  // complete;
  // When done decoding successfully, handle request, write response and
  // close connection. Note that writing the response may require multiple
  // Ethernet packets. Fortunately, the common networking chips (e.g. W5100 and
  // W5500, both from Wiznet) have buffers of their own for TCP/IP packets, so
  // for the size of response we're likely to write (i.e. not image buffers),
  // we can reasonably expect to be able do so during one call to PerformIO.
  alpaca_server.PerformIO(request);
}
