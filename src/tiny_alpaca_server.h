#ifndef TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_
#define TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_

// TinyAlpacaServerBase handles AlpacaRequests, dispatching to the appropriate
// device instance.
//
// TinyAlpacaServer adds in the networking support, using all but one of the
// hardware sockets provided by the Ethernet chip (i.e. reserving one for DHCP
// renewal), with one socket used for the Alpaca (UDP) Discovery Protocol, and
// the remaining sockets used for the Alpaca (HTTP) Management and Device APIs.
//
// TinyAlpacaServer::Initialize should be called from the setup function of the
// Arduino sketch, and then TinyAlpacaServer::PerformIO should be called from
// the loop function of the sketch.

#include "alpaca_devices.h"
#include "alpaca_discovery_server.h"
#include "device_impl_base.h"
#include "server_description.h"
#include "server_sockets_and_connections.h"
#include "utils/array_view.h"
#include "utils/platform.h"

namespace alpaca {

// The non-networking portion of TinyAlpacaServer, separated from
// TinyAlpacaServer for testability.
class TinyAlpacaServerBase : public RequestListener {
 public:
  TinyAlpacaServerBase(const ServerDescription& server_description,
                       ArrayView<DeviceInterface*> devices);

  template <size_t N>
  TinyAlpacaServerBase(const ServerDescription& server_description,
                       DeviceInterface* (&devices)[N])
      : TinyAlpacaServerBase(server_description,
                             ArrayView<DeviceInterface*>(devices, N)) {}

  // Calls Initialize on the nested objects. Returns true if all of the objects
  // are successfully initialized.
  bool Initialize();

  // Gives devices a chance to perform periodic work.
  void MaintainDevices();

  // RequestListener method overrides...
  void OnStartDecoding(AlpacaRequest& request) override;
  bool OnRequestDecoded(AlpacaRequest& request, Print& out) override;
  void OnRequestDecodingError(AlpacaRequest& request, EHttpStatusCode status,
                              Print& out) override;

 private:
  bool HandleManagementApiVersions(AlpacaRequest& request, Print& out);
  bool HandleManagementDescription(AlpacaRequest& request, Print& out);
  bool HandleServerSetup(AlpacaRequest& request, Print& out);

  AlpacaDevices alpaca_devices_;
  const ServerDescription& server_description_;
  uint32_t server_transaction_id_;
};

class TinyAlpacaServer : TinyAlpacaServerBase {
 public:
  TinyAlpacaServer(uint16_t tcp_port,
                   const ServerDescription& server_description,
                   ArrayView<DeviceInterface*> devices);

  template <size_t N>
  TinyAlpacaServer(uint16_t tcp_port,
                   const ServerDescription& server_description,
                   DeviceInterface* (&devices)[N])
      : TinyAlpacaServer(tcp_port, server_description,
                         ArrayView<DeviceInterface*>(devices, N)) {}

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

#endif  // TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_
