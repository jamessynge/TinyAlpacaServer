#ifndef TINY_ALPACA_SERVER_EXAMPLES_AM_WEATHERBOX_SRC_AM_WEATHER_BOX_H_
#define TINY_ALPACA_SERVER_EXAMPLES_AM_WEATHERBOX_SRC_AM_WEATHER_BOX_H_

// The AMWeatherBox class presents the AstroMakers WeatherBox device as an ASCOM
// Alpaca ObservingConditions device
//
// Author: james.synge@gmail.com

#include <TinyAlpacaServer.h>

namespace astro_makers {

class AMWeatherBox : public alpaca::ObservingConditionsAdapter {
 public:
  explicit AMWeatherBox(const alpaca::DeviceInfo& device_info);

  void Initialize() override;

  void MaintainDevice() override;

  alpaca::StatusOr<double> GetAveragePeriod() override;
  alpaca::StatusOr<double> GetRainRate() override;
  alpaca::StatusOr<double> GetSkyTemperature() override;
  alpaca::StatusOr<double> GetTemperature() override;
  alpaca::StatusOr<alpaca::Literal> GetSensorDescription(
      alpaca::ESensorName sensor_name) override;
  alpaca::Status SetAveragePeriod(double hours) override;
  alpaca::Status Refresh() override;
  alpaca::StatusOr<double> GetTimeSinceLastUpdate(
      alpaca::ESensorName sensor_name) override;

 private:
  bool IsIrThermInitialized();

  bool ir_therm_initialized_;
  uint32_t last_read_time_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_AM_WEATHERBOX_SRC_AM_WEATHER_BOX_H_
