#include "dht22_handler.h"

#include "pretend_devices.h"

using ::alpaca::Literal;
using ::alpaca::ObservingConditionsAdapter;
using ::alpaca::StatusOr;

// Just one simple device, used to report Observing Conditions.
static Dht22Device dht22;

// No extra actions.
const auto kSupportedActions = alpaca::LiteralArray({});

#define DEVICE_DESCRIPTION "DHT22 Humidity and Temperature Sensor"

const alpaca::DeviceInfo kDht22DeviceInfo{
    .device_type = alpaca::EDeviceType::kObservingConditions,
    .device_number = 1,
    .name = TAS_FLASHSTR("DHT22"),
    .unique_id = TAS_FLASHSTR("0.1"),
    .description = TAS_FLASHSTR(DEVICE_DESCRIPTION),
    .driver_info = TAS_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
    .driver_version = TAS_FLASHSTR("0.1"),
    .supported_actions = kSupportedActions,
    .interface_version = 1,
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
    return Literal(TASLIT(DEVICE_DESCRIPTION));
  }
  return alpaca::ErrorCodes::InvalidValue();
}
