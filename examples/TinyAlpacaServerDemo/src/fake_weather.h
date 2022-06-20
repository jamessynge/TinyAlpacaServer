#ifndef TINY_ALPACA_SERVER_EXAMPLES_TINYALPACASERVERDEMO_SRC_FAKE_WEATHER_H_
#define TINY_ALPACA_SERVER_EXAMPLES_TINYALPACASERVERDEMO_SRC_FAKE_WEATHER_H_

// FakeWeather demonstrates how an ASCOM Alpaca Observing Conditions device
// might be implemented (well, mocked).
//
// Author: james.synge@gmail.com

#include <TinyAlpacaServer.h>

class FakeWeather : public alpaca::ObservingConditionsAdapter {
 public:
  FakeWeather(const alpaca::DeviceInfo& device_info);

  // We override some base class property members, and provide fake values;
  // those that require a sensor and which are not overridden (e.g. GetDewPoint)
  // will return an error indicating that the property is not implemented.
  mcucore::StatusOr<double> GetTemperature() override;
  mcucore::StatusOr<double> GetSkyTemperature() override;

  mcucore::StatusOr<mcucore::ProgmemStringView> GetSensorDescription(
      alpaca::ESensorName sensor_name) override;
};

#endif  // TINY_ALPACA_SERVER_EXAMPLES_TINYALPACASERVERDEMO_SRC_FAKE_WEATHER_H_
