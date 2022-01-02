#ifndef TINY_ALPACA_SERVER_EXAMPLES_TINYALPACASERVERDEMO_DHT22_HANDLER_H_
#define TINY_ALPACA_SERVER_EXAMPLES_TINYALPACASERVERDEMO_DHT22_HANDLER_H_

// Dht22Handler represents a DHT22 humidity and sensor as an ASCOM Alpaca
// Observing Conditions device.
//
// Author: james.synge@gmail.com

#include <TinyAlpacaServer.h>

class Dht22Handler : public alpaca::ObservingConditionsAdapter {
 public:
  Dht22Handler();

  mcucore::StatusOr<double> GetHumidity() override;
  mcucore::StatusOr<double> GetTemperature() override;
  mcucore::StatusOr<bool> GetConnected() override;
  mcucore::StatusOr<mcucore::ProgmemStringView> GetSensorDescription(
      alpaca::ESensorName sensor_name) override;
};

#endif  // TINY_ALPACA_SERVER_EXAMPLES_TINYALPACASERVERDEMO_DHT22_HANDLER_H_
