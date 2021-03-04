#ifndef TINY_ALPACA_SERVER_SRC_PLATFORM_ETHERNET_H_
#define TINY_ALPACA_SERVER_SRC_PLATFORM_ETHERNET_H_

// Exports the Ethernet support needed by some parts of Tiny Alpaca Server.

#include "platform.h"

#if TAS_EMBEDDED_TARGET

#include <Ethernet3.h>  // IWYU pragma: export

#else  // !TAS_EMBEDDED_TARGET

#include "extras/ethernet3/ethernet3.h"  // IWYU pragma: export

#endif  // TAS_EMBEDDED_TARGET

namespace alpaca {

// Declare stuff

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_PLATFORM_ETHERNET_H_
