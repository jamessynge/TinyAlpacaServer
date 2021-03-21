#ifndef TINY_ALPACA_SERVER_SRC_UTILS_IP_DEVICE_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_IP_DEVICE_H_

// IpDevice takes care of setting up the Internet Offload device of the embedded
// device; currently supporting the WIZnet W5500 of a RobotDyn Mega ETH board.

#include "utils/addresses.h"
#include "utils/platform.h"

namespace alpaca {

struct Mega2560Eth {
  static constexpr uint8_t kW5500ChipSelectPin = 10;
  static constexpr uint8_t kW5500ResetPin = 7;
  static constexpr uint8_t kSDcardSelectPin = 4;

  // Configures the pins necessary for talking to the W5500 on the RobotDyn Mega
  // 2560 ETH board.
  static void setup_w5500();
};

class IpDevice {
 public:
  // TODO(jamessynge): Consider providing a ctor or method for setting the  RST
  // and CS pins; currently this must be done by the caller.

  // Set the MAC address of the Ethernet chip and get a DHCP assigned IP address
  // or use a generated address. Returns false if unable to configure addresses
  // or if there is no Ethernet hardware, else returns true.
  //
  // It *MAY* help with identifying devices on the network that are using this
  // software if they have a known "Organizationally Unique Identifier" (the
  // first 3 bytes of the MAC address). Therefore, setup() takes an optional
  // OuiPrefix allowing the caller to provide such a prefix.
  //
  // Call Mega2560Eth::setup_w5500 prior to calling this method.
  bool setup(const OuiPrefix* oui_prefix = nullptr);

  // Ensures that the DHCP lease (if there is one) is maintained. Returns a
  // DHCP_CHECK_* value; definitions in Ethernet3's Dhcp.h.
  int maintain_dhcp_lease();

 private:
  bool using_dhcp_{false};
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_IP_DEVICE_H_
