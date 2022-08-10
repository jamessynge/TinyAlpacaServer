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
  explicit AMWeatherBox(const alpaca::DeviceDescription& device_description);

  void InitializeDevice() override;

  void MaintainDevice() override;

  mcucore::StatusOr<double> GetAveragePeriod() override;
  mcucore::StatusOr<double> GetRainRate() override;
  mcucore::StatusOr<double> GetSkyTemperature() override;
  mcucore::StatusOr<double> GetTemperature() override;
  mcucore::StatusOr<mcucore::ProgmemStringView> GetSensorDescription(
      alpaca::ESensorName sensor_name) override;
  mcucore::Status SetAveragePeriod(double hours) override;
  mcucore::Status Refresh() override;
  mcucore::StatusOr<double> GetTimeSinceLastUpdate(
      alpaca::ESensorName sensor_name) override;

 private:
  bool IsIrThermInitialized();

  bool ir_therm_initialized_;
  uint32_t last_read_time_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_AM_WEATHERBOX_SRC_AM_WEATHER_BOX_H_
