#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_

// Exports the Ethernet support needed by some parts of Tiny Alpaca Server.
//
// Author: james.synge@gmail.com

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
  // Returns the implementation defined status value for the specified socket.
  static uint8_t SocketStatus(uint8_t sock_num);

  // Finds a hardware socket that is closed, and returns its socket number.
  // Returns -1 if there is no such socket.
  static int FindUnusedSocket();

  // Set socket 'sock_num' to listen for new TCP connections on port 'tcp_port',
  // regardless of what that socket is doing now. Returns true if able to do so;
  // false if not (e.g. if sock_num or tcp_port is invalid).
  static bool InitializeTcpListenerSocket(uint8_t sock_num, uint16_t tcp_port);

  // Returns true if the socket is connected to a peer.
  static bool SocketIsConnected(uint8_t sock_num);

  // Initiates a DISCONNECT of a TCP socket.
  static bool DisconnectSocket(uint8_t sock_num);

  // Forces a socket to be closed, with no packets sent out.
  static bool CloseSocket(uint8_t sock_num);

  // SnSR::CLOSE_WAIT && no data available to read.
  static bool IsClientDone(uint8_t sock_num);

  // Is the connection open for writing (i.e. this end hasn't closed or
  // half-closed it)?
  static bool IsOpenForWriting(uint8_t sock_num);

  // Returns true if the socket is completely closed (not in use for any
  // purpose).
  static bool SocketIsClosed(uint8_t sock_num);

  // Returns true if the status indicates that the TCP connection is at least
  // half-open.
  static bool StatusIsOpen(uint8_t status);

  // Returns true if the status indicates that the TCP connection is half-open.
  static bool StatusIsHalfOpen(uint8_t status);

  // Returns true if the status indicates that the TCP connection is in the
  // process of closing (e.g. FIN_WAIT).
  static bool StatusIsClosing(uint8_t status);
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_
