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

ConnectionResult TestTinyAlpacaServer::AnnounceConnect(std::string_view input) {
  TAS_CHECK(!server_connection_.has_socket());
  StringIoConnection conn(sock_num_, input);
  server_connection_.OnConnect(conn);
  return ConnectionResult{.remaining_input = conn.remaining_input(),
                          .output = conn.output(),
                          .connection_closed = !conn.connected()};
}

ConnectionResult TestTinyAlpacaServer::AnnounceCanRead(std::string_view input) {
  TAS_CHECK(server_connection_.has_socket());
  TAS_CHECK_EQ(sock_num_, server_connection_.sock_num());
  StringIoConnection conn(sock_num_, input);
  server_connection_.OnCanRead(conn);
  return ConnectionResult{.remaining_input = conn.remaining_input(),
                          .output = conn.output(),
                          .connection_closed = !conn.connected()};
}

ConnectionResult TestTinyAlpacaServer::AnnounceHalfClosed() {
  TAS_CHECK(server_connection_.has_socket());
  TAS_CHECK_EQ(sock_num_, server_connection_.sock_num());
  StringIoConnection conn(sock_num_, "");
  server_connection_.OnHalfClosed(conn);
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
