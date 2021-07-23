#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_OBSERVING_CONDITIONS_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_OBSERVING_CONDITIONS_H_

// Mock version of an ObservingConditionsAdapter subclass. Only mocks those
// methods that a subclass must override in order to enable interacting with
// sensors, not methods like HandleGetRequest.

#include <stdint.h>

#include "device_info.h"
#include "device_types/observing_conditions/observing_conditions_adapter.h"
#include "googletest/gmock.h"
#include "utils/status.h"
#include "utils/status_or.h"

namespace alpaca {
namespace test {

class MockObservingConditions : public ::alpaca::ObservingConditionsAdapter {
 public:
  using ObservingConditionsAdapter::ObservingConditionsAdapter;

  MOCK_METHOD(StatusOr<bool>, GetConnected, (), (override));

  MOCK_METHOD(bool, HandlePutAction,
              (const struct AlpacaRequest &, class Print &), (override));

  MOCK_METHOD(bool, HandlePutCommandBlind,
              (const struct AlpacaRequest &, class Print &), (override));

  MOCK_METHOD(bool, HandlePutCommandBool,
              (const struct AlpacaRequest &, class Print &), (override));

  MOCK_METHOD(bool, HandlePutCommandString,
              (const struct AlpacaRequest &, class Print &), (override));

  MOCK_METHOD(bool, HandlePutConnected,
              (const struct AlpacaRequest &, class Print &), (override));

  MOCK_METHOD(class Status, SetConnected, (bool), (override));

  MOCK_METHOD(StatusOr<double>, GetAveragePeriod, (), (override));

  MOCK_METHOD(StatusOr<double>, GetCloudCover, (), (override));

  MOCK_METHOD(StatusOr<double>, GetDewPoint, (), (override));

  MOCK_METHOD(StatusOr<double>, GetHumidity, (), (override));

  MOCK_METHOD(StatusOr<double>, GetPressure, (), (override));

  MOCK_METHOD(StatusOr<double>, GetRainRate, (), (override));

  MOCK_METHOD(StatusOr<class Literal>, GetSensorDescription, (enum ESensorName),
              (override));

  MOCK_METHOD(StatusOr<double>, GetSkyBrightness, (), (override));

  MOCK_METHOD(StatusOr<double>, GetSkyQuality, (), (override));

  MOCK_METHOD(StatusOr<double>, GetSkyTemperature, (), (override));

  MOCK_METHOD(StatusOr<double>, GetStarFWHM, (), (override));

  MOCK_METHOD(StatusOr<double>, GetTemperature, (), (override));

  MOCK_METHOD(StatusOr<double>, GetTimeSinceLastUpdate, (enum ESensorName),
              (override));

  MOCK_METHOD(StatusOr<double>, GetWindDirection, (), (override));

  MOCK_METHOD(StatusOr<double>, GetWindGust, (), (override));

  MOCK_METHOD(StatusOr<double>, GetWindSpeed, (), (override));

  MOCK_METHOD(bool, HandlePutAveragePeriod,
              (const struct AlpacaRequest &, class Print &), (override));

  MOCK_METHOD(bool, HandlePutRefresh,
              (const struct AlpacaRequest &, class Print &), (override));

  MOCK_METHOD(class Status, SetAveragePeriod, (double), (override));

  MOCK_METHOD(double, MaxAveragePeriod, (), (const, override));

  MOCK_METHOD(class Status, Refresh, (), (override));
};
}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_OBSERVING_CONDITIONS_H_
