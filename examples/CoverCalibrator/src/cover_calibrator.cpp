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
TAS_DEFINE_LITERAL(EnabledPinIs, " enabled pin is ");

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

}  // namespace

CoverCalibrator::CoverCalibrator()
    : alpaca::CoverCalibratorAdapter(kDeviceInfo),
      led1_(TimerCounterChannel::B, kLedChannel1EnabledPin),
      led2_(TimerCounterChannel::C, kLedChannel2EnabledPin),
      led3_(TimerCounterChannel::A, kLedChannel3EnabledPin),
      led4_(TimerCounterChannel::A, kLedChannel4EnabledPin),
      cover_() {}

#define VLOG_ENABLEABLE_BY_PIN(level, name, enableable_by_pin) \
  TAS_VLOG(level) << TASLIT(name) << EnabledPinIs()            \
                  << (enableable_by_pin.is_enabled() ? 1 : 0)

void CoverCalibrator::Initialize() {
  alpaca::CoverCalibratorAdapter::Initialize();

  pinMode(kLedChannel1PwmPin, OUTPUT);
  pinMode(kLedChannel2PwmPin, OUTPUT);
  pinMode(kLedChannel3PwmPin, OUTPUT);
  pinMode(kLedChannel4PwmPin, OUTPUT);

  // Fastest clock mode.
  TimerCounter3Initialize16BitFastPwm(alpaca::ClockPrescaling::kDivideBy1);
  TimerCounter4Initialize16BitFastPwm(alpaca::ClockPrescaling::kDivideBy1);

  brightness_ = 0;
  enabled_led_channels_ = 0xf;  // ALL.

  // TODO(jamessynge): Figure out what the initial position of the cover is,
  // OR always close it (maybe based on a choice by the end-user stored in
  // EEPROM).
  cover_.Initialize();

  // Announce enablement once only.

  VLOG_ENABLEABLE_BY_PIN(1, "LED #1", led1_);
  VLOG_ENABLEABLE_BY_PIN(1, "LED #2", led2_);
  VLOG_ENABLEABLE_BY_PIN(1, "LED #3", led3_);
  VLOG_ENABLEABLE_BY_PIN(1, "LED #4", led4_);
  VLOG_ENABLEABLE_BY_PIN(1, "Cover Motor ", cover_);
}

// Returns the current calibrator brightness that has been requested, but only
// if there are LED channels that are present (according to the hardware pins)..
// This does not take into account whether any of the LED is enabled by the
// switch feature.
StatusOr<int32_t> CoverCalibrator::GetBrightness() {
  if (IsCalibratorHardwareEnabled()) {
    return brightness_;
  }
  return alpaca::ErrorCodes::ActionNotImplemented();
}

// Returns the state of the calibration device, or kUnknown if not overridden
// by a subclass.
StatusOr<ECalibratorStatus> CoverCalibrator::GetCalibratorState() {
  if (IsCalibratorHardwareEnabled()) {
    // We treat 0 as turning off the calibrator. Not sure if that is right.
    if (brightness_ == 0) {
      return ECalibratorStatus::kOff;
    } else {
      return ECalibratorStatus::kReady;
    }
  } else {
    return ECalibratorStatus::kNotPresent;
  }
}

StatusOr<int32_t> CoverCalibrator::GetMaxBrightness() { return kMaxBrightness; }

Status CoverCalibrator::SetCalibratorBrightness(uint32_t brightness) {
  if (brightness == 0) {
    return SetCalibratorOff();
  }
  if (!IsCalibratorHardwareEnabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  }
  if (brightness > kMaxBrightness) {
    return alpaca::ErrorCodes::InvalidValue();
  }
  brightness_ = brightness;
  if (GetLedChannelEnabled(0)) {
    led1_.set_pulse_count(brightness_);
  }
  if (GetLedChannelEnabled(1)) {
    led2_.set_pulse_count(brightness_);
  }
  if (GetLedChannelEnabled(2)) {
    led3_.set_pulse_count(brightness_);
  }
  if (GetLedChannelEnabled(3)) {
    led4_.set_pulse_count(brightness_);
  }
  return alpaca::OkStatus();
}

Status CoverCalibrator::SetCalibratorOff() {
  if (!IsCalibratorHardwareEnabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  }
  led1_.set_pulse_count(0);
  led2_.set_pulse_count(0);
  led3_.set_pulse_count(0);
  led4_.set_pulse_count(0);
  brightness_ = 0;
  return alpaca::OkStatus();
}

bool CoverCalibrator::SetLedChannelEnabled(int channel, bool enabled) {
  if (0 <= channel && channel < 4) {
    if (enabled) {
      bitSet(enabled_led_channels_, channel);
    } else {
      bitClear(enabled_led_channels_, channel);
    }
    if (brightness_ > 0) {
      SetCalibratorBrightness(brightness_);
    }
  }
  return GetLedChannelEnabled(channel);
}

bool CoverCalibrator::IsCalibratorHardwareEnabled() const {
  return led1_.is_enabled() || led2_.is_enabled() || led3_.is_enabled() ||
         led4_.is_enabled();
}

bool CoverCalibrator::GetLedChannelEnabled(int channel) const {
  return GetLedChannelHardwareEnabled(channel) &&
         (enabled_led_channels_ & (1 << channel)) != 0;
}

bool CoverCalibrator::GetLedChannelHardwareEnabled(int channel) const {
  switch (channel) {
    case 0:
      return led1_.is_enabled();
    case 1:
      return led2_.is_enabled();
    case 2:
      return led3_.is_enabled();
    case 3:
      return led4_.is_enabled();
  }
  return false;
}

StatusOr<alpaca::ECoverStatus> CoverCalibrator::GetCoverState() {
  return cover_.GetCoverStatus();
}

Status CoverCalibrator::MoveCover(bool open) {
  if (!cover_.is_enabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  } else if (open) {
    cover_.Open();
  } else {
    cover_.Close();
  }
  return alpaca::OkStatus();
}

Status CoverCalibrator::HaltCoverMotion() {
  if (!cover_.is_enabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  } else {
    cover_.Halt();
  }
  return alpaca::OkStatus();
}

}  // namespace astro_makers
