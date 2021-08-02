
#include "device_types/cover_calibrator/cover_calibrator_constants.h"

#include "experimental/users/jamessynge/arduino/mcucore/src/inline_literal.h"
#include "experimental/users/jamessynge/arduino/mcucore/src/mcucore_platform.h"
#include "utils/print_misc.h"

namespace alpaca {

const __FlashStringHelper* ToFlashStringHelper(ECalibratorStatus v) {
  switch (v) {
    case ECalibratorStatus::kNotPresent:
      return TAS_FLASHSTR("NotPresent");
    case ECalibratorStatus::kOff:
      return TAS_FLASHSTR("Off");
    case ECalibratorStatus::kNotReady:
      return TAS_FLASHSTR("NotReady");
    case ECalibratorStatus::kReady:
      return TAS_FLASHSTR("Ready");
    case ECalibratorStatus::kUnknown:
      return TAS_FLASHSTR("Unknown");
    case ECalibratorStatus::kError:
      return TAS_FLASHSTR("Error");
  }
  return nullptr;
}

size_t PrintValueTo(ECalibratorStatus v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("ECalibratorStatus"),
                                 static_cast<uint32_t>(v), out);
}

const __FlashStringHelper* ToFlashStringHelper(ECoverStatus v) {
  switch (v) {
    case ECoverStatus::kNotPresent:
      return TAS_FLASHSTR("NotPresent");
    case ECoverStatus::kClosed:
      return TAS_FLASHSTR("Closed");
    case ECoverStatus::kMoving:
      return TAS_FLASHSTR("Moving");
    case ECoverStatus::kOpen:
      return TAS_FLASHSTR("Open");
    case ECoverStatus::kUnknown:
      return TAS_FLASHSTR("Unknown");
    case ECoverStatus::kError:
      return TAS_FLASHSTR("Error");
  }
  return nullptr;
}

size_t PrintValueTo(ECoverStatus v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("ECoverStatus"),
                                 static_cast<uint32_t>(v), out);
}

// Define stuff

}  // namespace alpaca
