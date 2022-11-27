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
#include "device_interface.h"
#include "request_listener.h"
#include "server_context.h"
#include "server_description.h"

namespace alpaca {

class TinyAlpacaDeviceServer : public RequestListener {
 public:
  TinyAlpacaDeviceServer(ServerContext& server_context,
                         const ServerDescription& server_description,
                         mcucore::ArrayView<DeviceInterface*> devices);

  template <size_t N>
  TinyAlpacaDeviceServer(ServerContext& server_context,
                         const ServerDescription& server_description,
                         DeviceInterface* (&devices)[N])
      : TinyAlpacaDeviceServer(
            server_context, server_description,
            mcucore::ArrayView<DeviceInterface*>(devices, N)) {}

  // Validates the set of devices (e.g. that the first device of each type has
  // device number 0), and does the minimum necessary to reset or disable any
  // features that might be turned on or enabled by default when the processor
  // resets. Must be called before InitializeDevices, and preferably as soon as
  // soon as possible after the program starts. CHECK fails (crashes) if the
  // configuration is not valid.
  void ValidateAndReset();

  // Initializes the server and its devices in preparation for serving requests.
  void InitializeForServing();

  // Gives devices a chance to perform periodic work. In the Arduino context,
  // this should be called from loop().
  void MaintainDevices();

  ServerContext& server_context() { return server_context_; }

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
  bool HandleAsset(AlpacaRequest& request, Print& out);

  AlpacaDevices alpaca_devices_;
  ServerContext& server_context_;
  // Maybe move the following into ServerContext?
  const ServerDescription& server_description_;
  uint32_t server_transaction_id_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_
