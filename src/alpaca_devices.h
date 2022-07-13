#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_DEVICES_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_DEVICES_H_

// AlpacaDevices is responsible for dispatching the appropriate HTTP requests to
// DeviceInterface implementations (e.g. an instance that implements an ASCOM
// Alpaca ObservingConditions device), and for providing them periodic Update
// calls.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "alpaca_request.h"
#include "device_interface.h"

namespace alpaca {

class AlpacaDevices {
 public:
  explicit AlpacaDevices(mcucore::ArrayView<DeviceInterface*> devices);

  // Prepares the server and device drivers to receive requests. Returns OK
  // if able to do so, otherwise an error.
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

  // Give each device a chance to add some output to the HTML page being built.
  void AddToHomePageHtml(const AlpacaRequest& request, EHtmlPageSection section,
                         mcucore::OPrintStream& strm);

 private:
  bool DispatchDeviceRequest(AlpacaRequest& request, DeviceInterface& device,
                             Print& out);

  mcucore::ArrayView<DeviceInterface*> devices_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_DEVICES_H_
