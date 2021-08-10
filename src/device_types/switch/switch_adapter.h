#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_SWITCH_ADAPTER_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_SWITCH_ADAPTER_H_

// Implements the common logic for dispatching Switch device requests to action
// specific methods. Switches are numbered from 0 to MaxSwitch - 1, i.e. that is
// the range the request.id field must be in for those methods which accept an
// ID parameter.
//
// The ASCOM Switch interface is used to define a number of 'switch devices'. A
// switch device can be used to control something, such as a power switch or may
// be used to sense the state of something, such as a limit switch.
//
// The MinSwitchValue, MaxSwitchValue and SwitchStep methods are used to define
// the range and values that a device can handle. This also defines the number
// of different values - states - that a device can have, from two for a
// traditional on-off switch, through those with a small number of states to
// those which have many states.
//
// The SetSwitchValue and GetSwitchValue methods are used to set and get the
// value of a device as a double.
//
// There is no fundamental difference between devices with different numbers of
// states.
//
// Naming Conventions
//
// Each device handled by a Switch is known as a device or switch device for
// general cases, a controller device if it can alter the state of the device
// and a sensor device if it can only be read.
//
// For convenience devices are referred to as boolean if the device can only
// have two states, and multi-state if it can have more than two values. These
// are treated the same in the interface definition.
//
// Author: james.synge@gmail.com

#include "device_types/device_impl_base.h"
#include "mcucore_platform.h"
#include "status_or.h"

namespace alpaca {

class SwitchAdapter : public DeviceImplBase {
 public:
  // The ASCOM API defines the return type of the MaxSwitch property as int16,
  // so the maximum value allowed is INT16_MAX, i.e. 2^15 - 1, or 32767.
  static constexpr uint16_t kMaxMaxSwitch = INT16_MAX;

  explicit SwitchAdapter(const DeviceInfo& device_info);
  // ~SwitchAdapter() override {}

  // Overrides of the base class methods:

  void Initialize() override;

  // Handles GET 'request', writes the HTTP response message to 'out'. Returns
  // true to indicate that the response was written without error, otherwise
  // false, in which case the connection to the client will be closed.
  //
  // For device specific methods (e.g. "/maxswitch"), gets the value to be
  // returned to the client by calling the appropriate GetXyz() method below
  // (e.g. GetHumidity). For other (ASCOM common) methods (e.g. "/driverinfo"),
  // delegates to the base classes' HandleGetRequest method.
  bool HandleGetRequest(const AlpacaRequest& request, Print& out) override;

  // Handles PUT 'request', writes the HTTP response message to 'out'. Returns
  // true to indicate that the response was written without error, otherwise
  // false, in which case the connection to the client will be closed.
  //
  // Delegates to the appropriate device and request specific method (e.g.
  // "/setswitch" PUT requests are passed to HandlePutCloseCover). The
  // delegatee is responsible for further validating the request (e.g. that any
  // required parameters are provided). Requests that are not specific to this
  // device type are delegated to the base class, DeviceImplBase.
  bool HandlePutRequest(const AlpacaRequest& request, Print& out) override;

  //////////////////////////////////////////////////////////////////////////////
  // Delegatees from the above methods which allow the method to write the full
  // response to out. These return true to indicate that the response was
  // written without error, otherwise false, in which case the connection to the
  // client will be closed. The switch_id argument passed to the methods below
  // has already been validated.

  // Gets the description of the specified switch device. This is to allow a
  // fuller description of the device to be returned, for example for a tool
  // tip.
  virtual bool HandleGetSwitchDescription(const AlpacaRequest& request,
                                          uint16_t switch_id, Print& out) = 0;

  // Gets the name of the specified switch device.
  virtual bool HandleGetSwitchName(const AlpacaRequest& request,
                                   uint16_t switch_id, Print& out) = 0;

  // Sets the name of the specified switch device. The switch name has not been
  // extracted from the request, nor validated.
  virtual bool HandleSetSwitchName(const AlpacaRequest& request,
                                   uint16_t switch_id, Print& out) = 0;

  //////////////////////////////////////////////////////////////////////////////
  // Accessors for various values.

