#include "device_types/cover_calibrator/cover_calibrator_constants.h"

// (Mostly) GENERATED FILE. See make_enum_to_string.py

// BEGIN_SOURCE_GENERATED_BY_MAKE_ENUM_TO_STRING

#include <McuCore.h>

// Must come after mcucore_platform.h so that MCU_HOST_TARGET is defined.
#if MCU_HOST_TARGET
#include <ostream>      // pragma: keep standard include
#include <string_view>  // pragma: keep standard include
#endif

#include <McuCore.h>

namespace alpaca {
namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(ECalibratorStatus v) {
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
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(ECalibratorStatus v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
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
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(ECalibratorStatus::kNotPresent ==
                static_cast<ECalibratorStatus>(0));
  static_assert(ECalibratorStatus::kOff == static_cast<ECalibratorStatus>(1));
  static_assert(ECalibratorStatus::kNotReady ==
                static_cast<ECalibratorStatus>(2));
  static_assert(ECalibratorStatus::kReady == static_cast<ECalibratorStatus>(3));
  static_assert(ECalibratorStatus::kUnknown ==
                static_cast<ECalibratorStatus>(4));
  static_assert(ECalibratorStatus::kError == static_cast<ECalibratorStatus>(5));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("NotPresent"),  // 0: kNotPresent
      MCU_PSD("Off"),         // 1: kOff
      MCU_PSD("NotReady"),    // 2: kNotReady
      MCU_PSD("Ready"),       // 3: kReady
      MCU_PSD("Unknown"),     // 4: kUnknown
      MCU_PSD("Error"),       // 5: kError
  );
  return mcucore::LookupFlashStringForDenseEnum<
      ECalibratorStatus_UnderlyingType>(flash_string_table,
                                        ECalibratorStatus::kNotPresent,
                                        ECalibratorStatus::kError, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(ECoverStatus v) {
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
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(ECoverStatus v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
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
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(ECoverStatus::kNotPresent == static_cast<ECoverStatus>(0));
  static_assert(ECoverStatus::kClosed == static_cast<ECoverStatus>(1));
  static_assert(ECoverStatus::kMoving == static_cast<ECoverStatus>(2));
  static_assert(ECoverStatus::kOpen == static_cast<ECoverStatus>(3));
  static_assert(ECoverStatus::kUnknown == static_cast<ECoverStatus>(4));
  static_assert(ECoverStatus::kError == static_cast<ECoverStatus>(5));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("NotPresent"),  // 0: kNotPresent
      MCU_PSD("Closed"),      // 1: kClosed
      MCU_PSD("Moving"),      // 2: kMoving
      MCU_PSD("Open"),        // 3: kOpen
      MCU_PSD("Unknown"),     // 4: kUnknown
      MCU_PSD("Error"),       // 5: kError
  );
  return mcucore::LookupFlashStringForDenseEnum<ECoverStatus_UnderlyingType>(
      flash_string_table, ECoverStatus::kNotPresent, ECoverStatus::kError, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
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
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, ECoverStatus v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

#endif  // MCU_HOST_TARGET

}  // namespace alpaca

// END_SOURCE_GENERATED_BY_MAKE_ENUM_TO_STRING
