#ifndef TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_
#define TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_

// TinyAlpacaServer is a wrapper around sstaub's Ethernet3 library. It
// initializes the device handlers, delegates relevant HTTP requests to them;
// the class also handles the device independent setup and management API.
//
// We make the following assumptions:
//
// * TinyAlpacaServer (TAS) can use kNumServerConnections of the hardware
//   sockets provided by the Ethernet instance.
//
// * TAS.begin() is called by the Arduino defined setup method, after IpDevice
//   has been used to initialize the hardware.
//
// * TAS.loop() is called by the Arduino defined loop method ~every time.
//
// * TAS takes care of calling the DHCP code to maintain the DHCP lease.
//
// * TAS dispatches TCP connection creation, input data, output space and
//   connection deletion events to the appropriate handler (e.g. the request
//   decoder).
//
// * When a complete request has been decoded, TAS dispatches the request to
//   the appropriate handler.
//
// * TAS handles the UDP packets of the Alpaca discovery protocol using data
//   provided at startup (e.g. the set of attached devices).
//
// TODO(jamessynge): Allow the caller to provide either Literal for the /setup
// HTML response, or to provide a function for that purpose.

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
