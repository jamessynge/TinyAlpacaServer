#ifndef TINY_ALPACA_SERVER_SRC_TINY_ALPACA_REQUEST_HANDLER_H_
#define TINY_ALPACA_SERVER_SRC_TINY_ALPACA_REQUEST_HANDLER_H_

// TinyAlpacaRequestHandler owns the Device Handlers and dispatches requests
// to the appropriate handler or method.

#include "device_api_handler_base.h"
#include "request_listener.h"
#include "server_description.h"
#include "utils/array.h"
#include "utils/platform.h"

namespace alpaca {

class TinyAlpacaRequestHandler : public RequestListener {
 public:
  using DeviceApiHandlerBasePtr = DeviceApiHandlerBase* const;

  TinyAlpacaRequestHandler(const ServerDescription& server_description,
                           ArrayView<DeviceApiHandlerBasePtr> device_handlers);

  template <size_t N>
  TinyAlpacaRequestHandler(const ServerDescription& server_description,
                           DeviceApiHandlerBasePtr (&device_handlers)[N])
      : TinyAlpacaRequestHandler(
            server_description,
            ArrayView<DeviceApiHandlerBasePtr>(device_handlers, N)) {}

  // Prepares the server and device handlers to receive requests. Returns true
  // if able to do so, false otherwise.
  virtual bool Initialize();

  // Delegates to device handlers so that they can perform actions other than
  // responding to a request (e.g. periodically reading sensor values).
  virtual void MaintainDevices();

  // RequestListener method overrides...
  void OnStartDecoding(AlpacaRequest& request) override;
  bool OnRequestDecoded(AlpacaRequest& request, Print& out) override;
  void OnRequestDecodingError(AlpacaRequest& request, EHttpStatusCode status,
                              Print& out) override;

 private:
  bool DispatchDeviceRequest(AlpacaRequest& request,
                             DeviceApiHandlerBase& handler, Print& out);
  bool HandleManagementApiVersions(AlpacaRequest& request, Print& out);
  bool HandleManagementDescription(AlpacaRequest& request, Print& out);
  bool HandleManagementConfiguredDevices(AlpacaRequest& request, Print& out);
  bool HandleServerSetup(AlpacaRequest& request, Print& out);

  uint32_t server_transaction_id_;
  const ServerDescription& server_description_;
  ArrayView<DeviceApiHandlerBasePtr> device_handlers_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_TINY_ALPACA_REQUEST_HANDLER_H_
