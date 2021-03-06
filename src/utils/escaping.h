#ifndef TINY_ALPACA_SERVER_SRC_UTILS_ESCAPING_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_ESCAPING_H_

// Core of support for printing JSON strings. Characters that are not valid in
// JSON strings (e.g. Ctrl-A) are not printed.

#include "utils/platform.h"

#if TAS_HOST_TARGET
#include <ostream>
#endif  // TAS_HOST_TARGET

namespace alpaca {

// Print c with appropriate escaping for JSON.
size_t PrintCharJsonEscaped(Print& out, char c);

#if TAS_HOST_TARGET
// Insert the escaped character into the ostream. Supports streaming
// JsonStringView instances, useful for logging and debugging.
void StreamCharJsonEscaped(std::ostream& out, const char c);
#endif  // TAS_HOST_TARGET

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_ESCAPING_H_
