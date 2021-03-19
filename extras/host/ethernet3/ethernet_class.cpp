#include "extras/host/ethernet3/ethernet_class.h"

EthernetClass Ethernet;

// static
uint8_t EthernetClass::state_[MAX_SOCK_NUM];
// static
uint16_t EthernetClass::server_port_[MAX_SOCK_NUM];
