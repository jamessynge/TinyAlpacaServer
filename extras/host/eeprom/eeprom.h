#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_EEPROM_EEPROM_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_EEPROM_EEPROM_H_

// A totally fake version of Arduino's EEPROM. Just stores in RAM, no
// persistence. IFF we really wanted to, we could use mmap to read and write
// from a file, and thus provide persistence from run to run... which would
// complicate testing.

#include <stdint.h>

class EEPROMClass {
 public:
  // Basic user access methods.
  uint8_t read(int idx) { return data_[idx]; }
  uint8_t operator[](const int idx) { return read(idx); }
  void write(int idx, uint8_t val) { data_[idx] = val; }
  void update(int idx, uint8_t val) { write(idx, val); }

  uint16_t length() { return sizeof data_; }

  // Functionality to 'get' and 'put' objects to and from EEPROM.
  template <typename T>
  T &get(int idx, T &t) {
    uint8_t *ptr = reinterpret_cast<uint8_t *>(&t);
    int limit = idx + sizeof(T);
    while (idx < limit) {
      *ptr++ = read(idx++);
    }
    return t;
  }

  template <typename T>
  const T &put(int idx, const T &t) {
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&t);
    int limit = idx + sizeof(T);
    while (idx < limit) {
      update(idx, *ptr++);
    }
    return t;
  }

 private:
  uint8_t data_[1024];
};

extern EEPROMClass EEPROM;

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_EEPROM_EEPROM_H_
