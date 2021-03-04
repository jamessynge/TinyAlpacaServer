#ifndef TINY_ALPACA_SERVER_EXTRAS_ETHERNET3_ETHERNET_H_
#define TINY_ALPACA_SERVER_EXTRAS_ETHERNET3_ETHERNET_H_

// Just enough of EthernetClass for Tiny Alpaca Server to compile on host, maybe
// to be a TCP server.

#include "extras/host_arduino/ip_address.h"  // IWYU pragma: export

class EthernetClass {
 public:
  // Declaring functions in the order called.
  void setHostname(const char* hostname);

  // Pretend version of begin, does nothing.
  // On embedded device, sets up the network chip, starts it running.
  template <class... T>
  static int begin(T&&...) {
    return 1;
  }

  // Maintains lease on DHCP, returns DHCP_CHECK_RENEW_OK, etc.
  int maintain();

  IPAddress localIP();
  // IPAddress subnetMask();
  // IPAddress gatewayIP();
  // IPAddress dnsServerIP();

  // static EthernetLinkStatus linkStatus();
  // static EthernetHardwareStatus hardwareStatus();

  // Pretend version of begin, does nothing.
  template <class... T>
  static void begin(T&&...) {}

  // static void MACAddress(uint8_t *mac_address);
  // static IPAddress localIP();
  // static IPAddress subnetMask();
  // static IPAddress gatewayIP();
  // static IPAddress dnsServerIP() { return _dnsServerAddress; }

  // void setMACAddress(const uint8_t *mac_address);
  // void setLocalIP(const IPAddress local_ip);
  // void setSubnetMask(const IPAddress subnet);
  // void setGatewayIP(const IPAddress gateway);
  // void setDnsServerIP(const IPAddress dns_server) { _dnsServerAddress =
  // dns_server; } void setRetransmissionTimeout(uint16_t milliseconds); void
  // setRetransmissionCount(uint8_t num);
};

extern EthernetClass Ethernet;

#endif  // TINY_ALPACA_SERVER_EXTRAS_ETHERNET3_ETHERNET_H_
