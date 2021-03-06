// TODO(jamessynge): Describe why this file exists/what it provides.

#include "device_info.h"

#include "constants.h"
#include "literals.h"
#include "utils/json_encoder.h"

namespace alpaca {

// TODO(jamessynge): Check on the case requirements of the device type.
StringView DeviceTypeToLiteral(EDeviceType) { return StringView("TBD"); }

void ConfiguredDeviceInfo::AddTo(JsonObjectEncoder& object_encoder) const {
  object_encoder.AddStringProperty(Literals::Name(), name);

  object_encoder.AddStringProperty(Literals::DeviceType(),
                                   DeviceTypeToLiteral(device_type));
  object_encoder.AddIntegerProperty(Literals::DeviceNumber(), device_number);

  // TODO(jamessynge): Add code for formatting a number as a string... or add
  // the necessary UUID class.
  object_encoder.AddStringProperty(Literals::UniqueID(),
                                   DeviceTypeToLiteral(device_type));
}

}  // namespace alpaca
