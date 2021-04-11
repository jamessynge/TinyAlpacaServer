#include "ascom_error_codes.h"

#include "literals.h"

namespace alpaca {

Status ErrorCodes::NotImplemented() {
  return Status(kNotImplemented, Literals::ErrorNotImplemented());
}

Status ErrorCodes::InvalidValue() {
  return Status(kInvalidValue, Literals::ErrorInvalidValue());
}

Status ErrorCodes::ValueNotSet() {
  return Status(kValueNotSet, Literals::ErrorValueNotSet());
}

Status ErrorCodes::NotConnected() {
  return Status(kNotConnected, Literals::ErrorNotConnected());
}

Status ErrorCodes::InvalidWhileParked() {
  return Status(kInvalidWhileParked, Literals::ErrorInvalidWhileParked());
}

Status ErrorCodes::InvalidWhileSlaved() {
  return Status(kInvalidWhileSlaved, Literals::ErrorInvalidWhileSlaved());
}

Status ErrorCodes::InvalidOperation() {
  return Status(kInvalidOperation, Literals::InvalidOperation());
}

Status ErrorCodes::ActionNotImplemented() {
  return Status(kActionNotImplemented, Literals::ErrorActionNotImplemented());
}

// Status ErrorCodes::NotInCacheException() {
//   return Status(0x40D, Literals::ErrorNotInCacheException());
// }

// Status ErrorCodes::SettingsProviderError() {
//   return Status(0x40A, Literals::ErrorSettingsProviderError());
// }

// Status ErrorCodes::UnspecifiedError() {
//   return Status(0x4FF, Literals::ErrorUnspecifiedError());
// }

}  // namespace alpaca
