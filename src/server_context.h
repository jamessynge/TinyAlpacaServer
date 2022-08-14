#ifndef TINY_ALPACA_SERVER_SRC_SERVER_CONTEXT_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_CONTEXT_H_

// Data that is shared between different connections.

#include <McuCore.h>

namespace alpaca {

class ServerContext {
 public:
  // ServerContext();

  // Initialize using the default EEPROM instance. Returns an error if unable
  // to initialize all features (so far just EepromTlv).
  mcucore::Status Initialize();

  // As above, but using the specified EEPROM instance. This variant exists to
  // make it easier to test.
  mcucore::Status Initialize(EEPROMClass& eeprom);

  // Returns a reference to the EepromTlv instance that can be used for reading
  // or writing TLV entries. MUST not be called before Initialize has returned
  // OK, else it will crash.
  mcucore::EepromTlv& eeprom_tlv();

 protected:
 private:
  mcucore::StatusOr<mcucore::EepromTlv> status_or_eeprom_tlv_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_CONTEXT_H_
