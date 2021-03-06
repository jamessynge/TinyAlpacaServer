// TODO(jamessynge): Describe why this file exists/what it provides.

#include "ascom_error_codes.h"

#include "platform.h"

namespace alpaca {

// static
Status ErrorCodes::ActionNotImplemented() { return Status(0x8004040C); }

}  // namespace alpaca
