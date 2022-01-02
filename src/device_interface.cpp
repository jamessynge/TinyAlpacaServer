#include "device_interface.h"

#include "device_info.h"
#include "json_encoder.h"
#include "json_encoder_helpers.h"
#include "mcucore_platform.h"

namespace alpaca {

class ConfiguredDevicesResponseValue : public mcucore::JsonElementSource {
 public:
  explicit ConfiguredDevicesResponseValue(
      mcucore::ArrayView<DeviceInterface*> devices)
      : devices_(devices) {}

  void AddTo(mcucore::JsonArrayEncoder& array_encoder) const override {
    for (auto device_ptr : devices_) {
      mcucore::JsonPropertySourceAdapter<DeviceInfo> property_source(
          device_ptr->device_info());
      array_encoder.AddObjectElement(property_source);
    }
  }

 private:
  mcucore::ArrayView<DeviceInterface*> devices_;
};

void ConfiguredDevicesResponse::AddTo(
    mcucore::JsonObjectEncoder& object_encoder) const {
  ConfiguredDevicesResponseValue value(devices_);
  object_encoder.AddArrayProperty(ProgmemStringViews::Value(), value);
  JsonMethodResponse::AddTo(object_encoder);
}

}  // namespace alpaca
