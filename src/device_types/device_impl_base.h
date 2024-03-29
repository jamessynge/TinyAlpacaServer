#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_DEVICE_IMPL_BASE_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_DEVICE_IMPL_BASE_H_

// This is a base class for device-type specific HTTP request handlers. In
// particular, supports requests with paths matching:
//
//      /api/v1/{device_type}/{device_number}/{method_name}
//      /setup/v1/{device_type}/{device_number}/setup
//
// Completely handles common Alpaca API information methods (e.g. /name), and
// takes care of extracting parameters for PUT methods, then delegates to the
// subclass.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "alpaca_request.h"
#include "device_description.h"
#include "device_interface.h"
#include "server_context.h"

namespace alpaca {

class DeviceImplBase : public DeviceInterface {
 public:
  explicit DeviceImplBase(ServerContext& server_context,
                          const DeviceDescription& device_description)
      : server_context_(server_context),
        device_description_(device_description) {}
  ~DeviceImplBase() override {}

  // Overrides of the base class methods:
  const DeviceDescription& device_description() const override {
    return device_description_;
  }

  // We don't override ResetHardware and InitializeDevice on the theory that
  // most real world devices will have some reset and initialize steps to
  // perform, and thus providing empty default implementations risks forgetting
  // to provide the appropriate implementations in the concrete subclass.
  // However, it is quite possible that there is no device specific validation
  // of the configuration to be performed, nor any periodic work to be done, so
  // we do provide a default implementations of ValidateConfiguration and
  // MaintainDevice.
  void ValidateConfiguration() override {}
  void MaintainDevice() override {}

  void AddConfiguredDeviceTo(
      mcucore::JsonObjectEncoder& object_encoder) const override;
  void AddToHomePageHtml(const AlpacaRequest& request, EHtmlPageSection section,
                         mcucore::OPrintStream& strm) override;
  bool HandleDeviceSetupRequest(const AlpacaRequest& request,
                                Print& out) override;

  // GET and HEAD requests are delegated to HandleGetRequest, and PUT requests
  // are delegated to HandlePutRequest. For any other method, writes an error
  // response to out and returns false.
  bool HandleDeviceApiRequest(const AlpacaRequest& request,
                              Print& out) override;

  // Create a tag for reading data stored in EEPROM using this device's
  // EepromDomain.
  mcucore::EepromTag MakeTag(uint8_t id);

  // Implements the core of HandleDeviceSetupRequest, i.e. writing the HTML body
  // of the response.
  virtual void WriteDeviceSetupHtml(const AlpacaRequest& request,
                                    mcucore::OPrintStream& strm) const;

 protected:
  // Additional methods provided by this class, can be overridden by subclass.

  // These Add* methods are called by AddToHomePageHtml in order to render the
  // appropriate sections of the body.
  virtual void AddStartDeviceSection(mcucore::OPrintStream& strm);
  virtual void AddDeviceBanner(mcucore::OPrintStream& strm);
  virtual void AddDeviceSummary(mcucore::OPrintStream& strm);
  // Adds any device type specific details after the device summary. This is
  // intended to be overridden.
  virtual void AddDeviceDetails(mcucore::OPrintStream& strm) {}
  virtual void AddEndDeviceSection(mcucore::OPrintStream& strm);

  // Handles a subset of the "ASCOM Alpaca Methods Common To All Devices": the
  // device metadata inquiry methods, such as /interfaceversion and
  // /supportedactions, which can be answered using the DeviceDescription
  // instance passed to the constructor.

  // Handles a GET 'request', writing the HTTP response message to out. Returns
  // true to indicate that the response was written without error and the
  // connection can remain open (which should depend in part on
  // request.do_close); otherwise returns false, in which case the connection to
  // the client will be closed.
  //
  virtual bool HandleGetRequest(const AlpacaRequest& request, Print& out);

  // Handles a PUT 'request', writing the HTTP response message to out. Returns
  // true to indicate that the response was written without error and the
  // connection can remain open (which should depend in part on
  // request.do_close); otherwise returns false, in which case the connection to
  // the client will be closed.
  //
  // Default implementation writes an error response to out and returns false.
  virtual bool HandlePutRequest(const AlpacaRequest& request, Print& out);

  // Default implementations of common methods.

  // Is the driver connected to (i.e. able to talk to) the device? By default,
  // we assume the device is connected, and prevent the device from being
  // disconnected. A subclass can override to change this.
  virtual mcucore::StatusOr<bool> GetConnected();

  // Invokes the named device-specific action.
  virtual bool HandlePutAction(const AlpacaRequest& request, Print& out);

  // Transmits an arbitrary string to the device, with no response expected.
  // Default implementation writes an error response to out and returns false.
  virtual bool HandlePutCommandBlind(const AlpacaRequest& request, Print& out);

  // Transmits an arbitrary string to the device and returns a boolean value
  // from the device. Default implementation writes an error response to out and
  // returns false.
  virtual bool HandlePutCommandBool(const AlpacaRequest& request, Print& out);

  // Transmits an arbitrary string to the device and returns a string value from
  // the device. Default implementation writes an error response to out and
  // returns false.
  virtual bool HandlePutCommandString(const AlpacaRequest& request, Print& out);

  // Sets the connected state of the device. Default implementation passes the
  // decoded boolean value of the Connected parameter to SetConnected, and
  // writes a response based on the returned mcucore::Status value.
  virtual bool HandlePutConnected(const AlpacaRequest& request, Print& out);

  // Connect to the device if value is true, disconnect if value is false.
  // Default implementation returns an error.
  virtual mcucore::Status SetConnected(bool value);

  ServerContext& server_context_;
  const DeviceDescription& device_description_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_DEVICE_IMPL_BASE_H_
