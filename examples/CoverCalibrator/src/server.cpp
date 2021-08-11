#include "server.h"

#include <Arduino.h>
#include <TinyAlpacaServer.h>

#include "cover_calibrator.h"
#include "led_channel_switch_group.h"

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
#define kServerName \
  "AstroMakers Cover Calibrator Server, based on Tiny Alpaca Server"
#define kManufacturer "Friends of AAVSO & ATMoB"
#define kManufacturerVersion "0.1"
#define kDeviceLocation "Earth Bound"

namespace astro_makers {
namespace {
using ::alpaca::DeviceInfo;
using ::alpaca::EDeviceType;
using ::mcucore::LiteralArray;

// No extra actions.
const auto kSupportedActions = mcucore::LiteralArray();

const DeviceInfo kCoverCalibratorDeviceInfo  // NOLINT
    {
        .device_type = EDeviceType::kCoverCalibrator,
        .device_number = 1,
        .name = TAS_FLASHSTR("Cover-Calibrator"),
        .unique_id = TAS_FLASHSTR("856cac35-7685-4a70-9bbf-be2b00f80af5"),
        .description = TAS_FLASHSTR("AstroMakers Cover Calibrator"),
        .driver_info =
            TAS_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
        .driver_version = TAS_FLASHSTR("0.1"),
        .supported_actions = kSupportedActions,
        .interface_version = 1,
    };

CoverCalibrator cover_calibrator(kCoverCalibratorDeviceInfo);  // NOLINT

const DeviceInfo kLedSwitchesDeviceInfo  // NOLINT
    {
        .device_type = EDeviceType::kSwitch,
        .device_number = 1,
        .name = TAS_FLASHSTR("Cover-Calibrator LED Channel Switches"),
        .unique_id = TAS_FLASHSTR("491c450a-0d1d-4f2b-9d28-5878e968e9df"),
        .description = TAS_FLASHSTR("AstroMakers Cover Calibrator Extension"),
        .driver_info =
            TAS_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
        .driver_version = TAS_FLASHSTR("0.1"),
        .supported_actions = kSupportedActions,
        .interface_version = 1,
    };

LedChannelSwitchGroup led_switches(  // NOLINT
    kLedSwitchesDeviceInfo, cover_calibrator);

// For responding to /management/v1/description
const alpaca::ServerDescription kServerDescription  // NOLINT
    {
        .server_name = TAS_FLASHSTR_128(kServerName),
        .manufacturer = TAS_FLASHSTR(kManufacturer),
        .manufacturer_version = TAS_FLASHSTR(kManufacturerVersion),
        .location = TAS_FLASHSTR(kDeviceLocation),
    };

alpaca::DeviceInterface* kDevices[] = {&cover_calibrator, &led_switches};

alpaca::TinyAlpacaServer tiny_alpaca_server(  // NOLINT
    /*tcp_port=*/80, kServerDescription, kDevices);

alpaca::IpDevice ip_device;

void announceAddresses() {
  Serial.println();
  alpaca::IpDevice::PrintNetworkAddresses();
  Serial.println();
}

}  // namespace

void setup() {
  mcucore::LogSink() << kServerDescription.server_name;
  mcucore::LogSink() << TAS_FLASHSTR("Initializing networking");
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

  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(nullptr): ") << sizeof(nullptr);
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(char*): ") << sizeof(char*);
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(short): ") << sizeof(short);  // NOLINT
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(int): ") << sizeof(int);
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(long): ") << sizeof(long);  // NOLINT
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(float): ") << sizeof(float);
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(double): ") << sizeof(double);
  TAS_VLOG(4) << TAS_FLASHSTR("sizeof(&setup): ") << sizeof(&setup);
}

void loop() {
  ip_device.MaintainDhcpLease();
  tiny_alpaca_server.PerformIO();
}

void logMCUStatusRegister(uint8_t mcusr) {
  mcucore::LogSink() << TAS_FLASHSTR("MCUSR: ") << mcucore::BaseHex << mcusr;
  if (TAS_VLOG_IS_ON(1)) {
    if (mcusr & _BV(JTRF)) {
      // JTAG Reset
      TAS_VLOG(1) << TAS_FLASHSTR("JTAG") << TAS_FLASHSTR(" reset occured");
    }
    if (mcusr & _BV(WDRF)) {
      // Watchdog Reset
      TAS_VLOG(1) << TAS_FLASHSTR("Watchdog") << TAS_FLASHSTR(" reset occured");
    }
    if (mcusr & _BV(BORF)) {
      // Brownout Reset
      TAS_VLOG(1) << TAS_FLASHSTR("Brownout") << TAS_FLASHSTR(" reset occured");
    }
    if (mcusr & _BV(EXTRF)) {
      // Reset button or otherwise some software reset
      TAS_VLOG(1) << TAS_FLASHSTR("External") << TAS_FLASHSTR(" reset occured");
    }
    if (mcusr & _BV(PORF)) {
      // Power On Reset
      TAS_VLOG(1) << TAS_FLASHSTR("Power-on") << TAS_FLASHSTR(" reset occured");
    }
  }
}

}  // namespace astro_makers
