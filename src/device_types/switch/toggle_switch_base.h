#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_TOGGLE_SWITCH_BASE_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_TOGGLE_SWITCH_BASE_H_

// ToggleSwitchBase is an abstract implementation of SwitchInterface for a
// single switch device with only two states (i.e. a simple toggle switch); it
// implements just those methods that treat the switch value as a double,
// mapping them to a boolean.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "device_types/switch/switch_interface.h"

namespace alpaca {

class ToggleSwitchBase : public SwitchInterface {
 public:
  ~ToggleSwitchBase() override;

  // Returns 0 if the switch is off, 1 if the switch is on.
  mcucore::StatusOr<double> GetSwitchValue() final;

  // Returns 0.
  double GetMinSwitchValue() final;

  // Returns 1.
  double GetMaxSwitchValue() final;

  // Returns 1.
  double GetSwitchStep() final;

  // Calls SetSwitch(value >= 0.5).
  mcucore::Status SetSwitchValue(double value) final;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_SWITCH_TOGGLE_SWITCH_BASE_H_
