#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_DEVICE_INTERFACE_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_DEVICE_INTERFACE_H_

#include <stdint.h>

#include <cstddef>

#include "alpaca_request.h"
#include "constants.h"
#include "device_description.h"
#include "device_interface.h"
#include "gmock/gmock.h"

namespace alpaca {
namespace test {

class MockDeviceInterface : public DeviceInterface {
 public:
  MOCK_METHOD(const alpaca::DeviceDescription&, device_description, (),
              (const, override));

  MOCK_METHOD(void, ValidateConfiguration, (), (override));

  MOCK_METHOD(void, ResetHardware, (), (override));

  MOCK_METHOD(void, InitializeDevice, (), (override));

  MOCK_METHOD(void, MaintainDevice, (), (override));

  MOCK_METHOD(bool, HandleDeviceSetupRequest,
              (const alpaca::AlpacaRequest& request, Print& out), (override));

  MOCK_METHOD(bool, HandleDeviceApiRequest,
              (const alpaca::AlpacaRequest& request, Print& out), (override));

  MOCK_METHOD(void, AddToHomePageHtml,
              (const alpaca::AlpacaRequest& request, EHtmlPageSection section,
               mcucore::OPrintStream& stream),
              (override));
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_DEVICE_INTERFACE_H_
