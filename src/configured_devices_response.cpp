#include "configured_devices_response.h"

namespace alpaca {

// Generate the properties of a single object in the Values array, i.e. info
// about a single device.
class ConfiguredDevicePropertySource : public mcucore::JsonPropertySource {
 public:
  explicit ConfiguredDevicePropertySource(
      const DeviceInterface& device_interface)
      : device_interface_(device_interface) {}

  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const override {
    device_interface_.AddConfiguredDeviceTo(object_encoder);
  }

 private:
  const DeviceInterface& device_interface_;
};

// Generate the elements of the array that is the value of the Value property.
class ConfiguredDevicesResponseValue : public mcucore::JsonElementSource {
 public:
  explicit ConfiguredDevicesResponseValue(
      mcucore::ArrayView<DeviceInterface*> devices)
      : devices_(devices) {}

  void AddTo(mcucore::JsonArrayEncoder& array_encoder) const override {
    for (auto device_ptr : devices_) {
      ConfiguredDevicePropertySource property_source(*device_ptr);
      array_encoder.AddObjectElement(property_source);
    }
  }

 private:
  mcucore::ArrayView<DeviceInterface*> devices_;
};

void ConfiguredDevicesResponse::AddTo(
    mcucore::JsonObjectEncoder& object_encoder) const {
  // Add the Value property first.
  ConfiguredDevicesResponseValue value(devices_);
  object_encoder.AddArrayProperty(ProgmemStringViews::Value(), value);

  // Then the remaining fields.
  JsonMethodResponse::AddTo(object_encoder);
}

}  // namespace alpaca
