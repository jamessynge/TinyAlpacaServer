#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_API_HANDLER_BASE_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_API_HANDLER_BASE_H_

// Class for handling common Alpaca API methods, i.e. those of the form:
//      /api/v1/{device_type}/{device_number}/name
// This is also the base class for device specific handlers.
//
//  There is no support for incrementally returning responses, so those need to
//  be small enough that it can fit in the buffers available via 'out' (e.g. at
//  most a few Ethernet frames as provided by a WIZ5500).

#include "alpaca_request.h"
#include "device_info.h"
#include "utils/platform.h"

namespace alpaca {

class DeviceApiHandlerBase {
 public:
  explicit DeviceApiHandlerBase(const DeviceInfo& device_info);
  virtual ~DeviceApiHandlerBase();

  const DeviceInfo& device_info() const { return device_info_; }
  EDeviceType device_type() const { return device_info_.device_type; }
  uint32_t device_number() const { return device_info_.device_number; }

  // Called to initialize the handler and underlying device.
  virtual void Initialize() { SetConnected(true); }

  // Called periodically to enable the device to perform long running
  // operations (e.g. to measure the temperature and accumulate an
  // average value).
  virtual void Loop() {}

  // Fill buffer with up to buffer_size unique bytes from the hardware, return
  // the number of unique bytes available. Return 0 (or don't override) if the
  // hardware can not provide any unique bytes.
  //
  // NOTE: Ideally, we'd provide a /setup page allowing the UniqueId generated
  // for a device to be cleared, i.e. when we have had to replace a sensor with
  // another of the same type. Perhaps we should do this via the "actions"
  // ASCOM method?
  virtual size_t GetUniqueBytes(uint8_t* buffer, size_t buffer_size);

  // Handles an ASCOM Device API, dispatches to the appropriate method based on
  // the HTTP method name. Returns true to indicate that the response was
  // written without error, otherwise false, in which case the connection to the
  // client will be closed.
  virtual bool HandleDeviceSetupRequest(const AlpacaRequest& request,
                                        Print& out);

  // Handles an ASCOM Device API, dispatches to the appropriate method based on
  // the HTTP method name. Returns true to indicate that the response was
  // written without error, otherwise false, in which case the connection to the
  // client will be closed.
  virtual bool HandleDeviceApiRequest(const AlpacaRequest& request, Print& out);

  // Handles a GET 'request', writing the HTTP response message to out. Returns
  // true to indicate that the response was written without error, otherwise
  // false, in which case the connection to the client will be closed.
  virtual bool HandleGetRequest(const AlpacaRequest& request, Print& out);

  // Handles a PUT 'request', writing the HTTP response message to out. Returns
  // true to indicate that the response was written without error, otherwise
  // false, in which case the connection to the client will be closed.
  virtual bool HandlePutRequest(const AlpacaRequest& request, Print& out);

  // Default implementations of common methods.

  // Is the driver connected to (i.e. able to talk to) the device?
  virtual bool GetConnected() { return false; }

  // Connect to the device if value is true, disconnect if value is false.
  virtual void SetConnected(bool value) {}

 private:
  const DeviceInfo& device_info_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_API_HANDLER_BASE_H_
