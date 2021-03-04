#ifndef TINY_ALPACA_SERVER_EXTRAS_ETHERNET3_ETHERNET_CLIENT_H_
#define TINY_ALPACA_SERVER_EXTRAS_ETHERNET3_ETHERNET_CLIENT_H_

// Just enough of EthernetClient for Tiny Alpaca Server to compile on host,
// maybe to be a TCP server.

#include "extras/host_arduino/client.h"

class EthernetClient : public Client {};

#endif  // TINY_ALPACA_SERVER_EXTRAS_ETHERNET3_ETHERNET_CLIENT_H_
