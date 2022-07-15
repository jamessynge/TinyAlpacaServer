#ifndef TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_
#define TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_

// TinyAlpacaDeviceServer handles AlpacaRequests, dispatching successfully
// decoded requests to the appropriate device instance.
//
// TinyAlpacaNetworkServer adds in the networking support, using all but one of
// the hardware sockets provided by the Ethernet chip (i.e. reserving one for
// DHCP renewal), with one socket used for the Alpaca (UDP) Discovery Protocol,
// and the remaining sockets used for the Alpaca (HTTP) Management and Device
// APIs.
//
// TinyAlpacaNetworkServer::Initialize should be called from the setup function
// of the Arduino sketch, and then TinyAlpacaServer::PerformIO should be called
// from the loop function of the sketch.
//
// Separating the two classes this way improves testability of the
// non-networking portion, i.e. of TinyAlpacaNetworkServer.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "alpaca_devices.h"
#include "alpaca_discovery_server.h"
#include "device_interface.h"
#include "server_description.h"
#include "server_sockets_and_connections.h"

namespace alpaca {

class TinyAlpacaDeviceServer : public RequestListener {
 public:
  TinyAlpacaDeviceServer(const ServerDescription& server_description,
                         mcucore::ArrayView<DeviceInterface*> devices);

  template <size_t N>
  TinyAlpacaDeviceServer(const ServerDescription& server_description,
                         DeviceInterface* (&devices)[N])
      : TinyAlpacaDeviceServer(
            server_description,
            mcucore::ArrayView<DeviceInterface*>(devices, N)) {}

  // Validates that the configuration of the server is sensible; should be
  // called before ResetHardware. CHECK fails if there are any problems.
  void ValidateConfiguration();

  // Does the minimum necessary to reset or disable any features that might be
  // turned on or enabled by default when the processor resets. Must be called
  // before InitializeDevices, and preferably as soon as soon as possible after
  // the program starts.
  void ResetHardware();

  // Initializes the server and its devices in preparation for serving requests.
  void InitializeForServing();

  // Gives devices a chance to perform periodic work.
  void MaintainDevices();

  const ServerDescription& server_description() const {
    return server_description_;
  }

  // RequestListener method overrides...
  void OnStartDecoding(AlpacaRequest& request) override;
  bool OnRequestDecoded(AlpacaRequest& request, Print& out) override;
  void OnRequestDecodingError(AlpacaRequest& request, EHttpStatusCode status,
                              Print& out) override;

 private:
  bool HandleManagementApiVersions(AlpacaRequest& request, Print& out);
  bool HandleManagementDescription(AlpacaRequest& request, Print& out);
  bool HandleServerSetup(AlpacaRequest& request, Print& out);
  bool HandleServerStatus(AlpacaRequest& request, Print& out);

  AlpacaDevices alpaca_devices_;
  const ServerDescription& server_description_;
  uint32_t server_transaction_id_;
};

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

#endif  // TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_
