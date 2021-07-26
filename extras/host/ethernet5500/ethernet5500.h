#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET5500_ETHERNET5500_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET5500_ETHERNET5500_H_

// Just enough of Ethernet5500's classes for Tiny Alpaca Server to compile on
// host, maybe to be a TCP & UDP server.

#include "extras/host/arduino/ip_address.h"            // IWYU pragma: export
#include "extras/host/ethernet5500/dhcp_class.h"       // IWYU pragma: export
#include "extras/host/ethernet5500/ethernet_class.h"   // IWYU pragma: export
#include "extras/host/ethernet5500/ethernet_client.h"  // IWYU pragma: export
#include "extras/host/ethernet5500/ethernet_config.h"  // IWYU pragma: export
#include "extras/host/ethernet5500/ethernet_server.h"  // IWYU pragma: export
#include "extras/host/ethernet5500/ethernet_udp.h"     // IWYU pragma: export
#include "extras/host/ethernet5500/w5500.h"

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET5500_ETHERNET5500_H_
