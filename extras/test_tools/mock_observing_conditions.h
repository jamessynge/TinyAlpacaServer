#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_OBSERVING_CONDITIONS_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_OBSERVING_CONDITIONS_H_

// Mock version of an ObservingConditionsAdapter subclass. Only mocks those
// methods that a subclass must override in order to enable interacting with
// sensors, not methods like HandleGetRequest.
//
// Author: james.synge@gmail.com

#include <McuCore.h>
#include <stdint.h>

#include "device_types/observing_conditions/observing_conditions_adapter.h"
#include "gmock/gmock.h"

namespace alpaca {
namespace test {

class MockObservingConditions : public ::alpaca::ObservingConditionsAdapter {
 public:
  using ObservingConditionsAdapter::ObservingConditionsAdapter;

  MOCK_METHOD(void, ResetHardware, (), (override));

  MOCK_METHOD(void, InitializeDevice, (), (override));

  MOCK_METHOD(mcucore::StatusOr<bool>, GetConnected, (), (override));

  MOCK_METHOD(bool, HandlePutAction, (const AlpacaRequest& request, Print& out),
              (override));

  MOCK_METHOD(bool, HandlePutCommandBlind,
              (const AlpacaRequest& request, Print& out), (override));

  MOCK_METHOD(bool, HandlePutCommandBool,
              (const AlpacaRequest& request, Print& out), (override));

  MOCK_METHOD(bool, HandlePutCommandString,
              (const AlpacaRequest& request, Print& out), (override));

  MOCK_METHOD(bool, HandlePutConnected,
              (const AlpacaRequest& request, Print& out), (override));

  MOCK_METHOD(mcucore::Status, SetConnected, (bool), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetAveragePeriod, (), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetCloudCover, (), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetDewPoint, (), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetHumidity, (), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetPressure, (), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetRainRate, (), (override));

  MOCK_METHOD(mcucore::StatusOr<class mcucore::ProgmemStringView>,
              GetSensorDescription, (enum ESensorName), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetSkyBrightness, (), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetSkyQuality, (), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetSkyTemperature, (), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetStarFWHM, (), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetTemperature, (), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetTimeSinceLastUpdate,
              (enum ESensorName), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetWindDirection, (), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetWindGust, (), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetWindSpeed, (), (override));

  MOCK_METHOD(bool, HandlePutAveragePeriod,
              (const AlpacaRequest& request, Print& out), (override));

  MOCK_METHOD(bool, HandlePutRefresh,
              (const AlpacaRequest& request, Print& out), (override));

  MOCK_METHOD(mcucore::Status, SetAveragePeriod, (double), (override));

  MOCK_METHOD(double, MaxAveragePeriod, (), (const, override));

  MOCK_METHOD(mcucore::Status, Refresh, (), (override));
};
}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_OBSERVING_CONDITIONS_H_
