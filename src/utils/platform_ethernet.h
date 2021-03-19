#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_

// Exports the Ethernet support needed by some parts of Tiny Alpaca Server.

#include "utils/platform.h"  // IWYU pragma: export

#if TAS_EMBEDDED_TARGET

#include <Client.h>     // IWYU pragma: export
#include <Ethernet3.h>  // IWYU pragma: export
#include <Stream.h>     // IWYU pragma: export

#else  // !TAS_EMBEDDED_TARGET

#include "extras/host/arduino/client.h"          // IWYU pragma : export
#include "extras/host/ethernet3/ethernet3.h"     // IWYU pragma: export
#include "extras/host/ethernet3/host_sockets.h"  // IWYU pragma: export

#endif  // TAS_EMBEDDED_TARGET

namespace alpaca {

// Helper for testing with the same API on host and embedded.
struct PlatformEthernet {
  // SnSR::CLOSE_WAIT && no data available to read.
  static bool IsClientDone(int sock_num);

  // Is the connection open for writing (i.e. this end hasn't closed or
  // half-closed it)?
  static bool IsOpenForWriting(int sock_num);
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_
