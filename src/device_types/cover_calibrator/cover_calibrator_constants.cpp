
#include "device_types/cover_calibrator/cover_calibrator_constants.h"

#include "inline_literal.h"
#include "mcucore_platform.h"
#include "print_misc.h"
#include "progmem_string_data.h"

namespace alpaca {

const __FlashStringHelper* ToFlashStringHelper(ECalibratorStatus v) {
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

size_t PrintValueTo(ECalibratorStatus v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("ECalibratorStatus"),
                                          static_cast<uint32_t>(v), out);
}

const __FlashStringHelper* ToFlashStringHelper(ECoverStatus v) {
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

size_t PrintValueTo(ECoverStatus v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("ECoverStatus"),
                                          static_cast<uint32_t>(v), out);
}

// Define stuff

}  // namespace alpaca
