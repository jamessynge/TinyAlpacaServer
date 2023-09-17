#include "extras/futures/setup_and_loop.h"

#include <McuCore.h>

namespace alpaca {

SetupAndLoop::SetupAndLoop(TinyAlpacaDeviceServer& device_server)
    : device_server_(device_server),
      network_server_(device_server),
      // Get an EepromTlv instance, to be used for persistence of settings.
      eeprom_tlv_(mcucore::EepromTlv::GetOrDie()) {}

void SetupAndLoop::Setup(mcunet::OuiPrefix oui_prefix) {
  mcucore::LogSink() << device_server_.server_description().server_name;
  mcucore::LogSink() << MCU_FLASHSTR("Initializing networking");
  mcunet::Mega2560Eth::SetupW5500();

  // Initialize the pseudo-random number generator with a random number
  // generated based on clock jitter.
  mcucore::JitterRandom::setRandomSeed();

  // Provide an "Organizationally Unique Identifier" which will be used as the
  // first 3 bytes of the MAC addresses generated; this means that all boards
  // running this sketch will share the first 3 bytes of their MAC addresses,
  // which may help with locating them.
  mcunet::OuiPrefix oui_prefix(0x53, 0x75, 0x76);

  MCU_CHECK_OK(ip_device_.InitializeNetworking(eeprom_tlv_, &oui_prefix));
  Serial.println();
  mcunet::IpDevice::PrintNetworkAddresses();
  Serial.println();

  // Initialize Tiny Alpaca Device Server, which will initialize sensors, etc.
  device_server_.Initialize(eeprom_tlv_);

  // Initialize Tiny Alpaca Network Server, which will initialize TCP listeners.
  network_server_.Initialize(eeprom_tlv_);
}

void SetupAndLoop::Loop() {
  ip_device_.MaintainDhcpLease();
  device_server_.MaintainDevices();
  network_server_.PerformIO();
}

}  // namespace alpaca
