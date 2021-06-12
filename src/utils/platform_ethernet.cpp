#include "utils/platform_ethernet.h"

#include "utils/logging.h"

namespace alpaca {

uint8_t PlatformEthernet::SocketStatus(uint8_t sock_num) {
  EthernetClient client(sock_num);
  return client.status();
}

int PlatformEthernet::FindUnusedSocket() {
  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    if (SocketIsClosed(sock_num)) {
      return sock_num;
    }
  }
  return -1;
}

bool PlatformEthernet::InitializeTcpListenerSocket(uint8_t sock_num,
                                                   uint16_t tcp_port) {
  TAS_VLOG(2) << TASLIT("PlatformEthernet::InitializeTcpListenerSocket(")
              << sock_num << TASLIT(", ") << tcp_port
              << TASLIT(") _server_port is ")
              << EthernetClass::_server_port[sock_num];
  TAS_DCHECK_LT(sock_num, MAX_SOCK_NUM);
#if TAS_EMBEDDED_TARGET
  EthernetClient client(sock_num);
  TAS_VLOG(2) << TASLIT("Socket status: ") << client.status();
  if (client.status() == SnSR::CLOSED) {
    if (EthernetClass::_server_port[sock_num] == tcp_port ||
        EthernetClass::_server_port[sock_num] == 0) {
      // TODO(jamessynge): Improve the underlying impl so that errors are
      // actually returned from socket and listen.
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

bool PlatformEthernet::SocketIsConnected(uint8_t sock_num) {
  // Since we don't half-close (shutdown) sockets, checking to see if it is
  // writeable is sufficient.
  return IsOpenForWriting(sock_num);
}

bool PlatformEthernet::DisconnectSocket(uint8_t sock_num) {
#if TAS_EMBEDDED_TARGET
  disconnect(sock_num);
  return true;
#else   // !TAS_EMBEDDED_TARGET
  return HostSockets::Disconnect(sock_num);
#endif  // TAS_EMBEDDED_TARGET
}

bool PlatformEthernet::CloseSocket(uint8_t sock_num) {
#if TAS_EMBEDDED_TARGET
  close(sock_num);
  return true;
#else   // !TAS_EMBEDDED_TARGET
  return HostSockets::Disconnect(sock_num);
#endif  // TAS_EMBEDDED_TARGET
}

bool PlatformEthernet::IsClientDone(uint8_t sock_num) {
  TAS_DCHECK_LT(sock_num, MAX_SOCK_NUM);
#if TAS_EMBEDDED_TARGET
  EthernetClient client(sock_num);
  return client.status() == SnSR::CLOSE_WAIT && client.available() <= 0;
#else   // !TAS_EMBEDDED_TARGET
  return HostSockets::IsClientDone(sock_num);
#endif  // TAS_EMBEDDED_TARGET
}

bool PlatformEthernet::IsOpenForWriting(uint8_t sock_num) {
  TAS_DCHECK_LT(sock_num, MAX_SOCK_NUM);
#if TAS_EMBEDDED_TARGET
  EthernetClient client(sock_num);
  auto status = client.status();
  return status == SnSR::ESTABLISHED || status == SnSR::CLOSE_WAIT;
#else   // !TAS_EMBEDDED_TARGET
  return HostSockets::IsOpenForWriting(sock_num);
#endif  // TAS_EMBEDDED_TARGET
}

bool PlatformEthernet::SocketIsClosed(uint8_t sock_num) {
  TAS_DCHECK_LT(sock_num, MAX_SOCK_NUM);
#if TAS_EMBEDDED_TARGET
  EthernetClient client(sock_num);
  return client.status() == SnSR::CLOSED;
#else   // !TAS_EMBEDDED_TARGET
  return HostSockets::SocketIsClosed(sock_num);
#endif  // TAS_EMBEDDED_TARGET
}

bool PlatformEthernet::StatusIsOpen(uint8_t status) {
  return status == SnSR::ESTABLISHED || status == SnSR::CLOSE_WAIT;
}

bool PlatformEthernet::StatusIsHalfOpen(uint8_t status) {
  return status == SnSR::CLOSE_WAIT;
}

bool PlatformEthernet::StatusIsClosing(uint8_t status) {
  switch (status) {
    case SnSR::FIN_WAIT:
    case SnSR::CLOSING:
    case SnSR::TIME_WAIT:
    case SnSR::LAST_ACK:
      return true;
  }
  return false;
}

}  // namespace alpaca
