#ifndef TINY_ALPACA_SERVER_SRC_UTILS_ADDRESSES_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_ADDRESSES_H_

// Support for generation and EEPROM persistence of MAC and IP addresses.
//
// Author: james.synge@gmail.com

#include "eeprom_io.h"
#include "mcucore_platform.h"
#include "utils/platform_ethernet.h"

namespace alpaca {

void printMACAddress(uint8_t mac[6]);

// Organizationally Unique Identifier: first 3 bytes of a MAC address that is
// NOT a globally unique address. The
struct OuiPrefix : Printable {
  // Ctor will ensure that the bit marking this as an OUI is set, and that
  // the multicast address bit is cleared.
  OuiPrefix();
  OuiPrefix(uint8_t a, uint8_t b, uint8_t c);
  size_t printTo(Print&) const override;

  uint8_t bytes[3];
};

// Represents an Ethernet address.
struct MacAddress : Printable {
  // Fills mac with a randomly generated, non-broadcast MAC address in the
  // space of Organizationally Unique Identifiers. If an OuiPrefix is supplied,
  // it will be used as the first 3 bytes of the MAC address.
  // The Arduino random number library is used, so be sure to seed it according
  // to the level or randomness you want in the generated address; if you don't
  // set the seed, the same sequence of numbers is always produced.
  void generateAddress(const OuiPrefix* oui_prefix = nullptr);
  size_t printTo(Print&) const override;

  // Saves to the specified address in the EEPROM; returns the address after
  // the saved MAC address.
  int save(int toAddress, mcucore::Crc32* crc) const;

  // Reads from the specified address in the EEPROM; returns the address after
  // the restored MAC address.
  int read(int fromAddress, mcucore::Crc32* crc);

  // Returns true if the first 3 bytes match the specified prefix.
  bool hasOuiPrefix(const OuiPrefix& oui_prefix) const;

  bool operator==(const MacAddress& other) const;

  uint8_t mac[6];
};

// Extends the IPAddress class provided by the Arduino core library to support
// saving an IPv4 address to EEPROM and later reading it back from EEPROM.
class SaveableIPAddress : public IPAddress {
 public:
  // Inherit the base class constructors.
  using IPAddress::IPAddress;

  // Saves to the specified address in the EEPROM; returns the address after
  // the saved value.
  int save(int toAddress, mcucore::Crc32* crc) const;

  // Reads from the specified address in the EEPROM; returns the address after
  // the restored value.
  int read(int fromAddress, mcucore::Crc32* crc);
};

// A pair of addresses (MAC and IP); the two are needed togther when working
// with the Arduino Ethernet library. Supports saving to EEPROM and later
// reading it back from EEPROM. This is useful because it allows us to generate
// random addresses when we first boot up a sketch, and then use those same
// addresses each time the sketch boots up in the future; this may make it
// easier for the person using the sketch to find their device on the LAN.
struct Addresses : Printable {
  // Load the saved addresses, which must have the oui_prefix if specified;
  // if unable to load them (not stored or wrong prefix), generate addresses
  // and store them in the EEPROM.
  void loadOrGenAndSave(const OuiPrefix* oui_prefix);

  // Save this struct's fields to EEPROM at address 0.
  void save() const;

  // Restore this struct's fields from EEPROM, starting at address 0.
  // Returns true if successful (i.e. the named and CRC matched),
  // false otherwse.
  bool load(const OuiPrefix* oui_prefix);

  // Randomly generate MAC and IPAddress. The MAC address has the specified
  // OuiPrefix if supplied (else it is random), and the IPAddress is in the link
  // local address range (169.254.1.0 to 169.254.254.255, according to RFC
  // 3927). No support is provided for detecting conflicts with other users of
  // the generated addresses. The Arduino random number library is used, so be
  // sure to seed it according to the level or randomness you want in the
  // generated address; if you don't set the seed, the same sequence of numbers
  // is always produced.
  void generateAddresses(const OuiPrefix* oui_prefix);

  // Print the addresses, preceded by a prefix (if provided) and followed by a
  // newline.
  void println(const char* prefix = nullptr) const;
  size_t printTo(Print&) const override;

  bool operator==(const Addresses& other) const;

  SaveableIPAddress ip;
  MacAddress mac;
};
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_ADDRESSES_H_
