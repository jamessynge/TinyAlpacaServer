#ifndef TINY_ALPACA_SERVER_SRC_UTILS_IP_DEVICE_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_IP_DEVICE_H_

// IpDevice takes care of setting up the Internet Offload device of the embedded
// device; currently supporting the WIZnet W5500 of a RobotDyn Mega ETH board.

#include "utils/addresses.h"
#include "utils/platform.h"
#include "utils/platform_ethernet.h"

namespace alpaca {

struct Mega2560Eth {
  // Configures the pins necessary for talking to the W5500 on the RobotDyn Mega
  // 2560 ETH board. max_sock_num can be 8, 4, 2, or 1. An Alpaca server needs
  // at least two UDP ports (DHCP and Alpaca Discovery), plus at least 1 TCP
  // port for HTTP.
  static void SetupW5500(uint8_t max_sock_num = MAX_SOCK_NUM);
};

class IpDevice {
 public:
  // Set the MAC address of the Ethernet chip and get a DHCP assigned IP address
  // or use a generated address. Returns false if unable to configure addresses
  // or if there is no Ethernet hardware, else returns true.
  //
  // It *MAY* help with identifying devices on the network that are using this
  // software if they have a known "Organizationally Unique Identifier" (the
  // first 3 bytes of the MAC address). Therefore, setup() takes an optional
  // OuiPrefix allowing the caller to provide such a prefix.
  //
  // Call Mega2560Eth::SetupW5500 prior to calling this method.
  bool InitializeNetworking(const OuiPrefix* oui_prefix = nullptr);

  // Ensures that the DHCP lease (if there is one) is maintained. Returns a
  // DHCP_CHECK_* value; definitions in Ethernet3's Dhcp.h.
  int MaintainDhcpLease();

  static void PrintNetworkAddresses();

 private:
  bool using_dhcp_{false};
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_IP_DEVICE_H_
