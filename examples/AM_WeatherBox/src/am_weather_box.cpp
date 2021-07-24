#include "am_weather_box.h"

#include <Arduino.h>
#include <SparkFunMLX90614.h>
#include <TinyAlpacaServer.h>

#include "constants.h"

namespace astro_makers {
namespace {

using ::alpaca::ErrorCodes;
using ::alpaca::ESensorName;
using ::alpaca::ObservingConditionsAdapter;
using ::alpaca::OkStatus;
using ::alpaca::Status;
using ::alpaca::StatusOr;

#if defined(TAS_ENABLED_VLOG_LEVEL) && TAS_ENABLED_VLOG_LEVEL >= 3
#define READ_INTERVAL_SECS 3
#else
#define READ_INTERVAL_SECS 15
#endif

constexpr uint32_t kReadIntervalMillis = READ_INTERVAL_SECS * 1000;

TAS_DEFINE_LITERAL(MLX90614Description, "MLX90614 Infrared Thermometer");
TAS_DEFINE_LITERAL(RG11Description, "Hydreon RG11 Rain Sensor");

IRTherm ir_therm;

}  // namespace

AMWeatherBox::AMWeatherBox(const alpaca::DeviceInfo& device_info)
    : ObservingConditionsAdapter(device_info),
      ir_therm_ready_(false),
      last_read_time_(0) {}

void AMWeatherBox::Initialize() {
  ObservingConditionsAdapter::Initialize();
  pinMode(kRg11SensorPin, kRg11SensorPinMode);
  if (DoReadIrTemps()) {
    last_read_time_ = millis();
  } else {
    TAS_VLOG(1) << TAS_FLASHSTR("MLX90614 not present or ready!");
  }
}

void AMWeatherBox::MaintainDevice() {
  auto now = millis();
  if ((now - last_read_time_) >= kReadIntervalMillis) {
    if (DoReadIrTemps()) {
      last_read_time_ = now;
      TAS_VLOG(3) << TAS_FLASHSTR("Sky: ") << GetSkyTemperature().value()
                  << TAS_FLASHSTR("\xE2\x84\x83, Ambient: ")
                  << GetTemperature().value()
                  << TAS_FLASHSTR("\xE2\x84\x83, Rain Detected: ")
                  << (GetRainRate().value() == 0 ? Literals::False()
                                                 : Literals::True());
    }
  }
}

bool AMWeatherBox::DoReadIrTemps() {
  if (ir_therm_ready_ || ir_therm.begin()) {
    ir_therm_ready_ = ir_therm.read();
  }
  return ir_therm_ready_;
}

StatusOr<double> AMWeatherBox::GetAveragePeriod() { return 0; }

Status AMWeatherBox::SetAveragePeriod(double hours) {
  TAS_DCHECK_EQ(hours, 0);  // MaxAveragePeriod should be 0.
  if (hours == 0) {
    return OkStatus();
  } else {
    return ErrorCodes::InvalidValue();
  }
}

StatusOr<double> AMWeatherBox::GetSkyTemperature() {
  if (ir_therm_ready_) {
    return ir_therm.object();
  }
  return ErrorCodes::NotConnected();
}

StatusOr<double> AMWeatherBox::GetRainRate() {
  if (digitalRead(kRg11SensorPin) == kRg11DetectsRain) {
    return 10;
  } else {
    return 0;
  }
}

StatusOr<double> AMWeatherBox::GetTemperature() {
  if (ir_therm_ready_) {
    return ir_therm.ambient();
  }
  return ErrorCodes::NotConnected();
}

StatusOr<alpaca::Literal> AMWeatherBox::GetSensorDescription(
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

Status AMWeatherBox::Refresh() {
  auto now = millis();
  if (DoReadIrTemps()) {
    last_read_time_ = now;
    return OkStatus();
  }
  return ErrorCodes::NotConnected();
}

}  // namespace astro_makers
