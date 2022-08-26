#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_SWITCH_GROUP_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_SWITCH_GROUP_H_

// Mock version of a SwitchAdapter subclass, where only the pure virtual methods
// of SwitchAdapter are mocked.
//
// Author: james.synge@gmail.com

#include <McuCore.h>
#include <stdint.h>

#include "device_types/switch/switch_adapter.h"
#include "gmock/gmock.h"

namespace alpaca {
namespace test {

class MockSwitchGroup : public SwitchAdapter {
 public:
  using SwitchAdapter::SwitchAdapter;

  MOCK_METHOD(void, ResetHardware, (), (override));

  MOCK_METHOD(void, InitializeDevice, (), (override));

  MOCK_METHOD(bool, HandleGetSwitchDescription,
              (const AlpacaRequest &, uint16_t, class Print &), (override));

  MOCK_METHOD(bool, HandleGetSwitchName,
              (const AlpacaRequest &, uint16_t, class Print &), (override));

  MOCK_METHOD(bool, HandleSetSwitchName,
              (const AlpacaRequest &, uint16_t, class Print &), (override));

  MOCK_METHOD(uint16_t, GetMaxSwitch, (), (override));

  MOCK_METHOD(bool, GetCanWrite, (uint16_t), (override));

  MOCK_METHOD(mcucore::StatusOr<bool>, GetSwitch, (uint16_t), (override));

  MOCK_METHOD(mcucore::StatusOr<double>, GetSwitchValue, (uint16_t),
              (override));

  MOCK_METHOD(double, GetMinSwitchValue, (uint16_t), (override));

  MOCK_METHOD(double, GetMaxSwitchValue, (uint16_t), (override));

  MOCK_METHOD(double, GetSwitchStep, (uint16_t), (override));

  MOCK_METHOD(mcucore::Status, SetSwitch, (uint16_t, bool), (override));

  MOCK_METHOD(mcucore::Status, SetSwitchValue, (uint16_t, double), (override));
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_SWITCH_GROUP_H_
