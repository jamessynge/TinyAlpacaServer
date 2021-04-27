#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_COVER_CALIBRATOR_COVER_CALIBRATOR_ADAPTER_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_COVER_CALIBRATOR_COVER_CALIBRATOR_ADAPTER_H_

// Implements much of the common logic for Cover Calibrator devices, dispatching
// requests to more device-type specific methods (e.g. GetBrightness).
//
// NOTE: the Alpaca API specification states that the brightness and max
// brightness return values are of type signed 32-bit integer, but that the
// brightness value that one can set is in the range 0 to max brightness,
// implying that max brightness is (quite reasonably) a positive value. I'm
// following that here, which implies that subclasses should take care to return
// sensible values.
//
// Author: james.synge@gmail.com

#include "device_types/cover_calibrator/cover_calibrator_constants.h"
#include "device_types/device_impl_base.h"
#include "utils/platform.h"
#include "utils/status_or.h"

namespace alpaca {

class CoverCalibratorAdapter : public DeviceImplBase {
 public:
  explicit CoverCalibratorAdapter(const DeviceInfo& device_info);

  // Handles GET 'request', writes the HTTP response message to 'out'. Returns
  // true to indicate that the response was written without error, otherwise
  // false, in which case the connection to the client will be closed.
  //
  // For device specific methods (e.g. "/humidity"), gets the value to be
  // returned to the client by calling the appropriate GetXyz() method below
  // (e.g. GetHumidity). For other methods (e.g. "/driverinfo"), delegates to
  // the base classes' HandleGetRequest method.
  bool HandleGetRequest(const AlpacaRequest& request, Print& out) override;

  // Handles PUT 'request', writes the HTTP response message to 'out'. Returns
  // true to indicate that the response was written without error, otherwise
  // false, in which case the connection to the client will be closed.
  //
  // Delegates to the appropriate device and request specific method (e.g.
  // "/closecover" PUT requests are passed to HandlePutCloseCover). The
  // delegatee is responsible for further validating the request (e.g. that the
  // "Brightness" parameter is provided for a "/calibratoron" PUT request, and
  // has a valid value). Requests that are not specific to this device type are
  // delegated to the base class, DeviceImplBase.
  bool HandlePutRequest(const AlpacaRequest& request, Print& out) override;

  //////////////////////////////////////////////////////////////////////////////
  // Accessors for various values. The default implementations return an
  // unimplemented error.

  // Returns the current calibrator brightness.
  virtual StatusOr<int32_t> GetBrightness();

  // Returns the state of the calibration device, or kUnknown if not overridden
  // by a subclass.
  virtual StatusOr<ECalibratorStatus> GetCalibratorState();

  // Returns the state of the device cover, or kUnknown if not overridden by a
  // subclass.
  virtual StatusOr<ECoverStatus> GetCoverState();

  // Returns the calibrator's maximum Brightness value.
  virtual StatusOr<int32_t> GetMaxBrightness();

  //////////////////////////////////////////////////////////////////////////////
  // Handlers for mutating requests (e.g. "/closecover"). These return true to
  // indicate that the response was written without error, otherwise false, in
  // which case the connection to the client will be closed.

  virtual bool HandlePutCalibratorOff(const AlpacaRequest& request, Print& out);
  virtual bool HandlePutCalibratorOn(const AlpacaRequest& request, Print& out);
  virtual bool HandlePutCloseCover(const AlpacaRequest& request, Print& out);
  virtual bool HandlePutHaltCover(const AlpacaRequest& request, Print& out);
  virtual bool HandlePutOpenCover(const AlpacaRequest& request, Print& out);

  // The above HandlePut* methods delegate to the following methods after
  // validating arguments, and generate responses based on the returned Status
  // values.
  virtual Status SetCalibratorOff();
  virtual Status SetCalibratorBrightness(uint32_t brightness);

  virtual Status MoveCover(bool open);
  virtual Status HaltCoverMotion();
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_COVER_CALIBRATOR_COVER_CALIBRATOR_ADAPTER_H_
