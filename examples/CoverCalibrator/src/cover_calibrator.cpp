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

TAS_DEFINE_LITERAL(IsNotEnabled, " is not enabled");
TAS_DEFINE_LITERAL(IsEnabled, " is enabled");

}  // namespace

CoverCalibrator::CoverCalibrator(const alpaca::DeviceInfo& device_info)
    : CoverCalibratorAdapter(device_info),
      led1_(TimerCounterChannel::B, kLedChannel1EnabledPin),
      led2_(TimerCounterChannel::C, kLedChannel2EnabledPin),
      led3_(TimerCounterChannel::A, kLedChannel3EnabledPin),
      led4_(TimerCounterChannel::A, kLedChannel4EnabledPin),
      cover_() {}

#define VLOG_ENABLEABLE_BY_PIN(level, name, enableable_by_pin)             \
  TAS_VLOG(level) << TAS_FLASHSTR(name)                                    \
                  << (enableable_by_pin.IsEnabled() ? IsEnabled()          \
                                                    : IsNotEnabled())      \
                  << TAS_FLASHSTR("; digitalRead(")                        \
                  << enableable_by_pin.enabled_pin() << TAS_FLASHSTR(")=") \
                  << enableable_by_pin.ReadPin()

void CoverCalibrator::Initialize() {
  CoverCalibratorAdapter::Initialize();

  pinMode(kLedChannel1PwmPin, OUTPUT);
  pinMode(kLedChannel2PwmPin, OUTPUT);
  pinMode(kLedChannel3PwmPin, OUTPUT);
  pinMode(kLedChannel4PwmPin, OUTPUT);

  // Fastest clock mode.
  TimerCounter3Initialize16BitFastPwm(alpaca::ClockPrescaling::kDivideBy1);
  TimerCounter4Initialize16BitFastPwm(alpaca::ClockPrescaling::kDivideBy1);

  calibrator_on_ = false;
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
  VLOG_ENABLEABLE_BY_PIN(1, "Cover Motor", cover_);
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
    if (!calibrator_on_) {
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

  if (!IsCalibratorHardwareEnabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  }
  if (brightness > kMaxBrightness) {
    return alpaca::ErrorCodes::InvalidValue();
  }
  calibrator_on_ = true;
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
  calibrator_on_ = false;
  return alpaca::OkStatus();
}

bool CoverCalibrator::SetLedChannelEnabled(int channel, bool enabled) {
  TAS_VLOG(4) << TAS_FLASHSTR("SetLedChannelEnabled(") << channel
              << TAS_FLASHSTR(", ") << enabled
              << TAS_FLASHSTR(") ENTER, brightness_ = ") << brightness_
              << TAS_FLASHSTR(", enabled_led_channels_ = ") << alpaca::BaseHex
              << enabled_led_channels_;

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

  TAS_VLOG(4) << TAS_FLASHSTR(
                     "SetLedChannelEnabled EXIT, GetLedChannelEnabled(")
              << channel << TAS_FLASHSTR(") = ")
              << GetLedChannelEnabled(channel)
              << TAS_FLASHSTR(", enabled_led_channels_ = ") << alpaca::BaseHex
              << enabled_led_channels_;
  return GetLedChannelEnabled(channel);
}

bool CoverCalibrator::IsCalibratorHardwareEnabled() const {
  return led1_.IsEnabled() || led2_.IsEnabled() || led3_.IsEnabled() ||
         led4_.IsEnabled();
}

bool CoverCalibrator::GetLedChannelEnabled(int channel) const {
  return GetLedChannelHardwareEnabled(channel) &&
         (enabled_led_channels_ & (1 << channel)) != 0;
}

bool CoverCalibrator::GetLedChannelHardwareEnabled(int channel) const {
  switch (channel) {
    case 0:
      return led1_.IsEnabled();
    case 1:
      return led2_.IsEnabled();
    case 2:
      return led3_.IsEnabled();
    case 3:
      return led4_.IsEnabled();
  }
  return false;
}

StatusOr<alpaca::ECoverStatus> CoverCalibrator::GetCoverState() {
  return cover_.GetCoverStatus();
}

Status CoverCalibrator::MoveCover(bool open) {
  if (!cover_.IsEnabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  } else if (open) {
    cover_.Open();
  } else {
    cover_.Close();
  }
  return alpaca::OkStatus();
}

Status CoverCalibrator::HaltCoverMotion() {
  if (!cover_.IsEnabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  } else {
    cover_.Halt();
  }
  return alpaca::OkStatus();
}

}  // namespace astro_makers
