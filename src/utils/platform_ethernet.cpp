#include "utils/platform_ethernet.h"

#include "utils/logging.h"
#include "utils/platform.h"

namespace alpaca {

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
#if TAS_EMBEDDED_TARGET
  TAS_DCHECK_LE(0, sock_num);
  TAS_DCHECK_LT(sock_num, MAX_SOCK_NUM);
  EthernetClient client(sock_num);
  auto status = client.status();
  return status == SnSR::ESTABLISHED || status == SnSR::CLOSE_WAIT;
#else   // !TAS_EMBEDDED_TARGET
  return HostSockets::IsOpenForWriting(sock_num);
#endif  // TAS_EMBEDDED_TARGET
}

}  // namespace alpaca
