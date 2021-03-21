#include "utils/ip_device.h"

#include "extras/host/ethernet3/ethernet_class.h"
#include "utils/platform.h"
#include "utils/platform_ethernet.h"

namespace alpaca {

// static
void Mega2560Eth::setup_w5500() {
  // We may not need all of these as the Ethernet3 EthernetClass and W5500Class
  // do some (or most) of this.
  // pinMode(kW5500ChipSelectPin, OUTPUT);
  // pinMode(kW5500ResetPin, OUTPUT);
  pinMode(kSDcardSelectPin, OUTPUT);

  // Tell the W5500 chip it is the selected SPI device.
  // digitalWrite(kW5500ChipSelectPin, LOW);

  // Tell the SD Card interface chip it is NOT the selected SPI device.
  digitalWrite(kSDcardSelectPin, HIGH);

  Ethernet.setRstPin(kW5500ResetPin);
  Ethernet.setCsPin(kW5500ChipSelectPin);

  // For now use all of the allowed sockets. Need to have at least one UDP
  // socket, and maybe more; our UDP uses include DHCP lease & lease renewal,
  // the Alpaca discovery protocol, and possibly for time. Then we need at least
  // one TCP socket, more if we want to handle multiple simultaneous requests.
  Ethernet.init(MAX_SOCK_NUM);
}

bool IpDevice::setup(const OuiPrefix* oui_prefix) {
  // Load the addresses saved to EEPROM, if they were previously saved. If
  // they were not successfully loaded, then generate them and save them into
  // the EEPROM.
  Addresses addresses;
  addresses.loadOrGenAndSave(oui_prefix);

  Serial.print("MAC: ");
  Serial.println(addresses.mac);
  Serial.print("Default IP: ");
  Serial.println(addresses.ip);

  if (Ethernet.begin(addresses.mac.mac)) {
    // Yeah, we were able to get an IP address via DHCP.
    using_dhcp_ = true;
  } else {
    // No DHCP server responded with a lease on an IP address.
    // Is there hardware?
    if (!Ethernet.link()) {
      // Oops, this isn't the right board to run this sketch.
      return false;
    }
    Serial.println("No DHCP");

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

    if (!Ethernet.begin(addresses.mac.mac, addresses.ip, subnet, gateway)) {
      return false;
    }
  }

  return true;
}

int IpDevice::maintain_dhcp_lease() {
  // If we're using an IP address assigned via DHCP, renew the lease
  // periodically. The Ethernet library will do so at the appropriate interval
  // if we call it often enough.
  if (using_dhcp_) {
    return Ethernet.maintain();
  } else {
    return DHCP_CHECK_NONE;
  }
}

}  // namespace alpaca
