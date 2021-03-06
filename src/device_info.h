#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_INFO_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_INFO_H_

// DeviceInfo provides data used used to respond to "Common ASCOM Methods", i.e.
// requests for common info about a device, and it also provides the list of
// support custom actions (also accessed via the common ASCOM methods).
#include "constants.h"
#include "utils/json_encoder.h"
#include "utils/literal.h"
#include "utils/platform.h"
#include "utils/string_view.h"

namespace alpaca {

// Stores the fields to be returned in the response to a configureddevices
// request, and provides a method for adding that data to the Value array.
struct ConfiguredDeviceInfo : public JsonPropertySource {
  ConfiguredDeviceInfo(Literal name, EDeviceType device_type,
                       uint32_t device_number, uint32_t config_id)
      : name(name),
        device_type(device_type),
        device_number(device_number),
        config_id(config_id) {}

  // Write the ConfiguredDevices description of this server to the specified
  // JsonObjectEncoder. The encoder should be for a nested object in the array
  // that is the value of the "Value" property of the response object
  void AddTo(JsonObjectEncoder& object_encoder) const override;

  const Literal name;
  const EDeviceType device_type;
  const uint32_t device_number;

  // The config_id is a random number generated when a device is added, when the
  // *type(s)* of device(s) used changes, or perhaps when calibration parameters
  // have been changed such that the values shouldn't be compared with prior
  // values from this device. The config_id can be used, along with other info,
  // to generate a UUID for the device, for use as its UniqueId.
  const uint32_t config_id;
};

// There should be one instance of DeviceInfo per in a sketch.
//
//
struct DeviceInfo {
  // Write the ConfiguredDevices description of this server to the specified
  // JsonObjectEncoder. The encoder should be for the nested object that is the
  // value of the "Value" property of the response object, NOT the outermost
  // object that is the body of the response to /man
  //
  // Why is this not a vir
  void AddTo(JsonObjectEncoder& object_encoder) const;

  const EDeviceType device_type;
  const uint32_t device_number;
  const Literal name;
  const Literal description;
  const Literal driver_info;
  const Literal driver_version;
  const int16_t interface_version;
  const LiteralArray supported_actions;

  // The config_id is a random number generated when a device is added, when the
  // *type(s)* of device(s) used changes, or perhaps when calibration parameters
  // have been changed such that the values shouldn't be compared with prior
  // values from this device. The config_id can be used, along with other info,
  // to generate a UUID for the device, for use as its UniqueId.
  const uint32_t config_id;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_INFO_H_
