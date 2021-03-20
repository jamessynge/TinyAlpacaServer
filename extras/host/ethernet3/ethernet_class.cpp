#include "extras/host/ethernet3/ethernet_class.h"

EthernetClass Ethernet;

// static
uint8_t EthernetClass::_state[MAX_SOCK_NUM];
// static
uint16_t EthernetClass::_server_port[MAX_SOCK_NUM];
