#include "device_info.h"

#include "constants.h"
#include "literals.h"
#include "utils/json_encoder.h"

namespace alpaca {

void DeviceInfo::AddTo(JsonObjectEncoder& object_encoder) const {
  object_encoder.AddStringProperty(Literals::Name(), name);

  // TODO(jamessynge): Check on the case requirements of the device type's name.
  object_encoder.AddStringProperty(Literals::DeviceType(),
                                   ToPrintableProgmemString(device_type));

  object_encoder.AddUIntProperty(Literals::DeviceNumber(), device_number);

  // TODO(jamessynge): Add ability to produce a UUID based on multiple factors,
  // including at least the device type and config_id. Alternatively, we might
  // use the device setup UI to allow the user to provide the UUID or randomness
  // source for the UUID. If based on additional, perhaps non-constant factors
  // such as the time when the UUID is first generated, then we might choose to
  // store the UUID in EEPROM.
  object_encoder.AddStringProperty(Literals::UniqueID(),
                                   AnyPrintable(config_id));
}

}  // namespace alpaca
