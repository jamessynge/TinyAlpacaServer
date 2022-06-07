#include "fake_weather.h"

#include <McuCore.h>

using ::alpaca::ObservingConditionsAdapter;
using ::mcucore::ProgmemStringView;
using ::mcucore::StatusOr;

FakeWeather::FakeWeather(const alpaca::DeviceInfo& device_info)
    : ObservingConditionsAdapter(device_info) {}

mcucore::StatusOr<double> FakeWeather::GetTemperature() { return 20; }

mcucore::StatusOr<double> FakeWeather::GetSkyTemperature() { return -5; }

mcucore::StatusOr<mcucore::ProgmemStringView> FakeWeather::GetSensorDescription(
    alpaca::ESensorName sensor_name) {
  if (sensor_name == alpaca::ESensorName::kTemperature ||
      sensor_name == alpaca::ESensorName::kSkyTemperature) {
    return MCU_PSV("Fantasy Device");
  }
  return alpaca::ErrorCodes::InvalidValue();
}
