// Error codes defined by ASCOM.
// See: https://ascom-standards.org/Help/Developer/html/T_ASCOM_ErrorCodes.htm

#include "ascom_error_codes.h"

#include "literals.h"

namespace alpaca {

mcucore::Status ErrorCodes::NotImplemented() {
  return mcucore::Status(kNotImplemented, Literals::ErrorNotImplemented());
}

mcucore::Status ErrorCodes::InvalidValue() {
  return mcucore::Status(kInvalidValue, Literals::ErrorInvalidValue());
}

mcucore::Status ErrorCodes::ValueNotSet() {
  return mcucore::Status(kValueNotSet, Literals::ErrorValueNotSet());
}

mcucore::Status ErrorCodes::NotConnected() {
  return mcucore::Status(kNotConnected, Literals::ErrorNotConnected());
}

mcucore::Status ErrorCodes::InvalidWhileParked() {
  return mcucore::Status(kInvalidWhileParked,
                         Literals::ErrorInvalidWhileParked());
}

mcucore::Status ErrorCodes::InvalidWhileSlaved() {
  return mcucore::Status(kInvalidWhileSlaved,
                         Literals::ErrorInvalidWhileSlaved());
}

mcucore::Status ErrorCodes::SettingsProviderError() {
  return mcucore::Status(0x40A, Literals::ErrorSettingsProviderError());
}

mcucore::Status ErrorCodes::InvalidOperation() {
  return mcucore::Status(kInvalidOperation, Literals::InvalidOperation());
}

mcucore::Status ErrorCodes::ActionNotImplemented() {
  return mcucore::Status(kActionNotImplemented,
                         Literals::ErrorActionNotImplemented());
}

mcucore::Status ErrorCodes::NotInCacheException() {
  return mcucore::Status(kNotInCacheException,
                         Literals::ErrorNotInCacheException());
}

mcucore::Status ErrorCodes::UnspecifiedError() {
  return mcucore::Status(kUnspecifiedError, Literals::ErrorUnspecifiedError());
}

}  // namespace alpaca
