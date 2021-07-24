#include "utils/ip_device.h"

#include "utils/inline_literal.h"
#include "utils/logging.h"
#include "utils/platform_ethernet.h"
#include "utils/traits/set_dhcp_trait.h"

namespace alpaca {
namespace {
constexpr uint8_t kW5500ChipSelectPin = 10;
constexpr uint8_t kW5500ResetPin = 7;
constexpr uint8_t kSDcardSelectPin = 4;

// T is a class with a setDhcp function.
template <typename T>
void MaybeSetDhcp(T& ethernet, true_type /*has_set_dhcp*/) {
  // We allocate a DhcpClass at file scope so that it isn't dynamically
  // allocated at setup time (i.e. so we're in better control of memory
  // consumption).
  static ::DhcpClass dhcp;
  ethernet.setDhcp(&dhcp);
}

// Type T does NOT have a setDhcp method.
template <typename T>
void MaybeSetDhcp(T& ethernet, false_type /*!has_set_dhcp*/) {}

template <typename T>
void MaybeSetDhcp(T& ethernet) {
  MaybeSetDhcp(ethernet, has_set_dhcp<T>{});
}

}  // namespace

// static
void Mega2560Eth::SetupW5500(uint8_t max_sock_num) {
  // Make sure that the SD Card interface is not the selected SPI device.
  pinMode(kSDcardSelectPin, OUTPUT);
  digitalWrite(kSDcardSelectPin, HIGH);

  // Configure Ethernet3's EthernetClass instance with the pins used to access
  // the W5500.
  Ethernet.setRstPin(kW5500ResetPin);
  Ethernet.setCsPin(kW5500ChipSelectPin);

  // If there has been a crash and restart of the ATmega, I've found that the
  // networking seems to be broken, so doing a hard reset explicitly so that
  // we always act more like a power-up situation.
  Ethernet.hardreset();

  // For now use all of the allowed sockets. Need to have at least one UDP
  // socket, and maybe more; our UDP uses include DHCP lease & lease renewal,
  // the Alpaca discovery protocol, and possibly for time. Then we need at least
  // one TCP socket, more if we want to handle multiple simultaneous requests.
  Ethernet.init(max_sock_num);
}

bool IpDevice::InitializeNetworking(const OuiPrefix* oui_prefix) {
  MaybeSetDhcp(Ethernet);

  // Load the addresses saved to EEPROM, if they were previously saved. If
  // they were not successfully loaded, then generate them and save them into
  // the EEPROM.
  Addresses addresses;
  addresses.loadOrGenAndSave(oui_prefix);

  if (Ethernet.begin(addresses.mac.mac)) {
    // Wonderful news, we were able to get an IP address via DHCP.
    using_dhcp_ = true;
  } else {
    // No DHCP server responded with a lease on an IP address.
    // Is there hardware?
    MacAddress mac;
    Ethernet.macAddress(mac.mac);
    if (!(mac == addresses.mac)) {
      // Oops, this isn't the right board to run this sketch.
      LogSink() << TAS_FLASHSTR("Found no network hardware");
      return false;
    }

    LogSink() << TAS_FLASHSTR("No DHCP, using default IP ") << addresses.ip;

    // No DHCP server responded with a lease on an IP address, so we'll
    // fallback to using our randomly generated IP.
    using_dhcp_ = false;

    // The link-local address range must not be divided into smaller
    // subnets, so we set our subnet mask accordingly:
    IPAddress subnet(255, 255, 0, 0);

    // Assume that the gateway is on the same subnet, at address 1 within
    // the subnet. This code will work with many subnets, not just a /16.
    IPAddress gateway = addresses.ip;
    gateway[0] &= subnet[0];
    gateway[1] &= subnet[1];
    gateway[2] &= subnet[2];
    gateway[3] &= subnet[3];
    gateway[3] |= 1;

    Ethernet.begin(addresses.mac.mac, addresses.ip, subnet, gateway);
  }

  return true;
}

int IpDevice::MaintainDhcpLease() {
  // If we're using an IP address assigned via DHCP, renew the lease
  // periodically. The Ethernet library will do so at the appropriate interval
  // if we call it often enough.
  if (using_dhcp_) {
    return Ethernet.maintain();
  } else {
    return DHCP_CHECK_NONE;
  }
}

void IpDevice::PrintNetworkAddresses() {
  alpaca::MacAddress mac;
  Ethernet.macAddress(mac.mac);
  LogSink() << TAS_FLASHSTR("MAC: ") << mac;
  LogSink() << TAS_FLASHSTR("IP: ") << Ethernet.localIP();
  LogSink() << TAS_FLASHSTR("Subnet: ") << Ethernet.subnetMask();
  LogSink() << TAS_FLASHSTR("Gateway: ") << Ethernet.gatewayIP();
  LogSink() << TAS_FLASHSTR("DNS: ") << Ethernet.dnsServerIP();
}

}  // namespace alpaca