  // Returns the number of switch devices managed by this driver.
  virtual uint16_t GetMaxSwitch() = 0;

  // Reports if the specified switch device can be written to, default true.
  // This is false if the device cannot be written to, for example a limit
  // switch or a sensor.  Must be implemented, must not return
  // ActionNotImplemented.
  virtual bool GetCanWrite(uint16_t switch_id) = 0;

  // Returns the state of switch device id as a boolean. Must be implemented,
  // must not throw a MethodNotImplementedException.
  //
  // A multi-state device will return true if the device is at the maximum
  // value, false if the value is at the minumum and either true or false as
  // specified by the driver developer for intermediate values.
  //
  // Some devices do not support reading their state although they do allow
  // state to be set. In these cases, on startup, the driver can not know the
  // hardware state and it is recommended that the driver either:
  //
  // a) Sets the device to a known state on connection.
  // b) Returns an InvalidOperationException until the client software has set
  //    the device state for the first time.
  //
  // In both cases the driver should save a local copy of the state which it
  // last set and return this through GetSwitch and GetSwitchValue.
  virtual mcucore::StatusOr<bool> GetSwitch(uint16_t switch_id) = 0;

  // Returns the value of switch device id as a double, which is expected to be
  // between MinSwitchValue and MaxSwitchValue. Must be implemented, must
  // not throw a MethodNotImplementedException.
  //
  // Some devices do not support reading their state although they do allow
  // state to be set. In these cases, on startup, the driver can not know
  // the hardware state and it is recommended that the driver either:
  //
  // a) Sets the device to a known state on connection.
  // b) Returns an InvalidOperationException until the client software has set
  //    the device state for the first time.
  //
  // In both cases the driver should save a local copy of the state which it
  // last set and return this through GetSwitch and GetSwitchValue.
  virtual mcucore::StatusOr<double> GetSwitchValue(uint16_t switch_id) = 0;

  //////////////////////////////////////////////////////////////////////////////
  // SwitchStep, MinSwitchValue and MaxSwitchValue can be used to determine the
  // way the device is controlled and/or displayed, for example by setting the
  // number of decimal places or number of states for a display. SwitchStep must
  // be greater than zero and the number of steps can be calculated as:
  //
  //  ((MaxSwitchValue - MinSwitchValue) / SwitchStep) + 1.
  //
  // The switch range (MaxSwitchValue - MinSwitchValue) must be an exact
  // multiple of SwitchStep.
  //
  // If a two state device cannot report its state, SwitchStep should return the
  // value 1.0.

  // Gets the minimum value of the specified switch device as a double. Must be
  // implemented.
  virtual double GetMinSwitchValue(uint16_t switch_id) = 0;

  // Gets the maximum value of the specified switch device as a double. Must be
  // implemented.
  virtual double GetMaxSwitchValue(uint16_t switch_id) = 0;

  // Returns the step size that this device supports (the difference between
  // successive values of the device). Must be implemented.
  virtual double GetSwitchStep(uint16_t switch_id) = 0;

  //////////////////////////////////////////////////////////////////////////////
  // Setters for mutating requests.

  // Sets a switch controller device to the specified state, true or false.
  // Returns MethodNotImplementedException if CanWrite is False. GetSwitchValue
  // must return MaxSwitchValue if the set state is true and MinSwitchValue if
  // the set state is false.
  virtual mcucore::Status SetSwitch(uint16_t switch_id, bool state) = 0;

  // Sets a switch device value to the specified value, whose value has been
  // verified to be between MinSwitchValue and MaxSwitchValue. A set value that
  // is intermediate between the values specified by SwitchStep should result in
  // the device being set to an achievable value close to the requested set
  // value.
  virtual mcucore::Status SetSwitchValue(uint16_t switch_id, double value) = 0;

 protected:
  // Returns true if the request has a valid ID parameter, else false, in which
  // case handler_ret is set to the value to be returned by HandleGetRequest or
  // HandlePutRequest.
  bool ValidateSwitchIdParameter(const AlpacaRequest& request, Print& out,
                                 bool& handler_ret);
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_SWITCH_ADAPTER_H_
