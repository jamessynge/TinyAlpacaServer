#ifndef TINY_ALPACA_SERVER_SRC_ASCOM_ERROR_CODES_H_
#define TINY_ALPACA_SERVER_SRC_ASCOM_ERROR_CODES_H_

// See: https://ascom-standards.org/Help/Developer/html/T_ASCOM_ErrorCodes.htm

#include "platform.h"
#include "utils/status.h"

namespace alpaca {

class ErrorCodes {
 public:
  static Status ActionNotImplemented();
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ASCOM_ERROR_CODES_H_
