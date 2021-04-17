#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PRINT_MISC_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PRINT_MISC_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

#include "utils/platform.h"
#include "utils/printable_progmem_string.h"

namespace alpaca {

// Utility function supporting the printing of enumerator names, in this case
// handling an enum value that is not a defined enumerator.
size_t PrintUnknownEnumValueTo(PrintableProgmemString name, uint32_t v,
                               Print& out);

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_PRINT_MISC_H_
