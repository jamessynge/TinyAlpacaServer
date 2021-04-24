#include "cover_calibrator.h"

#include "alpaca_request.h"
#include "ascom_error_codes.h"
#include "device_types/cover_calibrator/cover_calibrator_adapter.h"
#include "device_types/cover_calibrator/cover_calibrator_constants.h"
#include "utils/status.h"

#if 0
// Based on AM_CoverCalibrator_schematic_rev_5_pcb.pdf (which has more detail
// than rev 6).

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
// Pins to avoid:
// D00 - RX (Serial over USB)
// D01 - TX (Serial over USB)
// D04 - SDcard Chip Select
// D10 - W5500 Chip Select
// D13 - Built-in LED
// D50 - MISO (SPI)
// D51 - MOSI (SPI)
// D52 - SCK (SPI)

#define kLedChannel1PwmPin 46      // OC5A
#define kLedChannel1EnabledPin 43  // PL6

#define kLedChannel2PwmPin 45      // OC5B
#define kLedChannel2EnabledPin 41  // PG0

#define kLedChannel3PwmPin 44      // OC5C
#define kLedChannel3EnabledPin 39  // PG2

#define kLedChannel4PwmPin 12      // OC1B
#define kLedChannel4EnabledPin 37  // A8

#define kCoverMotorStepPin 3
#define kCoverMotorDirectionPin 5
#define kCoverOpenLimitPin 6    // PCINT8
#define kCoverCloseLimitPin 11  // PCINT5
#define kCoverEnabledPin 12

#endif

namespace astro_makers {
namespace {
using ::alpaca::ECalibratorStatus;
using ::alpaca::ECoverStatus;
using ::alpaca::ErrorCodes;
using ::alpaca::Status;
using ::alpaca::StatusOr;

constexpr MillisT kLedRampStepTime = 2;

// Define some literals, which get stored in PROGMEM (in the case of AVR chips).
TAS_DEFINE_LITERAL(CovCalName, "Cover-Calibrator");
TAS_DEFINE_LITERAL(CovCalDescription, "AstroMakers Cover Calibrator");
TAS_DEFINE_LITERAL(CovCalDriverInfo,
                   "https://github/jamessynge/tiny-alpaca-server");
TAS_DEFINE_LITERAL(CovCalDriverVersion, "0.1");
TAS_DEFINE_LITERAL(CovCalUniqueId, "856cac35-7685-4a70-9bbf-be2b00f80af5");

// No extra actions.
const auto kSupportedActions = alpaca::LiteralArray({});

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
      white_led_(kLedChannel1PwmPin, kLedChannel1EnabledPin,
                 kLedChannel1RampStepMicros),
      cover_(kCoverMotorStepPin, kCoverMotorDirectionPin, kCoverOpenLimitPin,
             kCoverCloseLimitPin, kCoverEnabledPin) {}

void CoverCalibrator::Initialize() {
  alpaca::CoverCalibratorAdapter::Initialize();
  // TODO(jamessynge): Figure out what the initial position of the cover is,
  // OR always close it (maybe based on a choice by the end-user stored in
  // EEPROM).
}

void CoverCalibrator::MaintainDevice() {
  alpaca::CoverCalibratorAdapter::MaintainDevice();
  white_led_.MaintainDevice();

  // Just in case we've fallen behind with updates, we give the stepper a chance
  // to take multiple steps.
  while (cover_.MoveStepper()) {
  }
}

// Returns the current calibrator brightness. Not sure if this should be the
// target or the brightness we've most recently told the LEDs to be.
StatusOr<int32_t> CoverCalibrator::GetBrightness() {
  if (white_led_.is_enabled()) {
    return white_led_.current_brightness();
  } else {
    return alpaca::ErrorCodes::ActionNotImplemented();
  }
}

// Returns the state of the calibration device, or kUnknown if not overridden
// by a subclass.
StatusOr<ECalibratorStatus> CoverCalibrator::GetCalibratorState() {
  if (white_led_.has_reached_target()) {
    // We treat 0 as turning off the calibrator. Not sure if that is right.
    if (white_led_.current_brightness() == 0) {
      return ECalibratorStatus::kOff;
    } else {
      return ECalibratorStatus::kReady;
    }
  } else {
    return ECalibratorStatus::kNotReady;
  }
}

StatusOr<int32_t> CoverCalibrator::GetMaxBrightness() {
  return white_led_.max_brightness();
}

Status CoverCalibrator::SetCalibratorBrightness(uint32_t brightness) {
  if (!white_led_.is_enabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  }
  if (brightness > white_led_.max_brightness()) {
    return alpaca::ErrorCodes::InvalidValue();
  }
  white_led_.set_brightness_target(brightness);
  return alpaca::OkStatus();
}

Status CoverCalibrator::SetCalibratorOff() {
  if (!white_led_.is_enabled()) {
    return alpaca::ErrorCodes::NotImplemented();
  }
  white_led_.set_brightness_immediately(0);
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
