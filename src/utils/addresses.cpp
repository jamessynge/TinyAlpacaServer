#include "utils/addresses.h"

#include "utils/hex_escape.h"
#include "utils/inline_literal.h"
#include "utils/logging.h"
#include "utils/o_print_stream.h"

namespace alpaca {
namespace {
// This is the name used to identify the data stored in the EEPROM. Changing the
// value (e.g. between "addrs" and "Addrs") has the effect of invalidating the
// currently stored values, which can be useful if you want to change the
// OuiPrefix, or to debug this code.
// TODO(jamessynge): Stop wasting RAM on this string.
const char kName[] = "addrs";

// A link-local address is in the range 169.254.1.0 to 169.254.254.255,
// inclusive. Learn more: https://tools.ietf.org/html/rfc3927
void pickIPAddress(IPAddress* output) {
  int c = random(254) + 1;
  TAS_VLOG(5) << TASLIT("pickIPAddress: c=") << c;

  int d = random(256);
  TAS_VLOG(5) << TASLIT("pickIPAddress: d=") << d;

  (*output)[0] = 169;
  (*output)[1] = 254;
  (*output)[2] = c;
  (*output)[3] = d;
}

// Will modify the first byte of a MAC address so that it is in the
// Organizationally Unique Identifier space, and that it is a unicast
// (rather than multicast) address.
uint8_t toOuiUnicast(uint8_t macByte1) {
  // Make sure this is in the locally administered space.
  macByte1 |= 2;
  // And not a multicast address.
  macByte1 &= ~1;
  return macByte1;
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// An Arduino Ethernet shield (or an freetronics EtherTen board, which I've used
// to test this code) does not have its own MAC address (the unique identifier
// used to distinguish one Ethernet device from another). Fortunately the design
// of MAC addresses allows for both globally unique addresses (i.e. assigned at
// the factory, unique world-wide) and locally unique addresses. This code will
// generate an address in the range allowed for local administered addresses and
// store it in EEPROM. Note though that there is no support here for probing to
// ensure that the allocated address is free. Read more about the issue here:
//
//     https://serverfault.com/a/40720
//     https://en.wikipedia.org/wiki/MAC_address#Universal_vs._local
//
// Quoting from the wikipedia article:
//
//     Universally administered and locally administered addresses are
//     distinguished by setting the second least significant bit of the
//     most significant byte of the address. If the bit is 0, the address
//     is universally administered. If it is 1, the address is locally
//     administered. In the example address 02-00-00-00-00-01 the most
//     significant byte is 02h. The binary is 00000010 and the second
//     least significant bit is 1. Therefore, it is a locally
//     administered address.

OuiPrefix::OuiPrefix() : OuiPrefix(0, 0, 0) {}

OuiPrefix::OuiPrefix(uint8_t a, uint8_t b, uint8_t c) {
  bytes[0] = toOuiUnicast(a);
  bytes[1] = b;
  bytes[2] = c;
}

size_t OuiPrefix::printTo(Print& p) const {
  size_t result = p.print(bytes[0], HEX);
  for (int i = 1; i < 3; ++i) {
    result += p.print('-');
    result += p.print(bytes[i], HEX);
  }
  return result;
}

////////////////////////////////////////////////////////////////////////////////

void MacAddress::generateAddress(const OuiPrefix* oui_prefix) {
  int first_index;
  if (oui_prefix) {
    first_index = 3;
    mac[0] = oui_prefix->bytes[0];
    mac[1] = oui_prefix->bytes[1];
    mac[2] = oui_prefix->bytes[2];
  } else {
    first_index = 0;
  }
  for (int i = first_index; i < 6; ++i) {
    auto r = static_cast<uint8_t>(random(256));
    if (i == 0) {
      r = toOuiUnicast(r);
    }
    mac[i] = r;
    TAS_VLOG(4) << TASLIT("mac[") << i << TASLIT("] = 0x") << BaseHex
                << (mac[i] + 0);
  }
}

size_t MacAddress::printTo(Print& p) const {
  size_t result = p.print(mac[0], HEX);
  for (int i = 1; i < 6; ++i) {
    result += p.print('-');
    result += p.print(mac[i], HEX);
  }
  return result;
}

int MacAddress::save(int toAddress, Crc32* crc) const {
  putBytes(toAddress, &mac[0], 6, crc);
  return toAddress + 6;
}

int MacAddress::read(int fromAddress, Crc32* crc) {
  getBytes(fromAddress, 6, &mac[0], crc);
  return fromAddress + 6;
}

bool MacAddress::hasOuiPrefix(const OuiPrefix& oui_prefix) const {
  return (mac[0] == oui_prefix.bytes[0] && mac[1] == oui_prefix.bytes[1] &&
          mac[2] == oui_prefix.bytes[2]);
}

bool MacAddress::operator==(const MacAddress& other) const {
  for (int i = 0; i < 6; ++i) {
    if (mac[i] != other.mac[i]) {
      return false;
    }
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// NOTE: I'm assuming here that IPAddress only supports IPv4 addresses. There
// are some Arduino libraries where IPAddress also supports IPv6 addresses,
// for which this code would need updating.

int SaveableIPAddress::save(int toAddress, Crc32* crc) const {
  for (int i = 0; i < 4; ++i) {
    const uint8_t b = (*this)[i];
    EEPROM.update(toAddress++, b);
    if (crc) {
      crc->appendByte(b);
    }
  }
  return toAddress;
}

int SaveableIPAddress::read(int fromAddress, Crc32* crc) {
  for (int i = 0; i < 4; ++i) {
    const uint8_t b = EEPROM.read(fromAddress++);
    (*this)[i] = b;
    if (crc) {
      crc->appendByte(b);
    }
  }
  return fromAddress;
}

////////////////////////////////////////////////////////////////////////////////

void Addresses::loadOrGenAndSave(const OuiPrefix* oui_prefix) {
  TAS_VLOG(4) << TASLIT("Entered loadOrGenAndSave");
  if (load(oui_prefix)) {
    return;
  }
  // Need to generate a new address.
  generateAddresses(oui_prefix);
  save();

#ifndef NDEBUG
  Addresses loader;
  TAS_DCHECK(loader.load(oui_prefix));
  TAS_DCHECK_EQ(loader.ip, ip);
#endif
}

void Addresses::save() const {
  TAS_VLOG(3) << TASLIT("Saving ") << kName;

  int ipAddress = saveName(0, kName);
  Crc32 crc;
  int macAddress = ip.save(ipAddress, &crc);
  int crcAddress = mac.save(macAddress, &crc);
  crc.put(crcAddress);
  // int dataStartAddress = crcAddress + 4;
  // int dataAddress = ip.save(dataStartAddress, &crc);
  // dataAddress = mac.save(dataAddress, &crc);
  // putCrc(crcAddress, crc);
}

bool Addresses::load(const OuiPrefix* oui_prefix) {
  int ipAddress;
  if (!verifyName(0, kName, &ipAddress)) {
    TAS_VLOG(2) << TASLIT("Stored name mismatch");
    return false;
  }
  Crc32 crc;
  int macAddress = ip.read(ipAddress, &crc);
  int crcAddress = mac.read(macAddress, &crc);
  if (!crc.verify(crcAddress)) {
    TAS_VLOG(2) << TASLIT("Stored crc mismatch");
    return false;
  }
  if (oui_prefix && !mac.hasOuiPrefix(*oui_prefix)) {
    TAS_VLOG(2) << TASLIT("Stored OUI prefix mismatch");
    return false;
  }
  return true;
}

void Addresses::generateAddresses(const OuiPrefix* oui_prefix) {
  mac.generateAddress(oui_prefix);
  pickIPAddress(&ip);
}

void Addresses::println(const char* prefix) const {
  if (prefix) {
    Serial.print(prefix);
  }
  Serial.println(*this);
}

size_t Addresses::printTo(Print& p) const {
  size_t result = p.print(kName);
  result += p.print(TASLIT(": MAC="));
  result += p.print(mac);
  result += p.print(TASLIT(", IP="));
  result += p.print(ip);
  return result;
}

bool Addresses::operator==(const Addresses& other) const {
  return ip == other.ip && mac == other.mac;
}
}  // namespace alpaca
