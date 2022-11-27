#include "server_context.h"

#include <McuCore.h>

namespace alpaca {

mcucore::Status ServerContext::Initialize() { return Initialize(EEPROM); }

mcucore::Status ServerContext::Initialize(EEPROMClass& eeprom) {
  status_or_eeprom_tlv_ = mcucore::EepromTlv::Get(eeprom);
  MCU_VLOG_IF_ERROR(1, status_or_eeprom_tlv_.status());
  return status_or_eeprom_tlv_.status();
}

mcucore::EepromTlv& ServerContext::eeprom_tlv() {
  // This will MCU_CHECK fail if not successfully initialized.
  MCU_VLOG_IF_ERROR(1, status_or_eeprom_tlv_.status());
  return status_or_eeprom_tlv_.value();
}

}  // namespace alpaca
