#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_INFO_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_INFO_H_

// DeviceInfo provides fixed data used to respond to many of Alpaca's "Common
// ASCOM Methods", and to /management/v1/configureddevices requests.
//
// TODO(jamessynge): Rename DeviceInfo to DeviceDescription, which better
// matches ServerDescription.
//
// Author: james.synge@gmail.com

#include "constants.h"
#include "experimental/users/jamessynge/arduino/mcucore/src/mcucore_platform.h"
#include "experimental/users/jamessynge/arduino/mcucore/src/progmem_string.h"
#include "utils/json_encoder.h"
#include "utils/literal.h"
#include "utils/string_view.h"

namespace alpaca {

// There must be one instance of DeviceInfo per device in a sketch.
struct DeviceInfo {
  // Write the ConfiguredDevices description of this server to the specified
  // JsonObjectEncoder. The encoder should be for the nested object that is the
  // value of the "Value" property of the response object, NOT the outermost
  // object that is the body of the response to /man
  void AddTo(JsonObjectEncoder& object_encoder) const;

  // One of the supported ASCOM Devices types such as Telescope, Camera,
  // Focuser, etc.
  EDeviceType device_type;

  // The device number that must be used to access this device through the
  // Alpaca Device API. It must be locally unique among the devices of the same
  // type served by this server.
  uint32_t device_number;

  // A short name for this device that a user would expect to see in a list of
  // available devices. Returned in the ConfiguredDevicesResponse and in the
  // response to the /name method of the device API.
  ProgmemString name;

  // A string representation of a random value that uniquely identifies this
  // ASCOM device; the random value should have a minimum of 48bits of
  // randomness. Where possible a UUID / GUID should be used, but this is not
  // mandatory. Returned in the ConfiguredDevicesResponse.
  ProgmemString unique_id;

  // The description of the device. Returned in response to the /description
  // method of the device API.
  ProgmemString description;

  // The description of the device driver. Returned in response to the
  // /driverinfo method of the device API.
  ProgmemString driver_info;

  // The driver version (a string containing only the major and minor version of
  // the driver). Returned in response to the /driverversion method of the
  // device API.
  ProgmemString driver_version;

  // The list of device-specific action names that the device supports. This is
  // returned in the response to the /supportedactions method of the device API.
  LiteralArray supported_actions;

  // The ASCOM Device interface version number that this device supports.
  // Returned in response to the /interfaceversion method of the device API;
  // that method gets the version of the ASCOM device interface contract to
  // which the device complies. Only one interface version is current at a
  // moment in time and all new devices should be built to the latest interface
  // version. Applications can choose which device interface versions they
  // support and it is in their interest to support previous versions as well as
  // the current version to ensure thay can use the largest number of devices.
  //
  // That can be summarized as saying that we should return 1, the current (and
  // only) version of the ASCOM Alpaca API as of April 2021. I'd like to provide
  // a default value here for the field, but the Arduino IDE's compiler (g++ for
  // approximately C++ 11) doesn't tolerate that when I use designated
  // initializers (a C++ 20 feature that g++ is supporting).
  uint8_t interface_version;

#if 0
  // MAY want to use this to allow the generation of a UUID based on MAC, device
  // type and config_id. That has the advantage an over including a UUID in the
  // code: it will be different for each instance using the same code, assuming
  // that the instance has a separate MAC addresses.
  uint32_t config_id;
#endif
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_INFO_H_
