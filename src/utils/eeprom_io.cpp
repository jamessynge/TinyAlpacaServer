#include "utils/eeprom_io.h"

#include "experimental/users/jamessynge/arduino/mcucore/src/logging.h"
#include "experimental/users/jamessynge/arduino/mcucore/src/o_print_stream.h"
#include "utils/hex_escape.h"

namespace alpaca {
namespace {
// Values from https://www.arduino.cc/en/Tutorial/EEPROMCrc:
constexpr uint32_t kCrcTable[16] AVR_PROGMEM = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4,
    0x4db26158, 0x5005713c, 0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c};

uint32_t GetCrcTableEntry(uint32_t key) {
  auto offset = key & 0x0f;  // One of the 16 entries in the table.
#ifdef ARDUINO_ARCH_AVR
  // Unable to get the far pointer support to work for some reason (i.e. the
  // Arduino IDE compiler claims that 'pgm_read_dword_far' )
  auto ptr = &kCrcTable[offset];
  return pgm_read_dword_near(ptr);
#else   // !ARDUINO_ARCH_AVR
  return kCrcTable[offset];
#endif  // ARDUINO_ARCH_AVR
}
}  // namespace

void Crc32::appendByte(uint8_t v) {
  TAS_VLOG(6) << TAS_FLASHSTR("Crc32::appendByte(") << (v + 0)
              << TAS_FLASHSTR(") old value=") << BaseHex << value_;
  value_ = GetCrcTableEntry(value_ ^ v) ^ (value_ >> 4);
  value_ = GetCrcTableEntry(value_ ^ (v >> 4)) ^ (value_ >> 4);
  value_ = ~value_;
  TAS_VLOG(6) << TAS_FLASHSTR("new value=") << BaseHex << value_;
}

// Store the value at the specified address.
int Crc32::put(int crcAddress) const {
  static_assert(4 == sizeof value_, "sizeof value_ is not 4");
  TAS_VLOG(6) << TAS_FLASHSTR("Crc32::put(") << crcAddress
              << TAS_FLASHSTR(") value=") << BaseHex << value_;
  EEPROM.put(crcAddress, value_);
  TAS_CHECK(verify(crcAddress));
  return crcAddress + static_cast<int>(sizeof value_);
}

// Validate that the computed value (value_) matches the value stored
// at the specified address.
bool Crc32::verify(int crcAddress) const {
  uint32_t stored = 0;
  EEPROM.get(crcAddress, stored);
  if (value_ != stored) {
    TAS_VLOG(1) << TAS_FLASHSTR("Crc32::verify(") << crcAddress
                << TAS_FLASHSTR(") computed value=") << BaseHex << value_
                << TAS_FLASHSTR(", stored value=") << BaseHex << stored;
    return false;
  }
  return value_ == stored;
}

int saveName(int toAddress, const char* name) {
  while (*name != 0) {
    EEPROM.put(toAddress++, *name++);
  }
  return toAddress;
}

bool verifyName(int atAddress, const char* name, int* afterAddress) {
  // Confirm the name matches.
  while (*name != 0) {
    char c;
    EEPROM.get(atAddress++, c);
    if (c != *name++) {
      // Names don't match.
      return false;
    }
  }
  *afterAddress = atAddress;
  return true;
}

void putBytes(int address, const uint8_t* src, size_t numBytes, Crc32* crc) {
  while (numBytes-- > 0) {
    uint8_t b = *src++;
    if (crc) {
      crc->appendByte(b);
    }
    EEPROM.update(address++, b);
  }
}

void getBytes(int address, size_t numBytes, uint8_t* dest, Crc32* crc) {
  while (numBytes-- > 0) {
    uint8_t b = EEPROM.read(address++);
    if (crc) {
      crc->appendByte(b);
    }
    *dest++ = b;
  }
}

}  // namespace alpaca
