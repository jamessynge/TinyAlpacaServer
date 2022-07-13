#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_INTERFACE_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_INTERFACE_H_

// DeviceInterface defines the API that a device must present to the Tiny Alpaca
// Server. W.r.t. the server, this is the device driver.
//
// The MaintainDevice method will be called periodically (typically hundreds of
// times per second), and the HandleDeviceSetupRequest and
// HandleDeviceApiRequest will be called when valid HTTP requests are decoded
// for this device, i.e. those with a path like:
//
//      /api/v1/{device_type}/{device_number}/{method_name}
//      /setup/v1/{device_type}/{device_number}/setup
//
// There is no support for incrementally returning responses (i.e. over the
// course of multiple Arduino loop() executions), so the responses produced by
// the Handle*Request methods generally need to be small enough to fit in the
// buffers available via 'out' (e.g. at most a few Ethernet frames as provided
// by a WIZ5500), else they'll stall the system while we wait for ACKs from the
// client.
//
// This is separated out in part to make it easier to test the request
// dispatching logic.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "alpaca_request.h"
#include "constants.h"
#include "device_info.h"
#include "json_response.h"

namespace alpaca {

// TODO(jamessynge): Augment the API with support for decoding device-type
// specific info, such as the ASCOM method name and custom parameter names and
// values. In particular, a method for determining whether the current parameter
// name (maybe already converted to an enum) is one that the current device
// desires to have kept given the HTTP method and ASCOM method; other parameters
// can then be discarded as uninteresting. TBD how we can express how such a
// value is stored; for example, we could just have a map from enum to string
// value, with the DeviceInterface impl responsible for converting the string to
// any more specific type that is needed). Alternately, the method for
// specifying whether the parameter should be kept could also be responsible for
// returning the desired data type (e.g. string, int, double).

class DeviceInterface {
 public:
  DeviceInterface() {}
  virtual ~DeviceInterface() {}

  virtual const DeviceInfo& device_info() const = 0;

  // Called to initialize the handler and underlying device.
  virtual void Initialize() = 0;

  // Called periodically to enable the device to perform long running
  // operations (e.g. to measure the temperature on some schedule and
  // accumulate the readings to produce an average value).
  virtual void MaintainDevice() = 0;

  // Optionally add some content to specified section of the HTML home page
  // page. We can expect every device to add at least a brief description of
  // itself (e.g. name, device type, device number, and unique id), though it is
  // best if the device adds additional info; for example, an Observing
  // Conditions device could display the current temperature. An advantage of
  // this approach is that the user can be provided with information about all
  // devices in a single round trip, rather than requiring many round trips to
  // fetch properties one at a time.
  virtual void AddToHomePageHtml(const AlpacaRequest& request,
                                 EHtmlPageSection section,
                                 mcucore::OPrintStream& strm) = 0;

  // Handles an ASCOM Device API (i.e. /setup/v1/...). Returns true to indicate
  // that the response was written without error and the connection can remain
  // open (which should depend in part on request.do_close); otherwise returns
  // false, in which case the connection to the client will be closed.
  // TODO(jamessynge): Support returning a webpage stored on the sdcard.
  // TODO(jamessynge): Design a file system layout to support that, or require
  // that the DeviceInfo provide the path to the file, or more likely both.
  virtual bool HandleDeviceSetupRequest(const AlpacaRequest& request,
                                        Print& out) = 0;

  // Handles ASCOM Alpaca Device API requests, i.e. requests of this form:
  //
  //     /api/v1/{device_type}/{device_number}/{method}
  //
  // Returns true to indicate that a response has been successfully written
  // without error to 'out' and that additional requests from the client may be
  // decoded; returns false to indicate that the client connection should be
  // closed.
  virtual bool HandleDeviceApiRequest(const AlpacaRequest& request,
                                      Print& out) = 0;
};

class ConfiguredDevicesResponse : public JsonMethodResponse {
 public:
  ConfiguredDevicesResponse(const AlpacaRequest& request,
                            mcucore::ArrayView<DeviceInterface*> devices)
      : JsonMethodResponse(request), devices_(devices) {}

  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const override;

 private:
  mcucore::ArrayView<DeviceInterface*> devices_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_INTERFACE_H_
