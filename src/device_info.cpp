#include "device_info.h"

#include <McuCore.h>

#include "constants.h"
#include "eeprom_ids.h"
#include "literals.h"

namespace alpaca {
namespace {
size_t PrintUuid(Print& out, const void* data) {
  auto* uuid = reinterpret_cast<const mcucore::Uuid*>(data);
  return uuid->printTo(out);
}
}  // namespace

void DeviceInfo::AddTo(mcucore::JsonObjectEncoder& object_encoder) const {
  object_encoder.AddStringProperty(ProgmemStringViews::DeviceName(), name);

  // TODO(jamessynge): Check on the case requirements of the device type's name.
  object_encoder.AddStringProperty(ProgmemStringViews::DeviceType(),
                                   ToFlashStringHelper(device_type));

  object_encoder.AddUIntProperty(ProgmemStringViews::DeviceNumber(),
                                 device_number);

  auto status_or_uuid = GetOrCreateUniqueId();
  if (!status_or_uuid.ok()) {
    MCU_DCHECK_OK(status_or_uuid)
        << MCU_FLASHSTR("Should have been able to GetOrCreateUniqueId");
  } else {
    auto& uuid = status_or_uuid.value();
    mcucore::AnyPrintable unique_id(PrintUuid, &uuid);
    object_encoder.AddStringProperty(ProgmemStringViews::UniqueID(), unique_id);
  }
}

mcucore::StatusOr<mcucore::Uuid> DeviceInfo::GetOrCreateUniqueId(
    mcucore::EepromTlv& tlv) const {
  mcucore::EepromTag tag{.domain = domain, .id = kUniqueIdTagId};
  mcucore::Uuid uuid;
  MCU_RETURN_IF_ERROR(uuid.ReadOrStoreEntry(tlv, tag));
  return uuid;
}

mcucore::StatusOr<mcucore::Uuid> DeviceInfo::GetOrCreateUniqueId() const {
  MCU_ASSIGN_OR_RETURN(auto tlv, mcucore::EepromTlv::GetIfValid());
  return GetOrCreateUniqueId(tlv);
}

mcucore::Status DeviceInfo::SetUuidForTest(mcucore::EepromTlv& tlv,
                                           const mcucore::Uuid& uuid) const {
  mcucore::EepromTag tag{.domain = domain, .id = kUniqueIdTagId};
  return uuid.WriteToEeprom(tlv, tag);
}

}  // namespace alpaca
