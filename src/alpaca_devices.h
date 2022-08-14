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
#include "constants.h"
#include "device_interface.h"
#include "server_context.h"

namespace alpaca {

class AlpacaDevices {
 public:
  explicit AlpacaDevices(ServerContext& server_context,
                         mcucore::ArrayView<DeviceInterface*> devices);

  // Validates the devices' DeviceDescription (e.g. that there is at most one
  // device number 0 of each device type). CHECK fails if there are any
  // problems.
  void ValidateDevices();

  // Does the minimum necessary to reset or disable any features that might be
  // turned on or enabled by default when the processor resets.
  void ResetHardware();

  // Calls Initialize on each device.
  void InitializeDevices();

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

  // Returns the specified device, or nullptr if not found.
  DeviceInterface* FindDevice(EDeviceType device_type, uint32_t device_number);

  ServerContext& server_context_;
  mcucore::ArrayView<DeviceInterface*> devices_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_DEVICES_H_
