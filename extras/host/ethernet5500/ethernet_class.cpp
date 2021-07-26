#include "extras/host/ethernet5500/ethernet_class.h"

#include "extras/host/ethernet5500/ethernet_config.h"

EthernetClass Ethernet;  // NOLINT

// static
uint8_t EthernetClass::_state[MAX_SOCK_NUM];
// static
uint16_t EthernetClass::_server_port[MAX_SOCK_NUM];

EthernetClass::EthernetClass()
    : _maxSockNum(MAX_SOCK_NUM), _pinCS(255), _pinRST(255), _dhcp(nullptr) {
  _customHostname[0] = 0;
}
