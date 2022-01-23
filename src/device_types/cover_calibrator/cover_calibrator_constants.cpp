#include "device_types/cover_calibrator/cover_calibrator_constants.h"

// (Mostly) GENERATED FILE. See make_enum_to_string.py

#include "print_misc.h"
#include "progmem_string_data.h"

// BEGIN_SOURCE_GENERATED_BY_MAKE_ENUM_TO_STRING

namespace alpaca {

const __FlashStringHelper* ToFlashStringHelper(ECalibratorStatus v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case ECalibratorStatus::kNotPresent:
      return MCU_FLASHSTR("NotPresent");
    case ECalibratorStatus::kOff:
      return MCU_FLASHSTR("Off");
    case ECalibratorStatus::kNotReady:
      return MCU_FLASHSTR("NotReady");
    case ECalibratorStatus::kReady:
      return MCU_FLASHSTR("Ready");
    case ECalibratorStatus::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case ECalibratorStatus::kError:
      return MCU_FLASHSTR("Error");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == ECalibratorStatus::kNotPresent) {
    return MCU_FLASHSTR("NotPresent");
  }
  if (v == ECalibratorStatus::kOff) {
    return MCU_FLASHSTR("Off");
  }
  if (v == ECalibratorStatus::kNotReady) {
    return MCU_FLASHSTR("NotReady");
  }
  if (v == ECalibratorStatus::kReady) {
    return MCU_FLASHSTR("Ready");
  }
  if (v == ECalibratorStatus::kUnknown) {
    return MCU_FLASHSTR("Unknown");
  }
  if (v == ECalibratorStatus::kError) {
    return MCU_FLASHSTR("Error");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(ECoverStatus v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case ECoverStatus::kNotPresent:
      return MCU_FLASHSTR("NotPresent");
    case ECoverStatus::kClosed:
      return MCU_FLASHSTR("Closed");
    case ECoverStatus::kMoving:
      return MCU_FLASHSTR("Moving");
    case ECoverStatus::kOpen:
      return MCU_FLASHSTR("Open");
    case ECoverStatus::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case ECoverStatus::kError:
      return MCU_FLASHSTR("Error");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == ECoverStatus::kNotPresent) {
    return MCU_FLASHSTR("NotPresent");
  }
  if (v == ECoverStatus::kClosed) {
    return MCU_FLASHSTR("Closed");
  }
  if (v == ECoverStatus::kMoving) {
    return MCU_FLASHSTR("Moving");
  }
  if (v == ECoverStatus::kOpen) {
    return MCU_FLASHSTR("Open");
  }
  if (v == ECoverStatus::kUnknown) {
    return MCU_FLASHSTR("Unknown");
  }
  if (v == ECoverStatus::kError) {
    return MCU_FLASHSTR("Error");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

size_t PrintValueTo(ECalibratorStatus v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("ECalibratorStatus"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(ECoverStatus v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("ECoverStatus"),
                                          static_cast<uint32_t>(v), out);
}

#if MCU_HOST_TARGET
// Support for debug logging of enums.

std::ostream& operator<<(std::ostream& os, ECalibratorStatus v) {
  switch (v) {
    case ECalibratorStatus::kNotPresent:
      return os << "NotPresent";
    case ECalibratorStatus::kOff:
      return os << "Off";
    case ECalibratorStatus::kNotReady:
      return os << "NotReady";
    case ECalibratorStatus::kReady:
      return os << "Ready";
    case ECalibratorStatus::kUnknown:
      return os << "Unknown";
    case ECalibratorStatus::kError:
      return os << "Error";
  }
  return os << "Unknown ECalibratorStatus, value=" << static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& os, ECoverStatus v) {
  switch (v) {
    case ECoverStatus::kNotPresent:
      return os << "NotPresent";
    case ECoverStatus::kClosed:
      return os << "Closed";
    case ECoverStatus::kMoving:
      return os << "Moving";
    case ECoverStatus::kOpen:
      return os << "Open";
    case ECoverStatus::kUnknown:
      return os << "Unknown";
    case ECoverStatus::kError:
      return os << "Error";
  }
  return os << "Unknown ECoverStatus, value=" << static_cast<int64_t>(v);
}

#endif  // MCU_HOST_TARGET

}  // namespace alpaca

// END_SOURCE_GENERATED_BY_MAKE_ENUM_TO_STRING
