#include "device_types/switch/toggle_switch_base.h"

#include <McuCore.h>

namespace alpaca {

ToggleSwitchBase::~ToggleSwitchBase() {}

mcucore::StatusOr<double> ToggleSwitchBase::GetSwitchValue() {
  auto status_or_on = GetSwitch();
  if (status_or_on.ok()) {
    if (status_or_on.value()) {
      return 1.0;
    } else {
      return 0.0;
    }
  }
  return status_or_on.status();
}

double ToggleSwitchBase::GetMinSwitchValue() { return 0; }

double ToggleSwitchBase::GetMaxSwitchValue() { return 1; }

double ToggleSwitchBase::GetSwitchStep() { return 1; }

mcucore::Status ToggleSwitchBase::SetSwitchValue(double value) {
  return SetSwitch(value >= 0.5);
}

}  // namespace alpaca
