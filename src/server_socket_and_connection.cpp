#include "server_socket_and_connection.h"

namespace alpaca {

ServerSocketAndConnection::ServerSocketAndConnection(
    uint16_t tcp_port, RequestListener& request_listener)
    : server_connection_(request_listener),
      server_socket_(tcp_port, server_connection_) {}

bool ServerSocketAndConnection::Initialize() {
  return server_socket_.Initialize();
}

void ServerSocketAndConnection::PerformIO() { server_socket_.PerformIO(); }

}  // namespace alpaca
