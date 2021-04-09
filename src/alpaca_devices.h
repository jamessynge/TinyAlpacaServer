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
#include "utils/json_encoder.h"
#include "utils/platform.h"

namespace alpaca {

class AlpacaDevices {
 public:
  using DeviceInterfacePtr = DeviceInterface* const;

  AlpacaDevices(const ServerDescription& server_description,
                ArrayView<DeviceInterfacePtr> devices);

  // Prepares the server and device handlers to receive requests. Returns true
  // if able to do so, false otherwise.
  bool Initialize();

  // Delegates to device handlers so that they can perform actions other than
  // responding to a request (e.g. periodically reading sensor values).
  void MaintainDevices();

  // Given an HTTP Device API or Device Setup request, dispatches to the
  // appropriate DeviceInterface implementation. Returns true if the request is
  // handled without error, or false if there is a problem (in which case the
  // caller is expected to close the connection).
  bool DispatchDeviceRequest(AlpacaRequest& request, Print& out);

  // Writes entries into the Value array of the JSON response being produced in
  // response to a request for "/management/v1/configureddevices".
  void AddConfiguredDevices(JsonArrayEncoder& encoder) const;

 private:
  bool DispatchDeviceRequest(AlpacaRequest& request, DeviceInterface& handler,
                             Print& out);

  const ServerDescription& server_description_;
  ArrayView<DeviceInterfacePtr> devices_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_DEVICES_H_
