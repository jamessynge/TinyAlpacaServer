#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_SWITCH_INTERFACE_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_SWITCH_INTERFACE_H_

// Defines the interface that an individual switch must implement.
//
// Author: james.synge@gmail.com

#include "alpaca_request.h"
#include "experimental/users/jamessynge/arduino/mcucore/src/mcucore_platform.h"
#include "utils/status.h"
#include "utils/status_or.h"

namespace alpaca {

// Represents a single switch, not a group of switches.
class SwitchInterface {
 public:
  virtual ~SwitchInterface() {}

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
                                          Print& out) = 0;

  // Gets the name of the specified switch device.
  virtual bool HandleGetSwitchName(const AlpacaRequest& request,
                                   Print& out) = 0;

  // Sets the name of the specified switch device. The switch name has not been
  // extracted from the request, nor validated.
  virtual bool HandleSetSwitchName(const AlpacaRequest& request,
                                   Print& out) = 0;

  //////////////////////////////////////////////////////////////////////////////
  // Accessors for various values.

  // Reports if the specified switch device can be written to, default true.
  // This is false if the device cannot be written to, for example a limit
  // switch or a sensor. Must be implemented, must not return
  // ActionNotImplemented.
  virtual bool GetCanWrite() = 0;

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
  virtual StatusOr<bool> GetSwitch() = 0;

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
  virtual StatusOr<double> GetSwitchValue() = 0;

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
  virtual double GetMinSwitchValue() = 0;

  // Gets the maximum value of the specified switch device as a double. Must be
  // implemented.
  virtual double GetMaxSwitchValue() = 0;

  // Returns the step size that this device supports (the difference between
  // successive values of the device). Must be implemented.
  virtual double GetSwitchStep() = 0;

  //////////////////////////////////////////////////////////////////////////////
  // Setters for mutating requests.

  // Sets a switch controller device to the specified state, true or false.
  // Returns MethodNotImplementedException if CanWrite is False. GetSwitchValue
  // must return MaxSwitchValue if the set state is true and MinSwitchValue if
  // the set state is false.
  virtual Status SetSwitch(bool state) = 0;

  // Sets a switch device value to the specified value, whose value has been
  // verified to be between MinSwitchValue and MaxSwitchValue. A set value that
  // is intermediate between the values specified by SwitchStep should result in
  // the device being set to an achievable value close to the requested set
  // value.
  virtual Status SetSwitchValue(double value) = 0;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_SWITCH_INTERFACE_H_
