#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_IP_ADDRESS_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_IP_ADDRESS_H_

// Arduino core provides IPAddress, this has (will have) the portion I need for
// Tiny Alpaca Server on the host.

#include "extras/host_arduino/int_types.h"
#include "extras/host_arduino/print.h"

class IPAddress : public Printable {
 public:
  IPAddress();
  IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet,
            uint8_t fourth_octet);
  explicit IPAddress(uint32_t address);
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_IP_ADDRESS_H_
