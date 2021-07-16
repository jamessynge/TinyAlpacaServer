#include "extras/test_tools/test_tiny_alpaca_server.h"

#include "extras/test_tools/string_io_connection.h"
#include "utils/logging.h"

namespace alpaca {
namespace test {

TestTinyAlpacaServer::TestTinyAlpacaServer(
    const ServerDescription& server_description,
    ArrayView<DeviceInterface*> devices)
    : TinyAlpacaServerBase(server_description, devices),
      server_connection_(*this),
      sock_num_(0) {}

ConnectionResult TestTinyAlpacaServer::AnnounceConnect(std::string_view input,
                                                       bool repeat_until_stable,
                                                       bool peer_half_closed) {
  TAS_CHECK(!server_connection_.has_socket());
  StringIoConnection conn(sock_num_, input, peer_half_closed);
  server_connection_.OnConnect(conn);
  if (repeat_until_stable) {
    while (conn.connected() && !conn.remaining_input().empty()) {
      const auto start_size = conn.remaining_input().size();
      server_connection_.OnCanRead(conn);
      if (conn.remaining_input().size() == start_size) {
        break;
      }
    }
  }
  return ConnectionResult{.remaining_input = conn.remaining_input(),
                          .output = conn.output(),
                          .connection_closed = !conn.connected()};
}

ConnectionResult TestTinyAlpacaServer::AnnounceCanRead(std::string_view input,
                                                       bool repeat_until_stable,
                                                       bool peer_half_closed) {
  TAS_CHECK(server_connection_.has_socket());
  TAS_CHECK_EQ(sock_num_, server_connection_.sock_num());
  TAS_CHECK(!(input.empty() && peer_half_closed))
      << TAS_FLASHSTR("Call AnnounceHalfClosed instead");
  StringIoConnection conn(sock_num_, input, peer_half_closed);
  server_connection_.OnCanRead(conn);
  if (repeat_until_stable) {
    while (conn.connected() && !conn.remaining_input().empty()) {
      const auto start_size = conn.remaining_input().size();
      server_connection_.OnCanRead(conn);
      if (conn.remaining_input().size() == start_size) {
        break;
      }
    }
  }
  return ConnectionResult{.remaining_input = conn.remaining_input(),
                          .output = conn.output(),
                          .connection_closed = !conn.connected()};
}

ConnectionResult TestTinyAlpacaServer::AnnounceHalfClosed(
    bool repeat_until_stable) {
  TAS_CHECK(server_connection_.has_socket());
  TAS_CHECK_EQ(sock_num_, server_connection_.sock_num());
  StringIoConnection conn(sock_num_, "", /*half_closed=*/true);
  server_connection_.OnHalfClosed(conn);
  if (repeat_until_stable && !conn.output().empty()) {
    while (conn.connected()) {
      const auto start_size = conn.output().size();
      server_connection_.OnHalfClosed(conn);
      if (conn.output().size() == start_size) {
        break;
      }
    }
  }
  return ConnectionResult{.output = conn.output(),
                          .connection_closed = !conn.connected()};
}

void TestTinyAlpacaServer::AnnounceOnDisconnect() {
  TAS_CHECK(server_connection_.has_socket());
  TAS_CHECK_EQ(sock_num_, server_connection_.sock_num());
  server_connection_.OnDisconnect();
  sock_num_ = (sock_num_ + 1) % MAX_SOCK_NUM;
}

}  // namespace test
}  // namespace alpaca
