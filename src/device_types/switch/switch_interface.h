#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_SWITCH_INTERFACE_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_SWITCH_INTERFACE_H_

// Defines the interface that an individual switch of a MultiSwitchAdapter must
// implement. There are some default implementations for name and descriptions.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "alpaca_request.h"

namespace alpaca {

// Represents a single switch, not a group of switches.
//
// All methods that take instances of AlpacaRequest and Print as args, and
// return a bool have the following in common:
//
// * Writes an HTTP response with an ASCOM Alpaca specified body (JSON object),
//   with the requested value in the Value field of the object.
// * Returns true if the response was successfully written and the connection
//   does not need to be closed; else returns false if the connection should be
//   closed.
//
// If instead the return type is StatusOr<T> or Status, then the method returns
// OK if the operation was correctly and successfully performed, else returns an
// error that can be used by the caller to generate an error response.
class SwitchInterface {
 public:
  virtual ~SwitchInterface() {}

  //////////////////////////////////////////////////////////////////////////////
  // Name and description getters and setters. Not sure why description can't be
  // set using the ASCOM API (see https://ascom-standards.org/api), but I've
  // included it here for completeness. The default implementations of the
  // getters produce minimal responses, and the setters return a not implemented
  // error.

  // Writes an HTTP response whose value is the description of the switch.
  virtual bool HandleGetSwitchDescription(const AlpacaRequest& request,
                                          Print& out) = 0;

  // Writes an HTTP response whose value is the name of the switch.
  virtual bool HandleGetSwitchName(const AlpacaRequest& request,
                                   Print& out) = 0;

  // Sets the description of the specified switch.
  virtual mcucore::Status HandleSetSwitchDescription(
      const AlpacaRequest& request, const mcucore::StringView& switch_name) = 0;

  // Sets the name of the specified switch.
  virtual mcucore::Status HandleSetSwitchName(
      const AlpacaRequest& request, const mcucore::StringView& switch_name) = 0;

  //////////////////////////////////////////////////////////////////////////////
  // Returns true if the specified switch can be written to (i.e. changed), else
  // returns false if the switch cannot be changed via this API; this might be
  // the case if the switch represents a device whose state we are able read,
  // but can't set, such as a limit switch or light meter, or if there is a
  // hardware lockout preventing the value of the switch being changed.
  virtual mcucore::StatusOr<bool> CanWrite() = 0;

  // Returns the state of switch device id as a boolean.
  //
  // From the ASCOM specification:
  //
  // A multi-state device will return true if the device is at the maximum
  // value, false if the value is at the minimum and either true or false as
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
  virtual mcucore::StatusOr<bool> Switch() = 0;

  // Returns the value of switch device id as a double, which is expected to be
  // between MinSwitchValue and MaxSwitchValue.
  //
  // From the ASCOM specification:
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
  virtual mcucore::StatusOr<double> SwitchValue() = 0;

  //////////////////////////////////////////////////////////////////////////////
  // From the ASCOM specification:
  //
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

  // Gets the minimum value of the specified switch device as a double.
  virtual mcucore::StatusOr<double> MinSwitchValue() = 0;

  // Gets the maximum value of the specified switch device as a double.
  virtual mcucore::StatusOr<double> MaxSwitchValue() = 0;

  // Returns the step size that this device supports (the difference between
  // successive values of the device).
  virtual mcucore::StatusOr<double> SwitchStep() = 0;

  //////////////////////////////////////////////////////////////////////////////
  // Setters for mutating requests.

  // Sets a switch device to the specified state, true or false. Returns an
  // error if CanWrite is False. GetSwitchValue must return MaxSwitchValue if
  // the set state is true and MinSwitchValue if the set state is false.
  virtual mcucore::Status SetSwitch(bool state) = 0;

  // Sets a switch device value to the specified value, whose value has been
  // verified to be between MinSwitchValue and MaxSwitchValue. A set value that
  // is intermediate between the values specified by SwitchStep should result in
  // the device being set to an achievable value close to the requested set
  // value.
  virtual mcucore::Status SetSwitchValue(double value) = 0;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_SWITCH_INTERFACE_H_
