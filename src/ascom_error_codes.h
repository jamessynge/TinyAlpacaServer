#ifndef TINY_ALPACA_SERVER_SRC_ASCOM_ERROR_CODES_H_
#define TINY_ALPACA_SERVER_SRC_ASCOM_ERROR_CODES_H_

// Error codes defined by ASCOM.
// See: https://ascom-standards.org/Help/Developer/html/T_ASCOM_ErrorCodes.htm

#include "utils/platform.h"
#include "utils/status.h"

namespace alpaca {

class ErrorCodes {
 public:
  // Reserved error code (0x8004040C) to indicate that the requested action is
  // not implemented in this driver.
  static Status ActionNotImplemented();

  // Reserved error code(0x8004040B) to indicate that the requested operation
  // can not be undertaken at this time.
  static Status InvalidOperationException();

  // Reserved error code (0x80040401) for reporting an invalid value.
  static Status InvalidValue();

  // Reserved error code (0x80040408) used to indicate that the attempted
  // operation is invalid because the mount is currently in a Parked state.
  static Status InvalidWhileParked();

  // Reserved error code (0x80040409) used to indicate that the attempted
  // operation is invalid because the mount is currently in a Slaved state.
  static Status InvalidWhileSlaved();

  // Reserved error code (0x80040407) used to indicate that the communications
  // channel is not connected.
  static Status NotConnected();

  // Reserved error number (0x80040400) for property or method not implemented.
  static Status NotImplemented();

  // Reserved error code (0x8004040D) to indicate that the requested item is not
  // present in the ASCOM cache.
  static Status NotInCacheException();

  // Reserved error code (0x8004040A) related to settings.
  static Status SettingsProviderError();

  // Reserved 'catch-all' error code (0x800404FF) used when nothing else was
  // specified.
  static Status UnspecifiedError();

  // Reserved error code (0x80040402) for reporting that a value has not been
  // set.
  static Status ValueNotSet();
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ASCOM_ERROR_CODES_H_
