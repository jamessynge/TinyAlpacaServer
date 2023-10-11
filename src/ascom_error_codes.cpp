// Error codes defined by ASCOM.
// See: https://ascom-standards.org/Help/Developer/html/T_ASCOM_ErrorCodes.htm

#include "ascom_error_codes.h"

#include <McuCore.h>

#include "literals.h"

namespace alpaca {

mcucore::Status ErrorCodes::NotImplemented() {
  return mcucore::Status(kNotImplementedStatusCode,
                         ProgmemStringViews::ErrorNotImplemented());
}

mcucore::Status ErrorCodes::InvalidValue() {
  return mcucore::Status(kInvalidValueStatusCode,
                         ProgmemStringViews::ErrorInvalidValue());
}

mcucore::Status ErrorCodes::ValueNotSet() {
  return mcucore::Status(kValueNotSetStatusCode,
                         ProgmemStringViews::ErrorValueNotSet());
}

mcucore::Status ErrorCodes::NotConnected() {
  return mcucore::Status(kNotConnectedStatusCode,
                         ProgmemStringViews::ErrorNotConnected());
}

mcucore::Status ErrorCodes::InvalidWhileParked() {
  return mcucore::Status(kInvalidWhileParkedStatusCode,
                         ProgmemStringViews::ErrorInvalidWhileParked());
}

mcucore::Status ErrorCodes::InvalidWhileSlaved() {
  return mcucore::Status(kInvalidWhileSlavedStatusCode,
                         ProgmemStringViews::ErrorInvalidWhileSlaved());
}

mcucore::Status ErrorCodes::SettingsProviderError() {
  return mcucore::Status(kSettingsProviderErrorStatusCode,
                         ProgmemStringViews::ErrorSettingsProviderError());
}

mcucore::Status ErrorCodes::InvalidOperation() {
  return mcucore::Status(kInvalidOperationStatusCode,
                         ProgmemStringViews::InvalidOperation());
}

mcucore::Status ErrorCodes::ActionNotImplemented() {
  return mcucore::Status(kActionNotImplementedStatusCode,
                         ProgmemStringViews::ErrorActionNotImplemented());
}

mcucore::Status ErrorCodes::NotInCacheException() {
  return mcucore::Status(kNotInCacheExceptionStatusCode,
                         ProgmemStringViews::ErrorNotInCacheException());
}

mcucore::Status ErrorCodes::UnspecifiedError() {
  return mcucore::Status(kUnspecifiedErrorStatusCode,
                         ProgmemStringViews::ErrorUnspecifiedError());
}

}  // namespace alpaca
