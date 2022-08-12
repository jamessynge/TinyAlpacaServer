#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_DESCRIPTION_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_DESCRIPTION_H_

// DeviceDescription provides fixed data used to respond to many of Alpaca's
// "Common ASCOM Methods", and to /management/v1/configureddevices requests.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "constants.h"

namespace alpaca {

// There must be one instance of DeviceDescription per device in a sketch.
struct DeviceDescription {
  // Write the ConfiguredDevices description of this server to the specified
  // mcucore::JsonObjectEncoder. The encoder should be for the nested object
  // that is the value of the "Value" property of the response object, NOT the
  // outermost object that is the body of the response to /man
  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const;

  // Get the UUID for this device; this may require generating it, and storing
  // it in EEPROM, if it isn't yet stored in EEPROM.
  mcucore::StatusOr<mcucore::Uuid> GetOrCreateUniqueId(
      mcucore::EepromTlv& tlv) const;

  // As above, but calls EepromTlv::GetIfValid to get the tlv value.
  mcucore::StatusOr<mcucore::Uuid> GetOrCreateUniqueId() const;

  mcucore::Status SetUuidForTest(mcucore::EepromTlv& tlv,
                                 const mcucore::Uuid& uuid) const;

  // One of the supported ASCOM Devices types such as Telescope, Camera,
  // Focuser, etc.
  EDeviceType device_type;

  // The device number that must be used to access this device through the
  // Alpaca Device API. The first device of each device_type must have
  // device_number zero, the next must have device number one, and so on.
  // Note that it is kind of silly to use a 32-bit integer for this value; while
  // it is the range that ASCOM Alpaca allows, an embedded device isn't likely
  // to have more than a handful of devices in total, so a uint8_t should be
  // sufficient.
  uint32_t device_number;

  // To allow for storing info about a device in EEPROM, each device must have
  // a unique EepromDomain. If the value is changed, all access to that domain's
  // data in the EEPROM is lost.
  mcucore::EepromDomain domain;

  // A short name for this device that a user would expect to see in a list of
  // available devices. Returned in the ConfiguredDevicesResponse and in the
  // response to the /name method of the device API.
  mcucore::ProgmemString name;

  // The description of the device. Returned in response to the /description
  // method of the device API.
  mcucore::ProgmemString description;

  // The description of the device driver. Returned in response to the
  // /driverinfo method of the device API.
  mcucore::ProgmemString driver_info;

  // The driver version (a string containing only the major and minor version of
  // the driver). Returned in response to the /driverversion method of the
  // device API.
  mcucore::ProgmemString driver_version;

  // The list of device-specific action names that the device supports. This is
  // returned in the response to the /supportedactions method of the device API.
  mcucore::ProgmemStringArray supported_actions;

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
  // only) version of the ASCOM Alpaca API as of April 2021. To that end, I've
  // decided to represent this as a function returning that value, rather than
  // as a value that can be different for different devices. This will make it
  // possible to find all the places referencing the version via search, while
  // just hard coding it to one anywhere that needed the value would make that
  // much harder.
  static constexpr uint8_t interface_version() { return 1; }
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_DESCRIPTION_H_
