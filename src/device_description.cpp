#include "device_description.h"

#include <McuCore.h>

#include "constants.h"
#include "eeprom_ids.h"
#include "literals.h"

namespace alpaca {
namespace {
using ::mcucore::EepromTlv;
using ::mcucore::Uuid;

size_t PrintUuid(Print& out, const void* data) {
  auto* uuid = reinterpret_cast<const Uuid*>(data);
  return uuid->printTo(out);
}
}  // namespace

void DeviceDescription::AddConfiguredDeviceTo(
    mcucore::JsonObjectEncoder& object_encoder, EepromTlv& tlv) const {
  object_encoder.AddStringProperty(ProgmemStringViews::DeviceName(), name);

  // TODO(jamessynge): Check on the case requirements of the device type's name.
  object_encoder.AddStringProperty(ProgmemStringViews::DeviceType(),
                                   ToFlashStringHelper(device_type));

  object_encoder.AddUIntProperty(ProgmemStringViews::DeviceNumber(),
                                 device_number);

  auto status_or_uuid = GetOrCreateUniqueId(tlv);
  if (!status_or_uuid.ok()) {
    MCU_DCHECK_OK(status_or_uuid)
        << MCU_PSD("Should have been able to GetOrCreateUniqueId");
  } else {
    auto& uuid = status_or_uuid.value();
    mcucore::AnyPrintable unique_id(PrintUuid, &uuid);
    object_encoder.AddStringProperty(ProgmemStringViews::UniqueID(), unique_id);
  }
}

mcucore::StatusOr<Uuid> DeviceDescription::GetOrCreateUniqueId(
    EepromTlv& tlv) const {
  mcucore::EepromTag tag{.domain = domain, .id = kUniqueIdTagId};
  Uuid uuid;
  MCU_RETURN_IF_ERROR(uuid.ReadOrStoreEntry(tlv, tag));
  return uuid;
}

mcucore::Status DeviceDescription::SetUuidForTest(EepromTlv& tlv,
                                                  const Uuid& uuid) const {
  mcucore::EepromTag tag{.domain = domain, .id = kUniqueIdTagId};
  return uuid.WriteToEeprom(tlv, tag);
}

}  // namespace alpaca
