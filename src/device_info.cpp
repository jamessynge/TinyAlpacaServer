#include "device_info.h"

#include "constants.h"
#include "json_encoder.h"
#include "literals.h"

namespace alpaca {

void DeviceInfo::AddTo(mcucore::JsonObjectEncoder& object_encoder) const {
  object_encoder.AddStringProperty(Literals::DeviceName(), name);

  // TODO(jamessynge): Check on the case requirements of the device type's name.
  object_encoder.AddStringProperty(Literals::DeviceType(),
                                   ToFlashStringHelper(device_type));

  object_encoder.AddUIntProperty(Literals::DeviceNumber(), device_number);

  object_encoder.AddStringProperty(Literals::UniqueID(), unique_id);
}

}  // namespace alpaca
