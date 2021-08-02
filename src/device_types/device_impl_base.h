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

#include "alpaca_request.h"
#include "ascom_error_codes.h"
#include "device_info.h"
#include "device_interface.h"
#include "experimental/users/jamessynge/arduino/mcucore/src/mcucore_platform.h"
#include "utils/status.h"
#include "utils/status_or.h"

namespace alpaca {

class DeviceImplBase : public DeviceInterface {
 public:
  explicit DeviceImplBase(const DeviceInfo& device_info)
      : device_info_(device_info) {}
  ~DeviceImplBase() override {}

  // Overrides of the base class methods:
  const DeviceInfo& device_info() const override { return device_info_; }
  EDeviceType device_type() const override { return device_info_.device_type; }
  uint32_t device_number() const override { return device_info_.device_number; }
  void Initialize() override {}
  void MaintainDevice() override {}
  size_t GetUniqueBytes(uint8_t* buffer, size_t buffer_size) override {
    return 0;
  }

  // Writes an error response to out and returns false.
  bool HandleDeviceSetupRequest(const AlpacaRequest& request,
                                Print& out) override;

  // Handles ASCOM Alpaca Device API requests, i.e. requests of this form:
  //
  //     /api/v1/{device_type}/{device_number}/{method}
  //
  // Returns true to indicate that a response has been successfully written to
  // 'out' and that additional requests from the client may be decoded; returns
  // false to indicate that the client connection should be closed.
  //
  // GET and HEAD requests are delegated to HandleGetRequest, and PUT requests
  // are delegated to HandlePutRequest. For any other method, writes an error
  // response to out and returns false.
  bool HandleDeviceApiRequest(const AlpacaRequest& request,
                              Print& out) override;

 protected:
  // Additional methods provided by this class, can be overridden by subclass.

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
  virtual StatusOr<bool> GetConnected();

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
  // writes a response based on the returned Status value.
  virtual bool HandlePutConnected(const AlpacaRequest& request, Print& out);

  // Connect to the device if value is true, disconnect if value is false.
  // Default implementation returns an error.
  virtual Status SetConnected(bool value);

 private:
  const DeviceInfo& device_info_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_DEVICE_IMPL_BASE_H_
