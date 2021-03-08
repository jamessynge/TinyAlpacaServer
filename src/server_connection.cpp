// TODO(jamessynge): Describe why this file exists/what it provides.

#include "server_connection.h"

#include "utils/platform.h"

namespace alpaca {

ServerConnection::ServerConnection(int sock_num, uint16_t tcp_port)
    : sock_num_(sock_num), tcp_port_(tcp_port) {}

void ServerConnection::OnConnect(EthernetClient& client) {}

void ServerConnection::OnCanRead(EthernetClient& client) {}

void ServerConnection::OnClientDone(EthernetClient& client) {}

}  // namespace alpaca
