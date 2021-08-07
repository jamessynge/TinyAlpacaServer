#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_DEVICES_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_DEVICES_H_

// AlpacaDevices is responsible for dispatching the appropriate HTTP requests to
// DeviceInterface implementations (e.g. an instance that implements an ASCOM
// Alpaca ObservingConditions device), and for providing them periodic Update
// calls.
//
// Author: james.synge@gmail.com

#include "alpaca_request.h"
#include "device_interface.h"
#include "device_types/device_impl_base.h"
#include "json_response.h"
#include "mcucore_platform.h"
#include "request_listener.h"
#include "server_description.h"
#include "utils/array_view.h"
#include "utils/json_encoder.h"

namespace alpaca {

class AlpacaDevices {
 public:
  explicit AlpacaDevices(ArrayView<DeviceInterface*> devices);

  // Prepares the server and device drivers to receive requests. Returns true
  // if able to do so, false otherwise.
  bool Initialize();

  // Delegates to device drivers so that they can perform actions other than
  // responding to a request (e.g. periodically reading sensor values).
  void MaintainDevices();

  // Given a request for "/management/v1/configureddevices", writes the response
  // to out.
  bool HandleManagementConfiguredDevices(AlpacaRequest& request, Print& out);

  // Given an HTTP Device API or Device Setup request, dispatches to the
  // appropriate DeviceInterface implementation. Returns true if the request is
  // handled without error, or false if there is a problem (in which case the
  // caller is expected to close the connection).
  bool DispatchDeviceRequest(AlpacaRequest& request, Print& out);

 private:
  bool DispatchDeviceRequest(AlpacaRequest& request, DeviceInterface& device,
                             Print& out);

  ArrayView<DeviceInterface*> devices_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_DEVICES_H_
