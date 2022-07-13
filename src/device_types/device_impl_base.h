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
#include "device_info.h"
#include "device_interface.h"

namespace alpaca {

class DeviceImplBase : public DeviceInterface {
 public:
  explicit DeviceImplBase(const DeviceInfo& device_info)
      : device_info_(device_info) {}
  ~DeviceImplBase() override {}

  // Overrides of the base class methods:
  const DeviceInfo& device_info() const override { return device_info_; }

  void Initialize() override {}
  void MaintainDevice() override {}
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
  // /supportedactions, which can be answered using the DeviceInfo instance
  // passed to the constructor.

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

 private:
  const DeviceInfo& device_info_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_DEVICE_IMPL_BASE_H_
