#include "device_interface.h"

#include "device_info.h"
#include "experimental/users/jamessynge/arduino/mcucore/src/mcucore_platform.h"
#include "utils/json_encoder.h"
#include "utils/json_encoder_helpers.h"

namespace alpaca {

class ConfiguredDevicesResponseValue : public JsonElementSource {
 public:
  explicit ConfiguredDevicesResponseValue(ArrayView<DeviceInterface*> devices)
      : devices_(devices) {}

  void AddTo(JsonArrayEncoder& array_encoder) const override {
    for (auto device_ptr : devices_) {
      JsonPropertySourceAdapter<DeviceInfo> property_source(
          device_ptr->device_info());
      array_encoder.AddObjectElement(property_source);
    }
  }

 private:
  ArrayView<DeviceInterface*> devices_;
};

void ConfiguredDevicesResponse::AddTo(JsonObjectEncoder& object_encoder) const {
  ConfiguredDevicesResponseValue value(devices_);
  object_encoder.AddArrayProperty(Literals::Value(), value);
  JsonMethodResponse::AddTo(object_encoder);
}

}  // namespace alpaca
