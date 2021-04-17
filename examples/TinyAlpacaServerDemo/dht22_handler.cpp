#include "dht22_handler.h"

#include "pretend_devices.h"

using ::alpaca::Literal;
using ::alpaca::ObservingConditionsAdapter;
using ::alpaca::StatusOr;

// Just one simple device, used to report Observing Conditions.
static Dht22Device dht22;

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
TAS_DEFINE_LITERAL(DHT22Name, "DHT22");
TAS_DEFINE_LITERAL(DHT22Description, "DHT22 Humidity and Temperature Sensor");
TAS_DEFINE_LITERAL(DHT22DriverInfo, "https://github/aavso/...");
TAS_DEFINE_LITERAL(DHT22DriverVersion, "https://github/aavso/...");

// No extra actions.
const auto kSupportedActions = alpaca::LiteralArray({});

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

Dht22Handler::Dht22Handler() : ObservingConditionsAdapter(kDht22DeviceInfo) {}

StatusOr<double> Dht22Handler::GetHumidity() {
  return dht22.get_relative_humidity();
}

StatusOr<double> Dht22Handler::GetTemperature() {
  return dht22.get_temperature();
}

StatusOr<bool> Dht22Handler::GetConnected() {
  return true;  // XXX: Return true if able to talk to the device.
}

StatusOr<Literal> Dht22Handler::GetSensorDescription(
    alpaca::ESensorName sensor_name) {
  if (sensor_name == alpaca::ESensorName::kHumidity ||
      sensor_name == alpaca::ESensorName::kTemperature) {
    return DHT22Description();
  }
  return alpaca::ErrorCodes::InvalidValue();
}
