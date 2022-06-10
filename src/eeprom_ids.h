#ifndef SRC_EEPROM_IDS_H
#define SRC_EEPROM_IDS_H

// There isn't (yet?) a good way to track which EepromTag.id values are used by
// different pieces of code. For TinyAlpacaServer, I'm recording the ids used
// here. Note that there can be multiple uses for the same id value, as long as
// the domain is different.

#include <Arduino.h>

namespace alpaca {
// Please sort the values here by ascending id.

// For storing the name given to a particular switch. Applies to all Switch
// device domains.
static constexpr uint8_t kMaxSwitchesForName = 10;
static constexpr uint8_t kSwitch0NameId = 50;
static constexpr uint8_t kSwitch1NameId = 51;
static constexpr uint8_t kSwitch2NameId = 52;
static constexpr uint8_t kSwitch3NameId = 53;
static constexpr uint8_t kSwitch4NameId = 54;
static constexpr uint8_t kSwitch5NameId = 55;
static constexpr uint8_t kSwitch6NameId = 56;
static constexpr uint8_t kSwitch7NameId = 57;
static constexpr uint8_t kSwitch8NameId = 58;
static constexpr uint8_t kSwitch9NameId = 59;

// For storing the UniqueId of a device; applies to all device domains.
static constexpr uint8_t kUniqueIdTagId = 128;

}  // namespace alpaca

#endif  // SRC_EEPROM_IDS_H
