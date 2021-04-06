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

#include "device_api_handler_base.h"
#include "request_listener.h"
#include "server_connection.h"
#include "server_description.h"
#include "utils/array.h"
#include "utils/platform.h"

namespace alpaca {

class TinyAlpacaServer : RequestListener {
 public:
  using DeviceApiHandlerBasePtr = DeviceApiHandlerBase* const;

  TinyAlpacaServer(uint16_t tcp_port,
                   const ServerDescription& server_description,
                   ArrayView<DeviceApiHandlerBasePtr> device_handlers);

  template <size_t N>
  TinyAlpacaServer(uint16_t tcp_port,
                   const ServerDescription& server_description,
                   DeviceApiHandlerBasePtr (&device_handlers)[N])
      : TinyAlpacaServer(
            tcp_port, server_description,
            ArrayView<DeviceApiHandlerBasePtr>(device_handlers, N)) {}

  // Prepares ServerConnections to receive TCP connections and a UDP listener to
  // receive Alpaca Discovery Protocol packets. Returns true if able to do so,
  // false otherwise.
  bool begin();

  // Performs network IO as appropriate.
  void loop();

 protected:
  void OnStartDecoding(AlpacaRequest& request) override;

  // Called when a request has been successfully decoded. 'out' should be used
  // to write a response to the client. Return true to continue decoding more
  // requests from the client, false to disconnect.
  bool OnRequestDecoded(AlpacaRequest& request, Print& out) override;

  // Called when decoding of a request has failed. 'out' should be used to write
  // an error response to the client. The connection to the client will be
  // closed after the response is returned.
  void OnRequestDecodingError(AlpacaRequest& request, EHttpStatusCode status,
                              Print& out) override;

 private:
  // Not using all of the ports, need to reserve one for UDP and maybe one for
  // outbound connections to a time server.
  static constexpr size_t kNumServerConnections = 4;
  using ServerConnectionsArray = ServerConnection[kNumServerConnections];
  static constexpr size_t kServerConnectionsStorage =
      sizeof(ServerConnectionsArray);

  // Returns a pointer to the ServerConnection with index 'ndx', where 'ndx' is
  // in the range [0, kNumServerConnections-1].
  ServerConnection* GetServerConnection(size_t ndx);

  // Returns a pointer to the ServerConnection assigned to socket 'sock_num'.
  ServerConnection* GetServerConnectionForSocket(int sock_num);

  // Finds a ServerConnection that isn't in use and assigns it to the specified
  // socket.
  bool AssignServerConnectionToSocket(int sock_num);

  bool DispatchDeviceRequest(AlpacaRequest& request,
                             DeviceApiHandlerBase& handler, Print& out);

  bool HandleManagementApiVersions(AlpacaRequest& request, Print& out);
  bool HandleManagementDescription(AlpacaRequest& request, Print& out);
  bool HandleManagementConfiguredDevices(AlpacaRequest& request, Print& out);
  bool HandleServerSetup(AlpacaRequest& request, Print& out);

  uint32_t server_transaction_id_;
  uint16_t tcp_port_;

  const ServerDescription& server_description_;
  ArrayView<DeviceApiHandlerBasePtr> device_handlers_;

  alignas(ServerConnection) uint8_t
      connections_storage_[kServerConnectionsStorage];
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_TINY_ALPACA_SERVER_H_
