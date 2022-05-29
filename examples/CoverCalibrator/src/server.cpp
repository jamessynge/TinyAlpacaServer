#include "server.h"

#include <Arduino.h>
#include <McuCore.h>
#include <McuNet.h>
#include <TinyAlpacaServer.h>

#include "cover_calibrator.h"
#include "led_channel_switch_group.h"

namespace astro_makers {
namespace {
using ::alpaca::DeviceInfo;
using ::alpaca::EDeviceType;

// No extra actions.
const auto kSupportedActions = mcucore::ProgmemStringArray();

// TODO(jamessynge): Replace field unique_id with a `EepromDomain device_domain`
// field, i.e. one that must be unique for each device of a single server.
// TODO(jamessynge): Use the device_domain to store and retrieve a UUID for
// each device.

const DeviceInfo kCoverCalibratorDeviceInfo  // NOLINT
    {
        .device_type = EDeviceType::kCoverCalibrator,
        .device_number = 1,
        .name = MCU_FLASHSTR("Cover-Calibrator"),
        .unique_id = MCU_FLASHSTR("856cac35-7685-4a70-9bbf-be2b00f80af5"),
        .description = MCU_FLASHSTR("AstroMakers Cover Calibrator"),
        .driver_info =
            MCU_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
        .driver_version = MCU_FLASHSTR("0.2"),
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
        .driver_version = MCU_FLASHSTR("0.2"),
        .supported_actions = kSupportedActions,
        .interface_version = 1,
    };

LedChannelSwitchGroup led_switches(  // NOLINT
    kLedSwitchesDeviceInfo, cover_calibrator);

// For responding to /management/v1/description
const alpaca::ServerDescription kServerDescription  // NOLINT
    {
        .server_name = MCU_FLASHSTR_128(
            "AstroMakers Cover Calibrator Server, based on Tiny Alpaca Server"),
        .manufacturer = MCU_FLASHSTR("Friends of AAVSO & ATMoB, 2022-05-29"),
        .manufacturer_version = MCU_FLASHSTR("0.2"),
        .location = MCU_FLASHSTR("Earth Bound"),
    };

alpaca::DeviceInterface* kDevices[] = {&cover_calibrator, &led_switches};

alpaca::TinyAlpacaDeviceServer device_server(  // NOLINT
    kServerDescription, kDevices);

alpaca::TinyAlpacaNetworkServer network_server(device_server);  // NOLINT

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

  // Initialize Tiny Alpaca Device Server, which will initialize sensors, etc.
  device_server.Initialize();

  // Initialize Tiny Alpaca Network Server, which will initialize TCP listeners
  // and the Alpaca Discovery Server.
  network_server.Initialize();
}

void loop() {
  ip_device.MaintainDhcpLease();
  device_server.MaintainDevices();
  network_server.PerformIO();
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
