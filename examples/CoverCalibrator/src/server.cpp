#include "server.h"

#include <Arduino.h>
#include <McuCore.h>
#include <McuNet.h>
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
        .name = MCU_FLASHSTR("Cover-Calibrator"),
        .unique_id = MCU_FLASHSTR("856cac35-7685-4a70-9bbf-be2b00f80af5"),
        .description = MCU_FLASHSTR("AstroMakers Cover Calibrator"),
        .driver_info =
            MCU_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
        .driver_version = MCU_FLASHSTR("0.1"),
        .supported_actions = kSupportedActions,
        .interface_version = 1,
    };

CoverCalibrator cover_calibrator(kCoverCalibratorDeviceInfo);  // NOLINT

const DeviceInfo kLedSwitchesDeviceInfo  // NOLINT
    {
        .device_type = EDeviceType::kSwitch,
        .device_number = 1,
        .name = MCU_FLASHSTR("Cover-Calibrator LED Channel Switches"),
        .unique_id = MCU_FLASHSTR("491c450a-0d1d-4f2b-9d28-5878e968e9df"),
        .description = MCU_FLASHSTR("AstroMakers Cover Calibrator Extension"),
        .driver_info =
            MCU_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
        .driver_version = MCU_FLASHSTR("0.1"),
        .supported_actions = kSupportedActions,
        .interface_version = 1,
    };

LedChannelSwitchGroup led_switches(  // NOLINT
    kLedSwitchesDeviceInfo, cover_calibrator);

// For responding to /management/v1/description
const alpaca::ServerDescription kServerDescription  // NOLINT
    {
        .server_name = MCU_FLASHSTR_128(kServerName),
        .manufacturer = MCU_FLASHSTR(kManufacturer),
        .manufacturer_version = MCU_FLASHSTR(kManufacturerVersion),
        .location = MCU_FLASHSTR(kDeviceLocation),
    };

alpaca::DeviceInterface* kDevices[] = {&cover_calibrator, &led_switches};

alpaca::TinyAlpacaServer tiny_alpaca_server(  // NOLINT
    /*tcp_port=*/80, kServerDescription, kDevices);

mcunet::IpDevice ip_device;

void announceAddresses() {
  Serial.println();
  mcunet::IpDevice::PrintNetworkAddresses();
  Serial.println();
}

}  // namespace

void setup() {
  mcucore::LogSink() << kServerDescription.server_name;
  mcucore::LogSink() << MCU_FLASHSTR("Initializing networking");
  mcunet::Mega2560Eth::SetupW5500();

  // Provide an "Organizationally Unique Identifier" which will be used as the
  // first 3 bytes of the MAC addresses generated; this means that all boards
  // running this sketch will share the first 3 bytes of their MAC addresses,
  // which may help with locating them.
  mcunet::OuiPrefix oui_prefix(0x53, 0x75, 0x76);
  MCU_CHECK(ip_device.InitializeNetworking(&oui_prefix))
      << MCU_FLASHSTR("Unable to initialize networking!");
  announceAddresses();

  // Initialize Tiny Alpaca Server, which will initialize TCP listeners.
  tiny_alpaca_server.Initialize();

  MCU_VLOG(4) << MCU_FLASHSTR("sizeof(nullptr): ") << sizeof(nullptr);
  MCU_VLOG(4) << MCU_FLASHSTR("sizeof(char*): ") << sizeof(char*);
  MCU_VLOG(4) << MCU_FLASHSTR("sizeof(short): ") << sizeof(short);  // NOLINT
  MCU_VLOG(4) << MCU_FLASHSTR("sizeof(int): ") << sizeof(int);
  MCU_VLOG(4) << MCU_FLASHSTR("sizeof(long): ") << sizeof(long);  // NOLINT
  MCU_VLOG(4) << MCU_FLASHSTR("sizeof(float): ") << sizeof(float);
  MCU_VLOG(4) << MCU_FLASHSTR("sizeof(double): ") << sizeof(double);
  MCU_VLOG(4) << MCU_FLASHSTR("sizeof(&setup): ") << sizeof(&setup);
}

void loop() {
  ip_device.MaintainDhcpLease();
  tiny_alpaca_server.PerformIO();
}

void logMCUStatusRegister(uint8_t mcusr) {
  mcucore::LogSink() << MCU_FLASHSTR("MCUSR: ") << mcucore::BaseHex << mcusr;
  if (MCU_VLOG_IS_ON(1)) {
    if (mcusr & _BV(JTRF)) {
      // JTAG Reset
      MCU_VLOG(1) << MCU_FLASHSTR("JTAG") << MCU_FLASHSTR(" reset occured");
    }
    if (mcusr & _BV(WDRF)) {
      // Watchdog Reset
      MCU_VLOG(1) << MCU_FLASHSTR("Watchdog") << MCU_FLASHSTR(" reset occured");
    }
    if (mcusr & _BV(BORF)) {
      // Brownout Reset
      MCU_VLOG(1) << MCU_FLASHSTR("Brownout") << MCU_FLASHSTR(" reset occured");
    }
    if (mcusr & _BV(EXTRF)) {
      // Reset button or otherwise some software reset
      MCU_VLOG(1) << MCU_FLASHSTR("External") << MCU_FLASHSTR(" reset occured");
    }
    if (mcusr & _BV(PORF)) {
      // Power On Reset
      MCU_VLOG(1) << MCU_FLASHSTR("Power-on") << MCU_FLASHSTR(" reset occured");
    }
  }
}

}  // namespace astro_makers
