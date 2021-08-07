#ifndef TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_PLATFORM_ETHERNET_H_

// Exports the Ethernet support needed by some parts of Tiny Alpaca Server.
//
// Author: james.synge@gmail.com

#include "mcucore_platform.h"  // IWYU pragma: export

#ifdef ARDUINO

#include <Client.h>          // IWYU pragma: export
#include <Ethernet5500.h>    // IWYU pragma: export
#include <IPAddress.h>       // IWYU pragma: export
#include <Stream.h>          // IWYU pragma: export
#include <utility/socket.h>  // IWYU pragma: export

#elif TAS_EMBEDDED_TARGET

#error "No support known for this platform."

#elif TAS_HOST_TARGET

#include <Client.h>        // IWYU pragma : export
#include <Ethernet5500.h>  // IWYU pragma: export
#include <IPAddress.h>     // IWYU pragma : export

#include "mcucore/extrashost/ethernet5500/host_sockets.h"  // IWYU pragma: export

#endif  // ARDUINO

namespace alpaca {

#ifndef TAS_HAS_PLATFORM_ETHERNET_INTERFACE
#if TAS_HOST_TARGET
#define TAS_HAS_PLATFORM_ETHERNET_INTERFACE 1
#endif  // TAS_HOST_TARGET
#endif  // TAS_HAS_PLATFORM_ETHERNET_INTERFACE

#if TAS_HAS_PLATFORM_ETHERNET_INTERFACE
class PlatformEthernetInterface {
 public:
  virtual ~PlatformEthernetInterface();

  // Returns the implementation defined status value for the specified socket.
  virtual uint8_t SocketStatus(uint8_t sock_num) = 0;

  // Finds a hardware socket that is closed, and returns its socket number.
  // Returns -1 if there is no such socket.
  virtual int FindUnusedSocket() = 0;

  // Set socket 'sock_num' to listen for new TCP connections on port 'tcp_port',
  // regardless of what that socket is doing now. Returns true if able to do so;
  // false if not (e.g. if sock_num or tcp_port is invalid).
  virtual bool InitializeTcpListenerSocket(uint8_t sock_num,
                                           uint16_t tcp_port) = 0;

  // Returns true if the hardware socket is being used for TCP and is not
  // LISTENING; if so, then it is best not to repurpose the hardware socket.
  virtual bool SocketIsInTcpConnectionLifecycle(uint8_t sock_num) = 0;

  // Returns true if the hardware socket is listening for TCP connections.
  virtual bool SocketIsTcpListener(uint8_t sock_num, uint16_t tcp_port) = 0;

  // Returns true if the socket is connected to a peer.
  virtual bool SocketIsConnected(uint8_t sock_num) = 0;

  // Initiates a DISCONNECT of a TCP socket.
  virtual bool DisconnectSocket(uint8_t sock_num) = 0;

  // Forces a socket to be closed, with no packets sent out.
  virtual bool CloseSocket(uint8_t sock_num) = 0;

  // SnSR::CLOSE_WAIT && no data available to read.
  virtual bool IsClientDone(uint8_t sock_num) = 0;

  // Is the connection open for writing (i.e. this end hasn't closed or
  // half-closed it)?
  virtual bool IsOpenForWriting(uint8_t sock_num) = 0;

  // Returns true if the socket is completely closed (not in use for any
  // purpose).
  virtual bool SocketIsClosed(uint8_t sock_num) = 0;

  // Returns true if the status indicates that the TCP connection is at least
  // half-open.
  virtual bool StatusIsOpen(uint8_t status) = 0;

  // Returns true if the status indicates that the TCP connection is half-open.
  virtual bool StatusIsHalfOpen(uint8_t status) = 0;

  // Returns true if the status indicates that the TCP connection is in the
  // process of closing (e.g. FIN_WAIT).
  virtual bool StatusIsClosing(uint8_t status) = 0;
};
#endif  // TAS_HAS_PLATFORM_ETHERNET_INTERFACE

// Helper for testing with the same API on host and embedded.
struct PlatformEthernet {
#if TAS_HAS_PLATFORM_ETHERNET_INTERFACE
  static void SetPlatformEthernetImplementation(
      PlatformEthernetInterface* platform_ethernet_impl);
#endif  // TAS_HAS_PLATFORM_ETHERNET_INTERFACE

  // Returns the implementation defined status value for the specified socket.
  static uint8_t SocketStatus(uint8_t sock_num);

  // Finds a hardware socket that is closed, and returns its socket number.
  // Returns -1 if there is no such socket.
  static int FindUnusedSocket();

  // Set socket 'sock_num' to listen for new TCP connections on port 'tcp_port',
  // regardless of what that socket is doing now. Returns true if able to do so;
  // false if not (e.g. if sock_num or tcp_port is invalid).
  static bool InitializeTcpListenerSocket(uint8_t sock_num, uint16_t tcp_port);

  // Returns true if the hardware socket is being used for TCP and is not
  // LISTENING; if so, then it is best not to repurpose the hardware socket.
  static bool SocketIsInTcpConnectionLifecycle(uint8_t sock_num);

  // Returns true if the hardware socket is listening for TCP connections.
  static bool SocketIsTcpListener(uint8_t sock_num, uint16_t tcp_port);

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
