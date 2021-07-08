#include "device_info.h"

#include "constants.h"
#include "literals.h"
#include "utils/json_encoder.h"

namespace alpaca {

void DeviceInfo::AddTo(JsonObjectEncoder& object_encoder) const {
  object_encoder.AddStringProperty(Literals::Name(), name);

  // TODO(jamessynge): Check on the case requirements of the device type's name.
  object_encoder.AddStringProperty(Literals::DeviceType(),
                                   ToFlashStringHelper(device_type));

  object_encoder.AddUIntProperty(Literals::DeviceNumber(), device_number);

  object_encoder.AddStringProperty(Literals::UniqueID(), unique_id);
}

}  // namespace alpaca
