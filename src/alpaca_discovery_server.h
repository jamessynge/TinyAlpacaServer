#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_DISCOVERY_SERVER_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_DISCOVERY_SERVER_H_

// TinyAlpacaDiscoveryServer responds to Alpaca discovery messages, enabling an
// ASCOM Alpaca client to learn about a Tiny Alpaca Server device on the
// network.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "experimental/users/jamessynge/arduino/mcunet/src/platform_ethernet.h"

namespace alpaca {

class TinyAlpacaDiscoveryServer {
 public:
  explicit TinyAlpacaDiscoveryServer(uint16_t tcp_port) : tcp_port_(tcp_port) {}

  // Prepares a UDP socket to listen for Alpaca Discovery Protocol messages.
  bool Initialize();

  // Handles UDP requests.
  void PerformIO();

 private:
  EthernetUDP udp_;
  uint16_t tcp_port_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_DISCOVERY_SERVER_H_
