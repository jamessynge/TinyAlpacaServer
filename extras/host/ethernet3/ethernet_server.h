#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET3_ETHERNET_SERVER_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET3_ETHERNET_SERVER_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

#include "extras/host/ethernet3/ethernet_client.h"
#include "extras/host/ethernet3/ethernet_config.h"

class EthernetServer {
 public:
  explicit EthernetServer(uint16_t port);
  // Finds an unused socket and uses it to listen for connections to the TCP
  // port 'port_'.
  void begin();

  // Returns an EthernetClient for the server listening on the TCP port 'port_'
  // for which there is data available from a client. The caller should use
  // getSocketNumber to determine which connection the data is from.
  EthernetClient available();

 private:
  // Accept
  void accept();
  uint16_t port_;
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET3_ETHERNET_SERVER_H_
