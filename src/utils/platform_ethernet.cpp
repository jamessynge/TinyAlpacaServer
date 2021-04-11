#include "utils/platform_ethernet.h"

#include "utils/logging.h"

namespace alpaca {

bool PlatformEthernet::InitializeTcpListenerSocket(int sock_num,
                                                   uint16_t tcp_port) {
  TAS_VLOG(2) << TASLIT("PlatformEthernet::InitializeTcpListenerSocket(")
              << sock_num << TASLIT(", ") << tcp_port
              << TASLIT(") _server_port is ")
              << EthernetClass::_server_port[sock_num];
  TAS_DCHECK_LE(0, sock_num);
  TAS_DCHECK_LT(sock_num, MAX_SOCK_NUM);
#if TAS_EMBEDDED_TARGET
  EthernetClient client(sock_num);
  TAS_VLOG(2) << TASLIT("Socket status: ") << client.status();
  if (client.status() == SnSR::CLOSED) {
    if (EthernetClass::_server_port[sock_num] == tcp_port ||
        EthernetClass::_server_port[sock_num] == 0) {
      ::socket(sock_num, SnMR::TCP, tcp_port, 0);
      ::listen(sock_num);
      EthernetClass::_server_port[sock_num] = tcp_port;
      return true;
    }
  }
  return false;
#else   // !TAS_EMBEDDED_TARGET
  return HostSockets::InitializeTcpListenerSocket(sock_num, tcp_port);
#endif  // TAS_EMBEDDED_TARGET
}

bool PlatformEthernet::SocketIsConnected(int sock_num) {
  // Since we don't half-close (shutdown) sockets, checking to see if it is
  // writeable is sufficient.
  return IsOpenForWriting(sock_num);
}

bool PlatformEthernet::IsClientDone(int sock_num) {
  TAS_DCHECK_LE(0, sock_num);
  TAS_DCHECK_LT(sock_num, MAX_SOCK_NUM);
#if TAS_EMBEDDED_TARGET
  EthernetClient client(sock_num);
  return client.status() == SnSR::CLOSE_WAIT;
#else   // !TAS_EMBEDDED_TARGET
  return HostSockets::IsClientDone(sock_num);
#endif  // TAS_EMBEDDED_TARGET
}

bool PlatformEthernet::IsOpenForWriting(int sock_num) {
  TAS_DCHECK_LE(0, sock_num);
  TAS_DCHECK_LT(sock_num, MAX_SOCK_NUM);
#if TAS_EMBEDDED_TARGET
  EthernetClient client(sock_num);
  auto status = client.status();
  return status == SnSR::ESTABLISHED || status == SnSR::CLOSE_WAIT;
#else   // !TAS_EMBEDDED_TARGET
  return HostSockets::IsOpenForWriting(sock_num);
#endif  // TAS_EMBEDDED_TARGET
}

bool PlatformEthernet::SocketIsClosed(int sock_num) {
  TAS_DCHECK_LE(0, sock_num);
  TAS_DCHECK_LT(sock_num, MAX_SOCK_NUM);
#if TAS_EMBEDDED_TARGET
  EthernetClient client(sock_num);
  return client.status() == SnSR::CLOSED;
#else   // !TAS_EMBEDDED_TARGET
  return HostSockets::SocketIsClosed(sock_num);
#endif  // TAS_EMBEDDED_TARGET
}

}  // namespace alpaca
