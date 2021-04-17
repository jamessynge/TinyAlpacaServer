#ifndef TINY_ALPACA_SERVER_SRC_UTILS_EEPROM_IO_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_EEPROM_IO_H_

// Support for writing names and values to EEPROM, and later reading them back.
// Requires the Arduino EEPROM library.
//
// Author: james.synge@gmail.com

#include "utils/platform.h"

namespace alpaca {

// Class for computing a 32-bit Cyclic Redundancy Check (a hash).
// Used for verifying that the EEPROM is uncorrupted.
class Crc32 {
 public:
  // Add the next byte of the sequence on which we're computing a CRC.
  void appendByte(uint8_t v);

  // The current value of the CRC.
  uint32_t value() const { return value_; }

  // Store the CRC (value_) at the specified address. Returns the address after
  // the stored CRC.
  int put(int crcAddress) const;

  // Validate that the computed CRC (value_) matches the CRC stored at the
  // specified address.
  bool verify(int crcAddress) const;

 private:
  uint32_t value_ = ~0L;
};

int saveName(int toAddress, const char* name);
bool verifyName(int atAddress, const char* name, int* afterAddress);

// By passing all of the bytes written to a CRC instance as we save to the
// EEPROM, we can ensure that the CRC value is computed from the same bytes
// that we're later going to validate.
void putBytes(int address, const uint8_t* src, size_t numBytes, Crc32* crc);

// Similarly, we can validate during restore.
void getBytes(int address, size_t numBytes, uint8_t* dest, Crc32* crc);

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_EEPROM_IO_H_
