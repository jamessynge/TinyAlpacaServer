#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_

// Exports the Ethernet support needed by some parts of Tiny Alpaca Server.

#include "utils/platform.h"  // IWYU pragma: export

#if TAS_EMBEDDED_TARGET

#include <Client.h>          // IWYU pragma: export
#include <Ethernet3.h>       // IWYU pragma: export
#include <IPAddress.h>       // IWYU pragma: export
#include <Stream.h>          // IWYU pragma: export
#include <utility/socket.h>  // IWYU pragma: export

#else  // !TAS_EMBEDDED_TARGET

#include "extras/host/arduino/client.h"          // IWYU pragma : export
#include "extras/host/arduino/ip_address.h"      // IWYU pragma : export
#include "extras/host/ethernet3/ethernet3.h"     // IWYU pragma: export
#include "extras/host/ethernet3/host_sockets.h"  // IWYU pragma: export

#endif  // TAS_EMBEDDED_TARGET

namespace alpaca {

// Helper for testing with the same API on host and embedded.
struct PlatformEthernet {
  // Set socket 'sock_num' to listen for new TCP connections on port 'tcp_port',
  // regardless of what that socket is doing now. Returns true if able to do so;
  // false if not (e.g. if sock_num or tcp_port is invalid).
  static bool InitializeTcpListenerSocket(int sock_num, uint16_t tcp_port);

  // Returns true if the socket is connected to a peer.
  static bool SocketIsConnected(int sock_num);

  // SnSR::CLOSE_WAIT && no data available to read.
  static bool IsClientDone(int sock_num);

  // Is the connection open for writing (i.e. this end hasn't closed or
  // half-closed it)?
  static bool IsOpenForWriting(int sock_num);

  // Returns true if the socket is completely closed (not in use for any
  // purpose).
  static bool SocketIsClosed(int sock_num);
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_
