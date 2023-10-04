#ifndef TINY_ALPACA_SERVER_SRC_ASCOM_ERROR_CODES_H_
#define TINY_ALPACA_SERVER_SRC_ASCOM_ERROR_CODES_H_

// Error codes defined by ASCOM.
// See: https://ascom-standards.org/Help/Developer/html/T_ASCOM_ErrorCodes.htm
//
// Author: james.synge@gmail.com

#include <McuCore.h>

namespace alpaca {

class ErrorCodes {
 public:
  // Reserved error code (0x400, 1024) for property or method not implemented.
  // Also applies to ISwitchV2.SetSwitch if the switch device is not writable.
  static constexpr uint16_t kNotImplemented = 1024;
  static constexpr mcucore::StatusCode kNotImplementedStatusCode =
      static_cast<mcucore::StatusCode>(kNotImplemented);
  static mcucore::Status NotImplemented();

  // Reserved error code (0x401, 1025) for reporting an invalid value.
  static constexpr uint16_t kInvalidValue = 1025;
  static constexpr mcucore::StatusCode kInvalidValueStatusCode =
      static_cast<mcucore::StatusCode>(kInvalidValue);
  static mcucore::Status InvalidValue();

  // Reserved error code (0x402) for reporting that a value has not been set.
  static constexpr uint16_t kValueNotSet = 1026;
  static constexpr mcucore::StatusCode kValueNotSetStatusCode =
      static_cast<mcucore::StatusCode>(kValueNotSet);
  static mcucore::Status ValueNotSet();

  // Reserved error code (0x407, 1031) used to indicate that the communications
  // channel is not connected.
  static constexpr uint16_t kNotConnected = 1031;
  static constexpr mcucore::StatusCode kNotConnectedStatusCode =
      static_cast<mcucore::StatusCode>(kNotConnected);
  static mcucore::Status NotConnected();

  // Reserved error code (0x408, 1032) used to indicate that the attempted
  // operation is invalid because the mount is currently in a Parked state.
  static constexpr uint16_t kInvalidWhileParked = 1032;
  static constexpr mcucore::StatusCode kInvalidWhileParkedStatusCode =
      static_cast<mcucore::StatusCode>(kInvalidWhileParked);
  static mcucore::Status InvalidWhileParked();

  // Reserved error code (0x409, 1033) used to indicate that the attempted
  // operation is invalid because the mount is currently in a Slaved state.
  static constexpr uint16_t kInvalidWhileSlaved = 1033;
  static constexpr mcucore::StatusCode kInvalidWhileSlavedStatusCode =
      static_cast<mcucore::StatusCode>(kInvalidWhileSlaved);
  static mcucore::Status InvalidWhileSlaved();

  // Reserved error code (0x40A, 1034) related to settings.
  static constexpr uint16_t kSettingsProviderError = 0x40A;
  static constexpr mcucore::StatusCode kSettingsProviderErrorStatusCode =
      static_cast<mcucore::StatusCode>(kSettingsProviderError);
  static mcucore::Status SettingsProviderError();

  // Reserved error code(0x40B, 1035) to indicate that the requested operation
  // can not be undertaken at this time.
  static constexpr uint16_t kInvalidOperation = 1035;
  static constexpr mcucore::StatusCode kInvalidOperationStatusCode =
      static_cast<mcucore::StatusCode>(kInvalidOperation);
  static mcucore::Status InvalidOperation();

  // Reserved error code (0x40C, 1036) to indicate that the requested action is
  // not implemented in this driver.
  static constexpr uint16_t kActionNotImplemented = 1036;
  static constexpr mcucore::StatusCode kActionNotImplementedStatusCode =
      static_cast<mcucore::StatusCode>(kActionNotImplemented);
  static mcucore::Status ActionNotImplemented();

  // Reserved error code (0x40D, 1037) to indicate that the requested item is
  // not present in the ASCOM cache.
  static constexpr uint16_t kNotInCacheException = 0x40D;
  static constexpr mcucore::StatusCode kNotInCacheExceptionStatusCode =
      static_cast<mcucore::StatusCode>(kNotInCacheException);
  static mcucore::Status NotInCacheException();

  // Reserved 'catch-all' error code (0x4FF, 1279) used when nothing else was
  // specified.
  static constexpr uint16_t kUnspecifiedError = 0x4FF;
  static constexpr mcucore::StatusCode kUnspecifiedErrorStatusCode =
      static_cast<mcucore::StatusCode>(kUnspecifiedError);
  static mcucore::Status UnspecifiedError();
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ASCOM_ERROR_CODES_H_
