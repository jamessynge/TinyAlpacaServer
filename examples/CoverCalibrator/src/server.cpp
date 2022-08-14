#include "server.h"

#include <Arduino.h>
#include <McuCore.h>
#include <McuNet.h>
#include <TinyAlpacaServer.h>

#include "cover_calibrator.h"
#include "led_channel_switch_group.h"

MCU_DEFINE_NAMED_DOMAIN(CoverCalibrator, 17);
MCU_DEFINE_NAMED_DOMAIN(LedChannelSwitch, 18);

namespace astro_makers {
namespace {
using ::alpaca::DeviceDescription;
using ::alpaca::EDeviceType;

// Shared context provided to devices.
alpaca::ServerContext server_context;  // NOLINT

const DeviceDescription kCoverCalibratorDeviceDescription  // NOLINT
    {
        .device_type = EDeviceType::kCoverCalibrator,
        .device_number = 0,
        .domain = MCU_DOMAIN(CoverCalibrator),
        .name = MCU_FLASHSTR("Cover-Calibrator"),
        .description = MCU_FLASHSTR("AstroMakers Cover Calibrator"),
        .driver_info =
            MCU_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
        .driver_version = MCU_FLASHSTR("0.2"),
        .supported_actions = {},  // No extra actions.
    };

CoverCalibrator cover_calibrator(  // NOLINT
    server_context, kCoverCalibratorDeviceDescription);

const DeviceDescription kLedSwitchesDeviceDescription  // NOLINT
    {
        .device_type = EDeviceType::kSwitch,
        .device_number = 0,
        .domain = MCU_DOMAIN(LedChannelSwitch),
        .name = MCU_FLASHSTR("Cover-Calibrator LED Channel Switches"),
        .description = MCU_FLASHSTR("AstroMakers Cover Calibrator Extension"),
        .driver_info =
            MCU_FLASHSTR("https://github/jamessynge/TinyAlpacaServer"),
        .driver_version = MCU_FLASHSTR("0.2"),
        .supported_actions = {},  // No extra actions.
    };

LedChannelSwitchGroup led_switches(  // NOLINT
    server_context, kLedSwitchesDeviceDescription, cover_calibrator);

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
    server_context, kServerDescription, kDevices);

alpaca::TinyAlpacaNetworkServer network_server(device_server);  // NOLINT

mcunet::IpDevice ip_device;

void announceAddresses() {
  Serial.println();
  mcunet::IpDevice::PrintNetworkAddresses();
  Serial.println();
}

}  // namespace

void setup() {
  // This is first so we can (if necessary) turn off any device that turns on
  // automatically when the microcontroller is reset.
  device_server.ValidateAndReset();

  mcucore::LogSink() << '\n' << kServerDescription.server_name;
  mcucore::LogSink() << kServerDescription.manufacturer << MCU_PSD(", version ")
                     << kServerDescription.manufacturer_version;

  mcucore::LogSink() << MCU_PSD("Initializing networking");
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
  device_server.InitializeForServing();

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
  mcucore::LogSink() << MCU_PSD("MCUSR: ") << mcucore::BaseHex << mcusr;
  if (MCU_VLOG_IS_ON(1)) {
    if (mcusr & _BV(JTRF)) {
      // JTAG Reset
      MCU_VLOG(1) << MCU_PSD("JTAG") << MCU_PSD(" reset occured");
    }
    if (mcusr & _BV(WDRF)) {
      // Watchdog Reset
      MCU_VLOG(1) << MCU_PSD("Watchdog") << MCU_PSD(" reset occured");
    }
    if (mcusr & _BV(BORF)) {
      // Brownout Reset
      MCU_VLOG(1) << MCU_PSD("Brownout") << MCU_PSD(" reset occured");
    }
    if (mcusr & _BV(EXTRF)) {
      // Reset button or otherwise some software reset
      MCU_VLOG(1) << MCU_PSD("External") << MCU_PSD(" reset occured");
    }
    if (mcusr & _BV(PORF)) {
      // Power On Reset
      MCU_VLOG(1) << MCU_PSD("Power-on") << MCU_PSD(" reset occured");
    }
  }
}

}  // namespace astro_makers
