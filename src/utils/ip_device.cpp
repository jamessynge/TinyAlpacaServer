#include "utils/ip_device.h"

#include "utils/inline_literal.h"
#include "utils/logging.h"

namespace alpaca {
namespace {
constexpr uint8_t kW5500ChipSelectPin = 10;
constexpr uint8_t kW5500ResetPin = 7;
constexpr uint8_t kSDcardSelectPin = 4;
}  // namespace

// static
void Mega2560Eth::SetupW5500(uint8_t max_sock_num) {
  // Make sure that the SD Card interface is not the selected SPI device.
#if kSDcardSelectPin != 4
  pinMode(kSDcardSelectPin, OUTPUT);
  digitalWrite(kSDcardSelectPin, HIGH);
#else
  TAS_VLOG(1) << FLASHSTR("SetupW5500 applying fix for pin ")
              << kSDcardSelectPin
              << FLASHSTR(", replacing with PE7 (pin 9 of ATmega2560)");

  // Set PE7 (Port E, bit 7) to output HIGH. As described in section 13.2.3 of
  // the ATmega2560 datasheet, we can't transition directly from tri-state to
  // output HIGH, so we swap the order of setting the bits, as suggested in the
  // datasheet.
  bitSet(PORTE, PE7);  // Set to output high.
  bitSet(DDRE, DDE7);  // Set data direction to output.
#endif

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
  // Load the addresses saved to EEPROM, if they were previously saved. If
  // they were not successfully loaded, then generate them and save them into
  // the EEPROM.
  Addresses addresses;
  addresses.loadOrGenAndSave(oui_prefix);

  Serial.print(TASLIT("MAC: "));
  Serial.println(addresses.mac);
  Serial.print(TASLIT("Default IP: "));
  Serial.println(addresses.ip);

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
      Serial.println(TASLIT("Found no hardware"));
      return false;
    }
    Serial.println(TASLIT("No DHCP"));

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
  Serial.print(TASLIT("MAC: "));
  Serial.println(mac);
  Serial.print(TASLIT("IP: "));
  Serial.println(Ethernet.localIP());
  Serial.print(TASLIT("Subnet: "));
  Serial.println(Ethernet.subnetMask());
  Serial.print(TASLIT("Gateway: "));
  Serial.println(Ethernet.gatewayIP());
  Serial.print(TASLIT("DNS: "));
  Serial.println(Ethernet.dnsServerIP());
}

}  // namespace alpaca
