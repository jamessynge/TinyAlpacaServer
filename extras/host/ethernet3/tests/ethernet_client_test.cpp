#include "extras/host/ethernet3/ethernet_client.h"

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace {

TEST(EthernetClientTest, NoFixtureTest) {
  EthernetClient a(1);
  a = EthernetClient(2);
}

}  // namespace
}  // namespace alpaca
