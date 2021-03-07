#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_

// Exports the Ethernet support needed by some parts of Tiny Alpaca Server.

#include "utils/platform.h"

#if TAS_EMBEDDED_TARGET

#include <Ethernet3.h>  // IWYU pragma: export

namespace alpaca {

// Set socket 'sock_num' to listen for new TCP connections on port 'tcp_port',
// regardless of what that socket is doing now. Returns true if able to do so;
// false if not (e.g. if sock_num or tcp_port is invalid).
bool InitializeTcpListenerSocket(int sock_num, uint16_t tcp_port);

// SnSR::CLOSE_WAIT && no data available to read.
bool IsClientDone(int sock_num);

}  // namespace alpaca

#else  // !TAS_EMBEDDED_TARGET

#include "extras/ethernet3/ethernet3.h"     // IWYU pragma: export
#include "extras/ethernet3/host_sockets.h"  // IWYU pragma: export

#endif  // TAS_EMBEDDED_TARGET

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_
