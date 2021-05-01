#include "cover_calibrator.h"

#if 0
// Based on AM_CoverCalibrator_schematic_rev_5_pcb.pdf, which has more detail
// than rev 6, though in rev 6 there is apparently no LED 1 enabled pin.

#define kLedChannel1PwmPin 5
#define kLedChannel1EnabledPin 9
#define kLedChannel1RampStepMicros 1500

#define kLedChannel2PwmPin 6
#define kLedChannel2EnabledPin 10
#define kLedChannel2RampStepMicros 1500

#define kLedChannel3PwmPin 7
#define kLedChannel3EnabledPin 11
#define kLedChannel3RampStepMicros 1500

#define kLedChannel4PwmPin 8
#define kLedChannel4EnabledPin 12
#define kLedChannel4RampStepMicros 1500

#define kCoverMotorStepPin 3
#define kCoverMotorDirectionPin 4
#define kCoverOpenLimitPin 20
#define kCoverCloseLimitPin 21
#define kCoverEnabledPin 13

#else
// Modified pin selection to avoid pins used for other purposes.
// Pins we should (or must) avoid on the Robotdyn Mega ETH:
// D00 - RX (Serial over USB)
// D01 - TX (Serial over USB)
// D04 - SDcard Chip Select
// D09 - SDcard Detect
// D10 - W5500 Chip Select
// D13 - Built-in LED
// D50 - MISO (SPI)
// D51 - MOSI (SPI)
// D52 - SCK (SPI)

#define kLedChannel1PwmPin 5           // OC3A      unchanged
#define kLedChannel2PwmPin 6           // OC4A      unchanged
#define kLedChannel2EnabledPin PIN_A1  //           was 10
#define kLedChannel3PwmPin 7           // OC4B      unchanged
#define kLedChannel3EnabledPin PIN_A2  //           was 11
#define kLedChannel4PwmPin 8           // OC4C      unchanged
#define kLedChannel4EnabledPin PIN_A3  //           was 12
#define kCoverMotorStepPin 3           //           unchanged
#define kCoverMotorDirectionPin 5      //           unchanged
#define kCoverOpenLimitPin 20          //           unchanged
#define kCoverCloseLimitPin 21         //           unchanged
#define kCoverEnabledPin PIN_A4        //           was 13

#endif

#define kLedChannel1RampStepMicros 1500

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
                   "https://github/jamessynge/tiny-alpaca-server");
TAS_DEFINE_LITERAL(CovCalDriverVersion, "0.1");
TAS_DEFINE_LITERAL(CovCalUniqueId, "856cac35-7685-4a70-9bbf-be2b00f80af5");

// No extra actions.
const auto kSupportedActions = alpaca::LiteralArray({
    // Some day add action(s) to allow client to specify the LED string (or
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
      led1_(TimerCounterChannel::A),
      led2_(TimerCounterChannel::A /*, kLedChannel2EnabledPin */),
      led3_(TimerCounterChannel::B /*, kLedChannel3EnabledPin */),
      led4_(TimerCounterChannel::C /*, kLedChannel4EnabledPin */),
      cover_(kCoverMotorStepPin, kCoverMotorDirectionPin, kCoverOpenLimitPin,
             kCoverCloseLimitPin, kCoverEnabledPin) {}

void CoverCalibrator::Initialize() {
  alpaca::CoverCalibratorAdapter::Initialize();
  // TODO(jamessynge): Figure out what the initial position of the cover is,
  // OR always close it (maybe based on a choice by the end-user stored in
  // EEPROM).

  pinMode(kLedChannel1PwmPin, OUTPUT);
  pinMode(kLedChannel2PwmPin, OUTPUT);
  pinMode(kLedChannel3PwmPin, OUTPUT);
  pinMode(kLedChannel4PwmPin, OUTPUT);

  TimerCounter3Initialize16BitFastPwm(alpaca::ClockPrescaling::kAsIs);
  TimerCounter4Initialize16BitFastPwm(alpaca::ClockPrescaling::kAsIs);
}

void CoverCalibrator::MaintainDevice() {
  alpaca::CoverCalibratorAdapter::MaintainDevice();

  // Just in case we've fallen behind with updates, we give the stepper a chance
  // to take multiple steps.
  while (cover_.MoveStepper()) {
  }
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
  return cover_.GetStatus();
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
