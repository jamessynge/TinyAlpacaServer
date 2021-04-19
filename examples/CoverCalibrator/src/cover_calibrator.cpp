#include "src/cover_calibrator.h"

#include "alpaca_request.h"
#include "device_types/cover_calibrator/cover_calibrator_adapter.h"
#include "device_types/cover_calibrator/cover_calibrator_constants.h"

// Based on AM_CoverCalibrator_schematic_rev_5_pcb.pdf (which has more detail
// than rev 6).

#define kLedChannel1PwmPin 5
#define kLedChannel1EnabledPin 9

#define kLedChannel2PwmPin 6
#define kLedChannel2EnabledPin 10

#define kLedChannel3PwmPin 7
#define kLedChannel3EnabledPin 11

#define kLedChannel4PwmPin 8
#define kLedChannel4EnabledPin 12

#define kCoverMotorStepPin 3
#define kCoverMotorDirectionPin 4
#define kCoverOpenLimitPin 20
#define kCoverCloseLimitPin 21
#define kCoverEnabledPin 13

namespace astro_makers {
namespace {
using alpaca::AlpacaRequest;
using alpaca::ECalibratorStatus;
using alpaca::ECoverStatus;
using alpaca::Status;
using alpaca::StatusOr;

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
      led1_(kLedChannel1PwmPin, kLedChannel1EnabledPin),
      cover_(kCoverMotorStepPin, kCoverMotorDirectionPin, kCoverOpenLimitPin,
             kCoverCloseLimitPin, kCoverEnabledPin) {}

void CoverCalibrator::Initialize() {
  alpaca::CoverCalibratorAdapter::Initialize();
  // TODO(jamessynge): Figure out what that initial position of the cover is,
  // OR always close it. It's best if we have a position sensor and limit
  // switches.
}

void CoverCalibrator::Update() {
  alpaca::CoverCalibratorAdapter::Update();
  while (current_brightness_ != brightness_target_) {
    const MillisT now = millis();
    const auto elapsed = now - last_change_ms_;
    if (elapsed < kLedRampStepTime) {
      break;
    }
    AdjustCurrentBrightness();
    last_change_ms_ += kLedRampStepTime;
  }
  // TODO(jamessynge): Handle the stepper motor updates.
}

// Returns the current calibrator brightness. Not sure if this should be the
// target or the brightness we've most recently told the LEDs to be.
StatusOr<int32_t> CoverCalibrator::GetBrightness() {
  return current_brightness_;
}

// Returns the state of the calibration device, or kUnknown if not overridden
// by a subclass.
StatusOr<ECalibratorStatus> CoverCalibrator::GetCalibratorState() {
  if (current_brightness_ == brightness_target_) {
    // We treat 0 as turning off the calibrator. Not sure if that is right.
    if (current_brightness_ == 0) {
      return ECalibratorStatus::kOff;
    } else {
      return ECalibratorStatus::kReady;
    }
  } else {
    return ECalibratorStatus::kNotReady;
  }
}

StatusOr<int32_t> CoverCalibrator::GetMaxBrightness() { return 255; }

Status CoverCalibrator::SetBrightness(uint32_t brightness) {
  TAS_DCHECK_LT(brightness, 256);
  if (brightness_target_ != brightness) {
    if (current_brightness_ == brightness_target_) {
      last_change_ms_ = millis();
    }
    brightness_target_ = brightness;
  }
  return alpaca::OkStatus();
}

void CoverCalibrator::AdjustCurrentBrightness() {
  current_brightness_ += (current_brightness_ < brightness_target_) ? 1 : -1;
  analogWrite(led5Pin, current_brightness_);
  analogWrite(led6Pin, current_brightness_);
  analogWrite(led7Pin, current_brightness_);
  analogWrite(led8Pin, current_brightness_);
}

// bool CoverCalibrator::HandlePutCloseCover(const AlpacaRequest& request,
//                                           Print& out) {}
// bool CoverCalibrator::HandlePutHaltCover(const AlpacaRequest& request,
//                                          Print& out) {}
// bool CoverCalibrator::HandlePutOpenCover(const AlpacaRequest& request,
//                                          Print& out) {}

}  // namespace astro_makers
