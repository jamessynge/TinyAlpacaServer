#include "server.h"

#include <Arduino.h>
#include <TinyAlpacaServer.h>

#include "cover_calibrator.h"
#include "led_channel_switch_group.h"

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
TAS_DEFINE_LITERAL(
    ServerName,
    "AstroMakers Cover Calibrator Server, based on Tiny Alpaca Server");
TAS_DEFINE_LITERAL(Manufacturer, "Friends of AAVSO & ATMoB");
TAS_DEFINE_LITERAL(ManufacturerVersion, "0.1");
TAS_DEFINE_LITERAL(DeviceLocation, "Earth Bound");

namespace astro_makers {
namespace {
using ::alpaca::DeviceInfo;
using ::alpaca::EDeviceType;
using ::alpaca::LiteralArray;

// No extra actions.
const auto kSupportedActions = LiteralArray();

TAS_DEFINE_LITERAL(GithubRepoLink,
                   "https://github/jamessynge/TinyAlpacaServer");
TAS_DEFINE_LITERAL(DriverVersion, "0.1");

TAS_DEFINE_LITERAL(CovCalName, "Cover-Calibrator");
TAS_DEFINE_LITERAL(CovCalDescription, "AstroMakers Cover Calibrator");
TAS_DEFINE_LITERAL(CovCalUniqueId, "856cac35-7685-4a70-9bbf-be2b00f80af5");

const DeviceInfo kCoverCalibratorDeviceInfo{
    .device_type = EDeviceType::kCoverCalibrator,
    .device_number = 1,
    .name = CovCalName(),
    .unique_id = CovCalUniqueId(),
    .description = CovCalDescription(),
    .driver_info = GithubRepoLink(),
    .driver_version = DriverVersion(),
    .supported_actions = kSupportedActions,
    .interface_version = 1,
};

CoverCalibrator cover_calibrator(kCoverCalibratorDeviceInfo);  // NOLINT

TAS_DEFINE_LITERAL(LedSwitchesName, "Cover-Calibrator LED Channel Switches");
TAS_DEFINE_LITERAL(LedSwitchesDescription,
                   "AstroMakers Cover Calibrator Extension");
TAS_DEFINE_LITERAL(LedSwitchesUniqueId, "491c450a-0d1d-4f2b-9d28-5878e968e9df");

const DeviceInfo kLedSwitchesDeviceInfo{
    .device_type = EDeviceType::kSwitch,
    .device_number = 1,
    .name = LedSwitchesName(),
    .unique_id = LedSwitchesUniqueId(),
    .description = LedSwitchesDescription(),
    .driver_info = GithubRepoLink(),
    .driver_version = DriverVersion(),
    .supported_actions = kSupportedActions,
    .interface_version = 1,
};

LedChannelSwitchGroup led_switches(  // NOLINT
    kLedSwitchesDeviceInfo, cover_calibrator);

constexpr alpaca::ServerDescription kServerDescription{
    .server_name = ServerName(),
    .manufacturer = Manufacturer(),
    .manufacturer_version = ManufacturerVersion(),
    .location = DeviceLocation(),
};

alpaca::DeviceInterface* kDevices[] = {&cover_calibrator, &led_switches};

alpaca::TinyAlpacaServer tiny_alpaca_server(  // NOLINT
    /*tcp_port=*/80, kServerDescription, kDevices);

// We allocate a DhcpClass at file scope so that it isn't dynamically allocated
// at setup time (i.e. so we're in better control of memory consumption).
::DhcpClass dhcp;

alpaca::IpDevice ip_device;

void announceAddresses() {
  Serial.println();
  alpaca::IpDevice::PrintNetworkAddresses();
  Serial.println();
}

}  // namespace

void setup() {
  TAS_VLOG(1) << TAS_FLASHSTR("Initializing networking");
  Ethernet.setDhcp(&dhcp);
  alpaca::Mega2560Eth::SetupW5500();

  // Provide an "Organizationally Unique Identifier" which will be used as the
  // first 3 bytes of the MAC addresses generated; this means that all boards
  // running this sketch will share the first 3 bytes of their MAC addresses,
  // which may help with locating them.
  alpaca::OuiPrefix oui_prefix(0x53, 0x75, 0x76);
  TAS_CHECK(ip_device.InitializeNetworking(&oui_prefix))
      << TAS_FLASHSTR("Unable to initialize networking!");
  announceAddresses();

  // Initialize Tiny Alpaca Server, which will initialize TCP listeners.
  tiny_alpaca_server.Initialize();

  TAS_VLOG(1) << TAS_FLASHSTR("sizeof(nullptr): ") << sizeof(nullptr);
  TAS_VLOG(1) << TAS_FLASHSTR("sizeof(char*): ") << sizeof(char*);
  TAS_VLOG(1) << TAS_FLASHSTR("sizeof(short): ") << sizeof(short);  // NOLINT
  TAS_VLOG(1) << TAS_FLASHSTR("sizeof(int): ") << sizeof(int);
  TAS_VLOG(1) << TAS_FLASHSTR("sizeof(long): ") << sizeof(long);  // NOLINT
  TAS_VLOG(1) << TAS_FLASHSTR("sizeof(float): ") << sizeof(float);
  TAS_VLOG(1) << TAS_FLASHSTR("sizeof(double): ") << sizeof(double);
  TAS_VLOG(1) << TAS_FLASHSTR("sizeof(&setup): ") << sizeof(&setup);
}

void loop() {
  ip_device.MaintainDhcpLease();
  tiny_alpaca_server.PerformIO();
}

void logMCUStatusRegister(uint8_t mcusr) {
  TAS_VLOG(1) << TAS_FLASHSTR("MCUSR: ") << alpaca::BaseHex << mcusr;
  if (mcusr & _BV(JTRF)) {
    // JTAG Reset
    TAS_VLOG(1) << TAS_FLASHSTR("JTAG") << TASLIT(" reset occured");
  }
  if (mcusr & _BV(WDRF)) {
    // Watchdog Reset
    TAS_VLOG(1) << TAS_FLASHSTR("Watchdog") << TASLIT(" reset occured");
  }
  if (mcusr & _BV(BORF)) {
    // Brownout Reset
    TAS_VLOG(1) << TAS_FLASHSTR("Brownout") << TASLIT(" reset occured");
  }
  if (mcusr & _BV(EXTRF)) {
    // Reset button or otherwise some software reset
    TAS_VLOG(1) << TAS_FLASHSTR("External") << TASLIT(" reset occured");
  }
  if (mcusr & _BV(PORF)) {
    // Power On Reset
    TAS_VLOG(1) << TAS_FLASHSTR("Power-on") << TASLIT(" reset occured");
  }
}

}  // namespace astro_makers
