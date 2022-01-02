#include "src/dht22_handler.h"

#include "src/pretend_devices.h"

using ::alpaca::ObservingConditionsAdapter;
using ::mcucore::ProgmemStringView;
using ::mcucore::StatusOr;

// Just one simple device, used to report Observing Conditions.
static Dht22Device dht22;

#define DEVICE_DESCRIPTION "DHT22 Humidity and Temperature Sensor"

const alpaca::DeviceInfo kDht22DeviceInfo{
    .device_type = alpaca::EDeviceType::kObservingConditions,
    .device_number = 1,
    .name = MCU_FLASHSTR("DHT22"),
    .unique_id = MCU_FLASHSTR("0.1"),
    .description = MCU_FLASHSTR(DEVICE_DESCRIPTION),
    .driver_info = MCU_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
    .driver_version = MCU_FLASHSTR("0.1"),
    .supported_actions = {},  // No extra actions.
    .interface_version = 1,
};

Dht22Handler::Dht22Handler() : ObservingConditionsAdapter(kDht22DeviceInfo) {}

mcucore::StatusOr<double> Dht22Handler::GetHumidity() {
  return dht22.get_relative_humidity();
}

mcucore::StatusOr<double> Dht22Handler::GetTemperature() {
  return dht22.get_temperature();
}

mcucore::StatusOr<bool> Dht22Handler::GetConnected() {
  return true;  // XXX: Return true if able to talk to the device.
}

mcucore::StatusOr<mcucore::ProgmemStringView>
Dht22Handler::GetSensorDescription(alpaca::ESensorName sensor_name) {
  if (sensor_name == alpaca::ESensorName::kHumidity ||
      sensor_name == alpaca::ESensorName::kTemperature) {
    return mcucore::ProgmemStringView(TASLIT(DEVICE_DESCRIPTION));
  }
  return alpaca::ErrorCodes::InvalidValue();
}
