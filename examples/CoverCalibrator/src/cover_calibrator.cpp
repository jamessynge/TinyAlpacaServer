#include "cover_calibrator.h"

#include <Arduino.h>

#include "constants.h"

namespace astro_makers {
namespace {
using ::alpaca::ECalibratorStatus;
using ::alpaca::ECoverStatus;
using ::alpaca::ErrorCodes;
using ::alpaca::Status;
using ::alpaca::StatusOr;
using ::alpaca::TimerCounterChannel;

constexpr MillisT kLedRampStepTime = 2;

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
TAS_DEFINE_LITERAL(CovCalName, "Cover-Calibrator");
TAS_DEFINE_LITERAL(CovCalDescription, "AstroMakers Cover Calibrator");
TAS_DEFINE_LITERAL(CovCalDriverInfo,
                   "https://github/jamessynge/TinyAlpacaServer");
TAS_DEFINE_LITERAL(CovCalDriverVersion, "0.1");
TAS_DEFINE_LITERAL(CovCalUniqueId, "856cac35-7685-4a70-9bbf-be2b00f80af5");

// No extra actions.
const auto kSupportedActions = alpaca::LiteralArray({
    // Someday add action(s) to allow client to specify the LED string (or
    // strings) to light up (e.g. Vis, IR, UV), maybe with actions such as
    // "EnableLed" and "DisableLed", with parameter(s) to specify the LED(s) to
    // be enabled or disabled.
});

const alpaca::DeviceInfo kDeviceInfo{
    .device_type = alpaca::EDeviceType::kCoverCalibrator,
    .device_number = 1,
    .name = CovCalName(),
    .unique_id = CovCalUniqueId(),
    .description = CovCalDescription(),
    .driver_info = CovCalDriverInfo(),
    .driver_version = CovCalDriverVersion(),
    .supported_actions = kSupportedActions,
    .interface_version = 1,
};

void InitializeDeviceEnabledPin(int pin, int mode = kDeviceEnabledPinMode) {
  if (pin != kNoSuchPin) {
    pinMode(pin, mode);
  }
}

bool GetDeviceEnabled(int pin, int mode = kDeviceEnabledPinMode) {
  if (pin != kNoSuchPin) {
    if (digitalRead(pin) == LOW) {
      return mode == INPUT_PULLUP;
    } else {
      return mode == INPUT;
    }
  }
  return true;
}

}  // namespace

CoverCalibrator::CoverCalibrator()
    : alpaca::CoverCalibratorAdapter(kDeviceInfo),
      led1_(TimerCounterChannel::A),  // OC3A for rev6.
      // led2_(TimerCounterChannel::C /*, kLedChannel2EnabledPin */),
      // led3_(TimerCounterChannel::A /*, kLedChannel3EnabledPin */),
      // led4_(TimerCounterChannel::A /*, kLedChannel4EnabledPin */),
      cover_() {}

void CoverCalibrator::Initialize() {
  alpaca::CoverCalibratorAdapter::Initialize();
  // TODO(jamessynge): Figure out what the initial position of the cover is,
  // OR always close it (maybe based on a choice by the end-user stored in
  // EEPROM).

  InitializeDeviceEnabledPin(kLedChannel1EnabledPin);
  // InitializeDeviceEnabledPin(kLedChannel2EnabledPin);
  // InitializeDeviceEnabledPin(kLedChannel3EnabledPin);
  // InitializeDeviceEnabledPin(kLedChannel4EnabledPin);

  pinMode(kLedChannel1PwmPin, OUTPUT);
  // pinMode(kLedChannel2PwmPin, OUTPUT);
  // pinMode(kLedChannel3PwmPin, OUTPUT);
  // pinMode(kLedChannel4PwmPin, OUTPUT);

  TimerCounter3Initialize16BitFastPwm(alpaca::ClockPrescaling::kDivideBy1);
  TimerCounter4Initialize16BitFastPwm(alpaca::ClockPrescaling::kDivideBy1);

  cover_.Initialize();
}

// Returns the current calibrator brightness. Not sure if this should be the
// target or the brightness we've most recently told the LEDs to be.
StatusOr<int32_t> CoverCalibrator::GetBrightness() {
  if (led1_.is_enabled()) {
    return led1_.get_pulse_count();
  } else {
    return alpaca::ErrorCodes::ActionNotImplemented();
  }
}

// Returns the state of the calibration device, or kUnknown if not overridden
// by a subclass.
StatusOr<ECalibratorStatus> CoverCalibrator::GetCalibratorState() {
  if (led1_.is_enabled()) {
    // We treat 0 as turning off the calibrator. Not sure if that is right.
    if (led1_.get_pulse_count() == 0) {
      return ECalibratorStatus::kOff;
    } else {
      return ECalibratorStatus::kReady;
    }
  } else {
    return ECalibratorStatus::kNotPresent;
  }
}

StatusOr<int32_t> CoverCalibrator::GetMaxBrightness() {
  return led1_.max_count();
}

Status CoverCalibrator::SetCalibratorBrightness(uint32_t brightness) {
  if (!led1_.is_enabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  }
  if (brightness > led1_.max_count()) {
    return alpaca::ErrorCodes::InvalidValue();
  }
  led1_.set_pulse_count(brightness);
  return alpaca::OkStatus();
}

Status CoverCalibrator::SetCalibratorOff() {
  if (!led1_.is_enabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  }
  led1_.set_pulse_count(0);
  return alpaca::OkStatus();
}

StatusOr<alpaca::ECoverStatus> CoverCalibrator::GetCoverState() {
  return cover_.GetCoverStatus();
}

Status CoverCalibrator::MoveCover(bool open) {
  if (!cover_.IsPresent()) {
    return alpaca::ErrorCodes::NotImplemented();
  } else if (open) {
    cover_.Open();
  } else {
    cover_.Close();
  }
  return alpaca::OkStatus();
}

Status CoverCalibrator::HaltCoverMotion() {
  if (!cover_.IsPresent()) {
    return alpaca::ErrorCodes::NotImplemented();
  } else {
    cover_.Halt();
  }
  return alpaca::OkStatus();
}

}  // namespace astro_makers
