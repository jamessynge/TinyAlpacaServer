#include "cover_calibrator.h"

#include <Arduino.h>
#include <McuCore.h>

#include "constants.h"

namespace astro_makers {

using ::alpaca::ECalibratorStatus;
using ::alpaca::TimerCounterChannel;
using ::mcucore::StatusOr;

CoverCalibrator::CoverCalibrator(const alpaca::DeviceInfo& device_info)
    : CoverCalibratorAdapter(device_info),
      led1_(TimerCounterChannel::B, kLedChannel1EnabledPin),
      led2_(TimerCounterChannel::C, kLedChannel2EnabledPin),
      led3_(TimerCounterChannel::A, kLedChannel3EnabledPin),
      led4_(TimerCounterChannel::A, kLedChannel4EnabledPin),
      cover_() {}

#define VLOG_ENABLEABLE_BY_PIN(level, name, enableable_by_pin)        \
  MCU_VLOG(level) << MCU_PSD(name)                                    \
                  << (enableable_by_pin.IsEnabled()                   \
                          ? MCU_FLASHSTR(" is enabled")               \
                          : MCU_FLASHSTR(" is not enabled"))          \
                  << MCU_PSD("; digitalRead(")                        \
                  << enableable_by_pin.enabled_pin() << MCU_PSD(")=") \
                  << enableable_by_pin.ReadPin()

void CoverCalibrator::ResetHardware() { cover_.ResetHardware(); }

void CoverCalibrator::InitializeDevice() {
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
  cover_.InitializeHardware();

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
mcucore::StatusOr<int32_t> CoverCalibrator::GetBrightness() {
  if (IsCalibratorHardwareEnabled()) {
    return brightness_;
  }
  return alpaca::ErrorCodes::ActionNotImplemented();
}

// Returns the state of the calibration device, or kUnknown if not overridden
// by a subclass.
mcucore::StatusOr<ECalibratorStatus> CoverCalibrator::GetCalibratorState() {
  if (!IsCalibratorHardwareEnabled()) {
    return ECalibratorStatus::kNotPresent;
  } else if (!calibrator_on_) {
    return ECalibratorStatus::kOff;
  } else {
    return ECalibratorStatus::kReady;
  }
}

mcucore::StatusOr<int32_t> CoverCalibrator::GetMaxBrightness() {
  return kMaxBrightness;
}

mcucore::Status CoverCalibrator::SetCalibratorBrightness(uint32_t brightness) {
  if (!IsCalibratorHardwareEnabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  }
  if (brightness > kMaxBrightness) {
    return alpaca::ErrorCodes::InvalidValue();
  }
  calibrator_on_ = true;
  brightness_ = brightness;
  if (GetLedChannelEnabled(0)) {
    MCU_VLOG_IF(1, !led1_.IsEnabled()) << MCU_PSD("led1 not enabled!");
    led1_.set_pulse_count(brightness_);
  }
  if (GetLedChannelEnabled(1)) {
    MCU_VLOG_IF(1, !led2_.IsEnabled()) << MCU_PSD("led2 not enabled!");
    led2_.set_pulse_count(brightness_);
  }
  if (GetLedChannelEnabled(2)) {
    MCU_VLOG_IF(1, !led3_.IsEnabled()) << MCU_PSD("led3 not enabled!");
    led3_.set_pulse_count(brightness_);
  }
  if (GetLedChannelEnabled(3)) {
    MCU_VLOG_IF(1, !led4_.IsEnabled()) << MCU_PSD("led4 not enabled!");
    led4_.set_pulse_count(brightness_);
  }
  return mcucore::OkStatus();
}

mcucore::Status CoverCalibrator::SetCalibratorOff() {
  if (!IsCalibratorHardwareEnabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  }
  led1_.set_pulse_count(0);
  led2_.set_pulse_count(0);
  led3_.set_pulse_count(0);
  led4_.set_pulse_count(0);
  brightness_ = 0;
  calibrator_on_ = false;
  return mcucore::OkStatus();
}

bool CoverCalibrator::SetLedChannelEnabled(int channel, bool enabled) {
  MCU_VLOG(2) << MCU_PSD("SetLedChannelEnabled(") << channel << MCU_PSD(", ")
              << enabled << MCU_PSD(") ENTER, brightness_ = ") << brightness_
              << MCU_PSD(", enabled_led_channels_ = ") << mcucore::BaseHex
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

  MCU_VLOG(4) << MCU_PSD("SetLedChannelEnabled EXIT, GetLedChannelEnabled(")
              << channel << MCU_PSD(") = ") << GetLedChannelEnabled(channel)
              << MCU_PSD(", enabled_led_channels_ = ") << mcucore::BaseHex
              << enabled_led_channels_;
  return GetLedChannelEnabled(channel);
}

bool CoverCalibrator::IsCalibratorHardwareEnabled() const {
  return led1_.IsEnabled() || led2_.IsEnabled() || led3_.IsEnabled() ||
         led4_.IsEnabled();
}

bool CoverCalibrator::GetLedChannelEnabled(int channel) const {
  if (!GetLedChannelHardwareEnabled(channel)) {
    return false;
  }
  const auto mask = 1 << channel;
  return (enabled_led_channels_ & mask) != 0;
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

mcucore::StatusOr<alpaca::ECoverStatus> CoverCalibrator::GetCoverState() {
  return cover_.GetCoverStatus();
}

mcucore::Status CoverCalibrator::MoveCover(bool open) {
  if (!cover_.IsEnabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  } else if (open) {
    cover_.Open();
  } else {
    cover_.Close();
  }
  return mcucore::OkStatus();
}

mcucore::Status CoverCalibrator::HaltCoverMotion() {
  if (!cover_.IsEnabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  } else {
    cover_.Halt();
  }
  return mcucore::OkStatus();
}

}  // namespace astro_makers
