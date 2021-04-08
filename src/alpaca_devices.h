#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_DEVICES_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_DEVICES_H_

// AlpacaDevices is responsible for dispatching the appropriate HTTP requests to
// DeviceInterface implementations (e.g. an instance that implements an ASCOM
// Alpaca ObservingConditions device), and for providing them periodic Update
// calls.

#include "device_impl_base.h"
#include "request_listener.h"
#include "server_description.h"
#include "utils/array.h"
#include "utils/platform.h"

namespace alpaca {

class AlpacaDevices : public RequestListener {
 public:
  using DeviceInterfacePtr = DeviceInterface* const;

  AlpacaDevices(const ServerDescription& server_description,
                ArrayView<DeviceInterfacePtr> devices);

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
  bool DispatchDeviceRequest(AlpacaRequest& request, DeviceInterface& handler,
                             Print& out);
  bool HandleManagementApiVersions(AlpacaRequest& request, Print& out);
  bool HandleManagementDescription(AlpacaRequest& request, Print& out);
  bool HandleManagementConfiguredDevices(AlpacaRequest& request, Print& out);
  bool HandleServerSetup(AlpacaRequest& request, Print& out);

  uint32_t server_transaction_id_;
  const ServerDescription& server_description_;
  ArrayView<DeviceInterfacePtr> devices_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_DEVICES_H_
