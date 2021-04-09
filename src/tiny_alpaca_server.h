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
// TODO(jamessynge): Refactor to make testing much easier. For example, pull the
// ServerConnections out of this class, just leaving the device handlers and the
// public RequestListener implementation. That will enable testing by passing
// AlpacaRequest and Print instances into the listener methods, thus avoiding
// the need for actual network IO during testing.

#include "alpaca_devices.h"
#include "alpaca_discovery_server.h"
#include "device_impl_base.h"
#include "server_connections.h"
#include "server_description.h"
#include "utils/array_view.h"
#include "utils/platform.h"

namespace alpaca {

class TinyAlpacaServer : public RequestListener {
 public:
  using DeviceInterfacePtr = DeviceInterface* const;

  TinyAlpacaServer(uint16_t tcp_port,
                   const ServerDescription& server_description,
                   ArrayView<DeviceInterfacePtr> devices);

  template <size_t N>
  TinyAlpacaServer(uint16_t tcp_port,
                   const ServerDescription& server_description,
                   DeviceInterfacePtr (&devices)[N])
      : TinyAlpacaServer(tcp_port, server_description,
                         ArrayView<DeviceInterfacePtr>(devices, N)) {}

  // Prepares ServerConnections to receive TCP connections and a UDP listener to
  // receive Alpaca Discovery Protocol packets. Returns true if able to do so,
  // false otherwise.
  bool Initialize();

  // Performs network IO as appropriate, and gives handlers a chance
  // to perform periodic work.
  void PerformIO();

  // RequestListener method overrides...
  void OnStartDecoding(AlpacaRequest& request) override;
  bool OnRequestDecoded(AlpacaRequest& request, Print& out) override;
  void OnRequestDecodingError(AlpacaRequest& request, EHttpStatusCode status,
                              Print& out) override;

 private:
  bool HandleManagementApiVersions(AlpacaRequest& request, Print& out);
  bool HandleManagementDescription(AlpacaRequest& request, Print& out);
  bool HandleManagementConfiguredDevices(AlpacaRequest& request, Print& out);
  bool HandleServerSetup(AlpacaRequest& request, Print& out);

  AlpacaDevices alpaca_devices_;
  ServerConnections server_connections_;
  TinyAlpacaDiscoveryServer discovery_server_;
  uint32_t server_transaction_id_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_
