#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_FAKE_PLATFORM_ETHERNET_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_FAKE_PLATFORM_ETHERNET_H_

// Goal: provide the ability to test ServerSocket, etc.

#include "utils/platform_ethernet.h"

namespace alpaca {

class FakePlatformEthernet : public PlatformEthernetInterface {
 public:
 private:
  struct FakeConnection {};
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_FAKE_PLATFORM_ETHERNET_H_
