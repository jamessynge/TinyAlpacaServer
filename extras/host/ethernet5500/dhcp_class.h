#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET5500_DHCP_CLASS_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET5500_DHCP_CLASS_H_

// DhcpClass is used in embedded environment to lease an IP address for the
// device, etc. On the host it does nothing.

#define DHCP_CHECK_NONE (0)
#define DHCP_CHECK_RENEW_FAIL (1)
#define DHCP_CHECK_RENEW_OK (2)
#define DHCP_CHECK_REBIND_FAIL (3)
#define DHCP_CHECK_REBIND_OK (4)

class DhcpClass {};

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET5500_DHCP_CLASS_H_
