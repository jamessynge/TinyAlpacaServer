// Error codes defined by ASCOM.
// See: https://ascom-standards.org/Help/Developer/html/T_ASCOM_ErrorCodes.htm

#include "ascom_error_codes.h"

#include "literals.h"

namespace alpaca {

mcucore::Status ErrorCodes::NotImplemented() {
  return mcucore::Status(kNotImplemented,
                         ProgmemStringViews::ErrorNotImplemented());
}

mcucore::Status ErrorCodes::InvalidValue() {
  return mcucore::Status(kInvalidValue,
                         ProgmemStringViews::ErrorInvalidValue());
}

mcucore::Status ErrorCodes::ValueNotSet() {
  return mcucore::Status(kValueNotSet, ProgmemStringViews::ErrorValueNotSet());
}

mcucore::Status ErrorCodes::NotConnected() {
  return mcucore::Status(kNotConnected,
                         ProgmemStringViews::ErrorNotConnected());
}

mcucore::Status ErrorCodes::InvalidWhileParked() {
  return mcucore::Status(kInvalidWhileParked,
                         ProgmemStringViews::ErrorInvalidWhileParked());
}

mcucore::Status ErrorCodes::InvalidWhileSlaved() {
  return mcucore::Status(kInvalidWhileSlaved,
                         ProgmemStringViews::ErrorInvalidWhileSlaved());
}

mcucore::Status ErrorCodes::SettingsProviderError() {
  return mcucore::Status(0x40A,
                         ProgmemStringViews::ErrorSettingsProviderError());
}

mcucore::Status ErrorCodes::InvalidOperation() {
  return mcucore::Status(kInvalidOperation,
                         ProgmemStringViews::InvalidOperation());
}

mcucore::Status ErrorCodes::ActionNotImplemented() {
  return mcucore::Status(kActionNotImplemented,
                         ProgmemStringViews::ErrorActionNotImplemented());
}

mcucore::Status ErrorCodes::NotInCacheException() {
  return mcucore::Status(kNotInCacheException,
                         ProgmemStringViews::ErrorNotInCacheException());
}

mcucore::Status ErrorCodes::UnspecifiedError() {
  return mcucore::Status(kUnspecifiedError,
                         ProgmemStringViews::ErrorUnspecifiedError());
}

}  // namespace alpaca
