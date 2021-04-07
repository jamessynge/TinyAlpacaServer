#include <Arduino.h>
#include <TinyAlpacaServer.h>

DhcpClass dhcp;
static alpaca::IpDevice ip_device;
static alpaca::TinyAlpacaDiscoveryServer discovery_server(80);  // NOLINT

void announceAddresses() {
  Serial.println();
  alpaca::IpDevice::PrintNetworkAddresses();
  Serial.println();
}

void announceFailure(const char* message) {
  while (true) {
    Serial.println(message);
    delay(1000);
  }
}

void setup() {
  // Setup serial, wait for it to be ready so that our logging messages can be
  // read.
  Serial.begin(9600);
  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost.
  while (!Serial) {
  }

  //////////////////////////////////////////////////////////////////////////////
  // Initialize networking.
  Ethernet.setDhcp(&dhcp);
  alpaca::Mega2560Eth::SetupW5500();

  // Provide an "Organizationally Unique Identifier" which will be used as the
  // first 3 bytes of the MAC addresses generated; this means that all boards
  // running this sketch will share the first 3 bytes of their MAC addresses,
  // which may help with locating them.
  alpaca::OuiPrefix oui_prefix(0x53, 0x75, 0x76);
  if (!ip_device.InitializeNetworking(&oui_prefix)) {
    announceFailure("Unable to initialize networking!");
  }

  announceAddresses();

  if (!discovery_server.Initialize()) {
    announceFailure("Unable to start listening for Alpaca Discovery messages!");
  }
}

void loop() {
  auto dhcp_check = ip_device.MaintainDhcpLease();
  switch (dhcp_check) {
    case DHCP_CHECK_NONE:
    case DHCP_CHECK_RENEW_OK:
      break;
    case DHCP_CHECK_RENEW_FAIL:
      Serial.println("DHCP_CHECK_RENEW_FAIL: Unable to renew the DHCP lease.");
      delay(1000);
      return;
    case DHCP_CHECK_REBIND_FAIL:
      Serial.println("DHCP_CHECK_REBIND_FAIL: Unable to get a new DHCP lease.");
      delay(1000);
      return;
    case DHCP_CHECK_REBIND_OK:
      announceAddresses();
      break;
    default:
      Serial.print("Unexpected result from MaintainDhcpLease: ");
      Serial.println(dhcp_check);
  }

  discovery_server.PerformIO();
}
