#ifndef TINY_ALPACA_SERVER_SRC_TINY_ALPACA_NETWORK_SERVER_H_
#define TINY_ALPACA_SERVER_SRC_TINY_ALPACA_NETWORK_SERVER_H_

// TinyAlpacaNetworkServer provides networking support to
// TinyAlpacaDeviceServer, using one hardware networking socket for the Alpaca
// (UDP) Discovery Protocol, and some or all of the remaining sockets used for
// the Alpaca (HTTP) Management and Device APIs.
//
// TinyAlpacaNetworkServer::Initialize should be called from the setup function
// of the Arduino sketch, and then TinyAlpacaServer::PerformIO should be called
// from the loop function of the sketch.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "alpaca_devices.h"
#include "alpaca_discovery_server.h"
#include "device_interface.h"
#include "server_context.h"
#include "server_description.h"
#include "server_sockets_and_connections.h"
#include "tiny_alpaca_device_server.h"

namespace alpaca {

class TinyAlpacaNetworkServer {
 public:
  explicit TinyAlpacaNetworkServer(TinyAlpacaDeviceServer& device_server,
                                   uint16_t tcp_port = 80);

  // Calls Initialize on the nested objects, e.g. initializes sockets so they
  // listen for connections to tcp_port. Returns true if all of the objects are
  // successfully initialized.
  bool Initialize();

  // Performs network IO as appropriate, and gives handlers a chance
  // to perform periodic work.
  void PerformIO();

 private:
  ServerSocketsAndConnections sockets_;
  TinyAlpacaDiscoveryServer discovery_server_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_TINY_ALPACA_NETWORK_SERVER_H_
