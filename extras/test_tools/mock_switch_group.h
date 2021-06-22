#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_SWITCH_GROUP_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_SWITCH_GROUP_H_

// Mock version of a SwitchAdapter subclass, where only the pure virtual methods
// of SwitchAdapter are mocked.

#include "device_types/switch/switch_adapter.h"
#include "extras/test_tools/mock_device_interface.h"
#include "googletest/gmock.h"

namespace alpaca {

class MockSwitchGroup : public SwitchAdapter {
 public:
  explicit MockSwitchGroup(const DeviceInfo &device_info)
      : SwitchAdapter(device_info) {}

  MOCK_METHOD(bool, HandleGetSwitchDescription,
              (const struct alpaca::AlpacaRequest &, int32_t, class Print &),
              (override));

  MOCK_METHOD(bool, HandleGetSwitchName,
              (const struct alpaca::AlpacaRequest &, int32_t, class Print &),
              (override));

  MOCK_METHOD(bool, HandleSetSwitchName,
              (const struct alpaca::AlpacaRequest &, int32_t,
               class alpaca::StringView, class Print &),
              (override));

  MOCK_METHOD(uint16_t, GetMaxSwitch, (), (override));

  MOCK_METHOD(bool, GetCanWrite, (int32_t), (override));

  MOCK_METHOD(StatusOr<bool>, GetSwitch, (int32_t), (override));

  MOCK_METHOD(StatusOr<double>, GetSwitchValue, (int32_t), (override));

  MOCK_METHOD(double, GetMinSwitchValue, (int32_t), (override));

  MOCK_METHOD(double, GetMaxSwitchValue, (int32_t), (override));

  MOCK_METHOD(double, GetSwitchStep, (int32_t), (override));

  MOCK_METHOD(class alpaca::Status, PutSetSwitch, (int32_t, bool), (override));

  MOCK_METHOD(class alpaca::Status, PutSetSwitchValue, (int32_t, double),
              (override));
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_SWITCH_GROUP_H_
