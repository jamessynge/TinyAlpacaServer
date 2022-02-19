#include "device_info.h"

#include <McuCore.h>

#include "constants.h"
#include "literals.h"

namespace alpaca {

void DeviceInfo::AddTo(mcucore::JsonObjectEncoder& object_encoder) const {
  object_encoder.AddStringProperty(ProgmemStringViews::DeviceName(), name);

  // TODO(jamessynge): Check on the case requirements of the device type's name.
  object_encoder.AddStringProperty(ProgmemStringViews::DeviceType(),
                                   ToFlashStringHelper(device_type));

  object_encoder.AddUIntProperty(ProgmemStringViews::DeviceNumber(),
                                 device_number);

  object_encoder.AddStringProperty(ProgmemStringViews::UniqueID(), unique_id);
}

}  // namespace alpaca
