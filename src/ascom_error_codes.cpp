#include "ascom_error_codes.h"

#include "literals.h"

namespace alpaca {

// Reserved error code (0x8004040C) to indicate that the requested action is
// not implemented in this driver.
Status ErrorCodes::ActionNotImplemented() {
  return Status(0x8004040C, Literals::ErrorActionNotImplemented());
}

// Reserved error code (0x8004040B) to indicate that the requested operation
// can not be undertaken at this time.
Status ErrorCodes::InvalidOperationException() {
  return Status(0x8004040B, Literals::ErrorInvalidOperationException());
}

// Reserved error code (0x80040401) for reporting an
// invalid value.
Status ErrorCodes::InvalidValue() {
  return Status(0x80040401, Literals::ErrorInvalidValue());
}

// Reserved error code (0x80040408) used to indicate that the attempted
// operation is invalid because the mount is currently in a Parked state.
Status ErrorCodes::InvalidWhileParked() {
  return Status(0x80040408, Literals::ErrorInvalidWhileParked());
}

// Reserved error code (0x80040409) used to indicate that the attempted
// operation is invalid because the mount is currently in a Slaved state.
Status ErrorCodes::InvalidWhileSlaved() {
  return Status(0x80040409, Literals::ErrorInvalidWhileSlaved());
}

// Reserved error code (0x80040407) used to indicate that the communications
// channel is not connected.
Status ErrorCodes::NotConnected() {
  return Status(0x80040407, Literals::ErrorNotConnected());
}

// Reserved error number (0x80040400) for property or method
// not implemented.
Status ErrorCodes::NotImplemented() {
  return Status(0x80040400, Literals::ErrorNotImplemented());
}

// Reserved error code (0x8004040D) to indicate that the requested item is not
// present in the ASCOM cache.
Status ErrorCodes::NotInCacheException() {
  return Status(0x8004040D, Literals::ErrorNotInCacheException());
}

// Reserved error code (0x8004040A) related to
// settings.
Status ErrorCodes::SettingsProviderError() {
  return Status(0x8004040A, Literals::ErrorSettingsProviderError());
}

// Reserved error code (0x80040402) for reporting that a value has not been
// set.
Status ErrorCodes::ValueNotSet() {
  return Status(0x80040402, Literals::ErrorValueNotSet());
}

// Reserved 'catch-all' error code (0x800404FF) used when nothing else was
// specified.
Status ErrorCodes::UnspecifiedError() {
  return Status(0x800404FF, Literals::ErrorUnspecifiedError());
}

}  // namespace alpaca
