#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_IP_ADDRESS_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_IP_ADDRESS_H_

// IPAddress holds and can print an IPv4 address. The Arduino version has more
// features that I've not yet needed for Tiny Alpaca Server/

#include "extras/host_arduino/int_types.h"
#include "extras/host_arduino/print.h"

class IPAddress : public Printable {
 public:
  IPAddress() : IPAddress(0, 0, 0, 0) {}
  IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet,
            uint8_t fourth_octet) {
    octets_[0] = first_octet;
    octets_[1] = second_octet;
    octets_[2] = third_octet;
    octets_[3] = fourth_octet;
  }
  // explicit IPAddress(uint32_t address);

  size_t printTo(Print& out) const override {
    uint16_t short_zero = 0;
    size_t count = 0;
    count += out.print(octets_[0] + short_zero);
    count += out.print('.');
    count += out.print(octets_[1] + short_zero);
    count += out.print('.');
    count += out.print(octets_[2] + short_zero);
    count += out.print('.');
    count += out.print(octets_[3] + short_zero);
    return count;
  }

 private:
  uint8_t octets_[4];
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_IP_ADDRESS_H_
