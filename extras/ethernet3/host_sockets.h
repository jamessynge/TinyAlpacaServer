#ifndef TINY_ALPACA_SERVER_EXTRAS_ETHERNET3_HOST_SOCKETS_H_
#define TINY_ALPACA_SERVER_EXTRAS_ETHERNET3_HOST_SOCKETS_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

// TODO(jamessynge): Consider implementing Ethernet3/src/utility/socket.* using
// the *nix Socket API, along with relevant portions of utility/w5500.* needed
// by EthernetClient, then using Ethernet3/src/EthernetClient.* (approximately)
// as is.

#include <cstdint>

namespace alpaca {

// Set socket 'sock_num' to listen for new TCP connections on port 'tcp_port',
// regardless of what that socket is doing now. Returns true if able to do so;
// false if not (e.g. if sock_num or tcp_port is invalid).
bool InitializeTcpListenerSocket(int sock_num, uint16_t tcp_port);

// If socket 'sock_num' is listening for TCP connections, and is not already
// connected to a client, and a new connection is available, accept it.
bool AcceptConnection(int sock_num);

// SnSR::CLOSE_WAIT && no data available to read.
bool IsClientDone(int sock_num);

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_ETHERNET3_HOST_SOCKETS_H_
