#include "am_weather_box.h"

#include <Adafruit_MLX90614.h>
#include <Arduino.h>
#include <TinyAlpacaServer.h>

#include "constants.h"

namespace astro_makers {
namespace {

using ::alpaca::ErrorCodes;
using ::alpaca::ESensorName;
using ::alpaca::Literals;
using ::alpaca::ObservingConditionsAdapter;
using ::mcucore::OkStatus;
using ::mcucore::Status;
using ::mcucore::StatusOr;

#if defined(MCU_ENABLED_VLOG_LEVEL) && MCU_ENABLED_VLOG_LEVEL >= 3
#define READ_INTERVAL_SECS 1
#else
#define READ_INTERVAL_SECS 15
#endif

constexpr uint32_t kReadIntervalMillis = READ_INTERVAL_SECS * 1000;

TAS_DEFINE_LITERAL(MLX90614Description, "MLX90614 Infrared Thermometer");
TAS_DEFINE_LITERAL(RG11Description, "Hydreon RG11 Rain Sensor");

Adafruit_MLX90614 ir_therm;

}  // namespace

AMWeatherBox::AMWeatherBox(const alpaca::DeviceInfo& device_info)
    : ObservingConditionsAdapter(device_info), ir_therm_initialized_(false) {}

void AMWeatherBox::Initialize() {
  ObservingConditionsAdapter::Initialize();
  pinMode(kRg11SensorPin, kRg11SensorPinMode);
  if (IsIrThermInitialized()) {
    last_read_time_ = millis();
    MCU_VLOG(1) << MCU_FLASHSTR("MLX90614 is ready");
  } else {
    MCU_VLOG(1) << MCU_FLASHSTR("MLX90614 is not present or ready!");
  }
}

void AMWeatherBox::MaintainDevice() {
  auto now = millis();
  if ((now - last_read_time_) >= kReadIntervalMillis) {
    last_read_time_ = now;
    if (IsIrThermInitialized()) {
      MCU_VLOG(3) << MCU_FLASHSTR("Sky: ") << GetSkyTemperature().value()
                  << MCU_FLASHSTR(" \xE2\x84\x83, Ambient: ")
                  << GetTemperature().value()
                  << MCU_FLASHSTR(" \xE2\x84\x83, Rain Detected: ")
                  << (GetRainRate().value() == 0 ? Literals::False()
                                                 : Literals::True());
    } else {
      MCU_VLOG(3) << MCU_FLASHSTR("Rain Detected: ")
                  << (GetRainRate().value() == 0 ? Literals::False()
                                                 : Literals::True());
    }
  }
}

bool AMWeatherBox::IsIrThermInitialized() {
  if (!ir_therm_initialized_) {
    ir_therm_initialized_ = ir_therm.begin();
  }
  return ir_therm_initialized_;
}

mcucore::StatusOr<double> AMWeatherBox::GetAveragePeriod() { return 0; }

mcucore::Status AMWeatherBox::SetAveragePeriod(double hours) {
  MCU_DCHECK_EQ(hours, 0);  // MaxAveragePeriod should be 0.
  if (hours == 0) {
    return mcucore::OkStatus();
  } else {
    return ErrorCodes::InvalidValue();
  }
}

mcucore::StatusOr<double> AMWeatherBox::GetSkyTemperature() {
  if (IsIrThermInitialized()) {
    return ir_therm.readObjectTempC();
  }
  return ErrorCodes::NotConnected();
}

mcucore::StatusOr<double> AMWeatherBox::GetRainRate() {
  if (digitalRead(kRg11SensorPin) == kRg11DetectsRain) {
    return 10;
  } else {
    return 0;
  }
}

mcucore::StatusOr<double> AMWeatherBox::GetTemperature() {
  if (IsIrThermInitialized()) {
    return ir_therm.readAmbientTempC();
  }
  return ErrorCodes::NotConnected();
}

mcucore::StatusOr<mcucore::Literal> AMWeatherBox::GetSensorDescription(
    ESensorName sensor_name) {
  if (sensor_name == ESensorName::kSkyTemperature ||
      sensor_name == ESensorName::kTemperature) {
    return MLX90614Description();
  }
  if (sensor_name == ESensorName::kRainRate) {
    return RG11Description();
  }
  return ErrorCodes::InvalidValue();
}

mcucore::Status AMWeatherBox::Refresh() { return mcucore::OkStatus(); }

mcucore::StatusOr<double> AMWeatherBox::GetTimeSinceLastUpdate(
    ESensorName sensor_name) {
  switch (sensor_name) {
    case ESensorName::kSkyTemperature:
    case ESensorName::kTemperature:
      if (IsIrThermInitialized()) {
        return 0.0;
      }
      break;

    case ESensorName::kRainRate:
      return 0.0;

    default:
      break;
  }
  return ErrorCodes::NotImplemented();
}

}  // namespace astro_makers
