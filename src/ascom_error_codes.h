#ifndef TINY_ALPACA_SERVER_SRC_ASCOM_ERROR_CODES_H_
#define TINY_ALPACA_SERVER_SRC_ASCOM_ERROR_CODES_H_

// Error codes defined by ASCOM.
// See: https://ascom-standards.org/Help/Developer/html/T_ASCOM_ErrorCodes.htm

#include "mcucore_platform.h"
#include "utils/status.h"

namespace alpaca {

class ErrorCodes {
 public:
  // Reserved error number (0x400, 1024) for property or method not implemented.
  static constexpr uint32_t kNotImplemented = 1024;
  static Status NotImplemented();

  // Reserved error code (0x401, 1025) for reporting an invalid value.
  static constexpr uint32_t kInvalidValue = 1025;
  static Status InvalidValue();

  // Reserved error code (0x402) for reporting that a value has not been set.
  static constexpr uint32_t kValueNotSet = 1026;
  static Status ValueNotSet();

  // Reserved error code (0x407, 1031) used to indicate that the communications
  // channel is not connected.
  static constexpr uint32_t kNotConnected = 1031;
  static Status NotConnected();

  // Reserved error code (0x408, 1032) used to indicate that the attempted
  // operation is invalid because the mount is currently in a Parked state.
  static constexpr uint32_t kInvalidWhileParked = 1032;
  static Status InvalidWhileParked();

  // Reserved error code (0x409, 1033) used to indicate that the attempted
  // operation is invalid because the mount is currently in a Slaved state.
  static constexpr uint32_t kInvalidWhileSlaved = 1033;
  static Status InvalidWhileSlaved();

  // Reserved error code (0x40A, 1034) related to settings.
  static constexpr uint32_t kSettingsProviderError = 0x40A;
  static Status SettingsProviderError();

  // Reserved error code(0x40B, 1035) to indicate that the requested operation
  // can not be undertaken at this time.
  static constexpr uint32_t kInvalidOperation = 1035;
  static Status InvalidOperation();

  // Reserved error code (0x40C, 1036) to indicate that the requested action is
  // not implemented in this driver.
  static constexpr uint32_t kActionNotImplemented = 1036;
  static Status ActionNotImplemented();

  // Reserved error code (0x40D, 1037) to indicate that the requested item is
  // not present in the ASCOM cache.
  static constexpr uint32_t kNotInCacheException = 0x40D;
  static Status NotInCacheException();

  // Reserved 'catch-all' error code (0x4FF, 1279) used when nothing else was
  // specified.
  static constexpr uint32_t kUnspecifiedError = 0x4FF;
  static Status UnspecifiedError();
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ASCOM_ERROR_CODES_H_
