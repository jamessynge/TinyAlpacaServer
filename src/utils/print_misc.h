#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PRINT_MISC_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PRINT_MISC_H_

// Miscellaneous print functions, in a separate compilation unit to avoid
// dependency cycles.
//
// Author: james.synge@gmail.com

#include "experimental/users/jamessynge/arduino/mcucore/src/mcucore_platform.h"

namespace alpaca {

// Utility function supporting the printing of enumerator names, in this case
// handling an enum value that is not a defined enumerator.
size_t PrintUnknownEnumValueTo(const __FlashStringHelper* name, uint32_t v,
                               Print& out);

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_PRINT_MISC_H_
