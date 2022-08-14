#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MINIMAL_DEVICE_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MINIMAL_DEVICE_H_

// Supports testing some aspects of DeviceImplBase.

#include "device_description.h"
#include "device_types/device_impl_base.h"
#include "server_context.h"

namespace alpaca {
namespace test {

class MinimalDevice : public DeviceImplBase {
 public:
  MinimalDevice(ServerContext& server_context,
                const DeviceDescription& description)
      : DeviceImplBase(server_context, description) {}

  void ResetHardware() override {}
  void InitializeDevice() override {}
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MINIMAL_DEVICE_H_
