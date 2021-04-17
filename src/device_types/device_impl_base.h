#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_DEVICE_IMPL_BASE_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_DEVICE_IMPL_BASE_H_

// This is the base class for device specific HTTP request handlers. Handles
// some common Alpaca API methods, and delegates the remainder to the subclass.
// In particular, supports requests with paths matching
//
//      /api/v1/{device_type}/{device_number}/{method_name}
//      /setup/v1/{device_type}/{device_number}/setup
//
//  There is no support for incrementally returning responses, so those need to
//  be small enough that it can fit in the buffers available via 'out' (e.g. at
//  most a few Ethernet frames as provided by a WIZ5500).
//
// Author: james.synge@gmail.com

#include "alpaca_request.h"
#include "ascom_error_codes.h"
#include "device_info.h"
#include "device_interface.h"
#include "utils/platform.h"
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
  void Update() override {}
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
  // device metadata inquiry methods, such as
  // /interfaceversion and /supportedactions, which can be answered using the
  // DeviceInfo instance passed to the constructor.

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

  // Is the driver connected to (i.e. able to talk to) the device?
  virtual StatusOr<bool> GetConnected() {
    return ErrorCodes::ActionNotImplemented();
  }

  // Connect to the device if value is true, disconnect if value is false.
  virtual Status SetConnected(bool value) {
    return ErrorCodes::ActionNotImplemented();
  }

 private:
  const DeviceInfo& device_info_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_DEVICE_IMPL_BASE_H_
