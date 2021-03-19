#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET3_W5500_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET3_W5500_H_

// Some portions of Ethernet3/src/utility/w5500.h for the purpose of faking
// the interface exposed by the Wiznet 5500 network chip.

#include "extras/host/arduino/int_types.h"

class SnMR {
 public:
  static const uint8_t CLOSE = 0x00;   // NOLINT
  static const uint8_t TCP = 0x01;     // NOLINT
  static const uint8_t UDP = 0x02;     // NOLINT
  static const uint8_t IPRAW = 0x03;   // NOLINT
  static const uint8_t MACRAW = 0x04;  // NOLINT
  static const uint8_t PPPOE = 0x05;   // NOLINT
  static const uint8_t ND = 0x20;      // NOLINT
  static const uint8_t MULTI = 0x80;   // NOLINT
};

enum SockCMD {
  Sock_OPEN = 0x01,
  Sock_LISTEN = 0x02,
  Sock_CONNECT = 0x04,
  Sock_DISCON = 0x08,
  Sock_CLOSE = 0x10,
  Sock_SEND = 0x20,
  Sock_SEND_MAC = 0x21,
  Sock_SEND_KEEP = 0x22,
  Sock_RECV = 0x40
};

class SnIR {
 public:
  static const uint8_t SEND_OK = 0x10;  // NOLINT
  static const uint8_t TIMEOUT = 0x08;  // NOLINT
  static const uint8_t RECV = 0x04;     // NOLINT
  static const uint8_t DISCON = 0x02;   // NOLINT
  static const uint8_t CON = 0x01;      // NOLINT
};

class SnSR {
 public:
  static const uint8_t CLOSED = 0x00;       // NOLINT
  static const uint8_t INIT = 0x13;         // NOLINT
  static const uint8_t LISTEN = 0x14;       // NOLINT
  static const uint8_t SYNSENT = 0x15;      // NOLINT
  static const uint8_t SYNRECV = 0x16;      // NOLINT
  static const uint8_t ESTABLISHED = 0x17;  // NOLINT
  static const uint8_t FIN_WAIT = 0x18;     // NOLINT
  static const uint8_t CLOSING = 0x1A;      // NOLINT
  static const uint8_t TIME_WAIT = 0x1B;    // NOLINT
  static const uint8_t CLOSE_WAIT = 0x1C;   // NOLINT
  static const uint8_t LAST_ACK = 0x1D;     // NOLINT
  static const uint8_t UDP = 0x22;          // NOLINT
  static const uint8_t IPRAW = 0x32;        // NOLINT
  static const uint8_t MACRAW = 0x42;       // NOLINT
  static const uint8_t PPPOE = 0x5F;        // NOLINT
};

class IPPROTO {
 public:
  static const uint8_t IP = 0;     // NOLINT
  static const uint8_t ICMP = 1;   // NOLINT
  static const uint8_t IGMP = 2;   // NOLINT
  static const uint8_t GGP = 3;    // NOLINT
  static const uint8_t TCP = 6;    // NOLINT
  static const uint8_t PUP = 12;   // NOLINT
  static const uint8_t UDP = 17;   // NOLINT
  static const uint8_t IDP = 22;   // NOLINT
  static const uint8_t ND = 77;    // NOLINT
  static const uint8_t RAW = 255;  // NOLINT
};
#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ETHERNET3_W5500_H_
