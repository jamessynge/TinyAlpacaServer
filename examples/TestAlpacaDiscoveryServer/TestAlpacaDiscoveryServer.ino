#include <Arduino.h>
#include <McuCore.h>
#include <McuNet.h>
#include <TinyAlpacaServer.h>

static mcunet::IpDevice ip_device;
static alpaca::TinyAlpacaDiscoveryServer discovery_server(80);  // NOLINT

void announceAddresses() {
  Serial.println();
  mcunet::IpDevice::PrintNetworkAddresses();
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
  Serial.begin(115200);
  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost.
  while (!Serial) {
  }

  //////////////////////////////////////////////////////////////////////////////
  // Initialize networking.
  mcunet::Mega2560Eth::SetupW5500();

  // Get an EepromTlv instance, to be used for persistence of settings.
  auto eeprom_tlv = mcucore::EepromTlv::GetOrDie();

  // Initialize the pseudo-random number generator with a random number
  // generated based on clock jitter.
  mcucore::JitterRandom::setRandomSeed();

  // Provide an "Organizationally Unique Identifier" which will be used as the
  // first 3 bytes of the MAC addresses generated; this means that all boards
  // running this sketch will share the first 3 bytes of their MAC addresses,
  // which may help with locating them.
  mcunet::OuiPrefix oui_prefix(0x53, 0x75, 0x76);
  MCU_CHECK_OK(ip_device.InitializeNetworking(eeprom_tlv, &oui_prefix));

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
