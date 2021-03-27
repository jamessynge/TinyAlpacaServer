#ifndef TINY_ALPACA_SERVER_EXAMPLES_TINYALPACASERVERDEMO_DHT22_HANDLER_H_
#define TINY_ALPACA_SERVER_EXAMPLES_TINYALPACASERVERDEMO_DHT22_HANDLER_H_

// Dht22Handler represents a DHT22 humidity and sensor as an ASCOM Alpaca
// Observing Conditions device.

#ifdef ARDUINO
#include <TinyAlpacaServer.h>
#else
#include "TinyAlpacaServer.h"
#endif

class Dht22Handler : public alpaca::ObservingConditionsAdapter {
 public:
  Dht22Handler();

  alpaca::StatusOr<float> GetHumidity() override;
  alpaca::StatusOr<float> GetTemperature() override;
  bool GetConnected() override;
  alpaca::StatusOr<alpaca::Literal> GetSensorDescription(
      alpaca::StringView sensor_name) override;
};

#endif  // TINY_ALPACA_SERVER_EXAMPLES_TINYALPACASERVERDEMO_DHT22_HANDLER_H_
