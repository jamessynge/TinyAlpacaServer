#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_DEVICE_INTERFACE_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_DEVICE_INTERFACE_H_

#include <stdint.h>

#include <cstddef>

#include "alpaca_request.h"
#include "constants.h"
#include "device_info.h"
#include "device_interface.h"
#include "gmock/gmock.h"

namespace alpaca {
namespace test {

class MockDeviceInterface : public DeviceInterface {
 public:
  MOCK_METHOD(const struct alpaca::DeviceInfo &, device_info, (),
              (const, override));

  MOCK_METHOD(enum alpaca::EDeviceType, device_type, (), (const, override));

  MOCK_METHOD(uint32_t, device_number, (), (const, override));

  MOCK_METHOD(void, Initialize, (), (override));

  MOCK_METHOD(void, MaintainDevice, (), (override));

  MOCK_METHOD(size_t, GetUniqueBytes, (uint8_t *, size_t), (override));

  MOCK_METHOD(bool, HandleDeviceSetupRequest,
              (const struct alpaca::AlpacaRequest &, class Print &),
              (override));

  MOCK_METHOD(bool, HandleDeviceApiRequest,
              (const struct alpaca::AlpacaRequest &, class Print &),
              (override));
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_DEVICE_INTERFACE_H_
