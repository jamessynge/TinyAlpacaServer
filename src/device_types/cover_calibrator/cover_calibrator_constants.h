#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_COVER_CALIBRATOR_COVER_CALIBRATOR_CONSTANTS_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_COVER_CALIBRATOR_COVER_CALIBRATOR_CONSTANTS_H_

// ASCOM constants specified in the Cover Calibrator device API.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#if MCU_HOST_TARGET
#include <ostream>  // pragma: keep standard include
#endif

namespace alpaca {

// CalibratorStatus Enumeration - Describes the state of a calibration device.
// https://ascom-standards.org/Help/Developer/html/T_ASCOM_DeviceInterface_CalibratorStatus.htm
using ECalibratorStatus_UnderlyingType = uint8_t;
enum class ECalibratorStatus : ECalibratorStatus_UnderlyingType {
  kNotPresent = 0,  // This device does not have a calibration capability
  kOff = 1,         // The calibrator is off
  kNotReady =
      2,  // The calibrator is stabilising or is not yet in the commanded state
  kReady = 3,    // The calibrator is ready for use
  kUnknown = 4,  // The calibrator state is unknown
  kError = 5,    // The calibrator encountered an error when changing state
};

// CoverStatus Enumeration - Describes the state of a telescope cover.
// https://ascom-standards.org/Help/Developer/html/T_ASCOM_DeviceInterface_CoverStatus.htm
using ECoverStatus_UnderlyingType = uint8_t;
enum class ECoverStatus : ECoverStatus_UnderlyingType {
  kNotPresent =
      0,  // This device does not have a cover that can be closed independently
  kClosed = 1,   // The cover is closed
  kMoving = 2,   // The cover is moving to a new position
  kOpen = 3,     // The cover is open
  kUnknown = 4,  // The state of the cover is unknown
  kError = 5,    // The device encountered an error when changing state
};

}  // namespace alpaca

// BEGIN_HEADER_GENERATED_BY_MAKE_ENUM_TO_STRING

namespace alpaca {

const __FlashStringHelper* ToFlashStringHelper(ECalibratorStatus v);
const __FlashStringHelper* ToFlashStringHelper(ECoverStatus v);

size_t PrintValueTo(ECalibratorStatus v, Print& out);
size_t PrintValueTo(ECoverStatus v, Print& out);

#if MCU_HOST_TARGET
// Support for debug logging of enums.
std::ostream& operator<<(std::ostream& os, ECalibratorStatus v);
std::ostream& operator<<(std::ostream& os, ECoverStatus v);
#endif  // MCU_HOST_TARGET

}  // namespace alpaca

// END_HEADER_GENERATED_BY_MAKE_ENUM_TO_STRING

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_COVER_CALIBRATOR_COVER_CALIBRATOR_CONSTANTS_H_
