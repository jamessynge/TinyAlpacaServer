// TODO(jamessynge): Describe why this file exists/what it provides.

#include "utils/platform_ethernet.h"

#include "utils/platform.h"

namespace alpaca {

#if TAS_EMBEDDED_TARGET

bool InitializeTcpListenerSocket(int sock_num, uint16_t tcp_port) {
  EthernetClient client(sock_num);
  client.stop();

  //  ::close(sock_num);
  //  return ::socket(sock, SnMR::TCP, _port, 0) && ::listen(sock);
}

bool IsClientDone(int sock_num) {
  TAS_DCHECK_NE(sock_num, MAX_SOCK_NUM);
  auto status = w5500.readSnSR(sock_num);
  return s == SnSR::CLOSE_WAIT && w5500.getRXReceivedSize(sock_num) == 0;
}

#endif  // TAS_EMBEDDED_TARGET

}  // namespace alpaca
