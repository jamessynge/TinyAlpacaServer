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
