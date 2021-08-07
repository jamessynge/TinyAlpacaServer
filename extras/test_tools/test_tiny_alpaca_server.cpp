#include "extras/test_tools/test_tiny_alpaca_server.h"

#include <sstream>
#include <string>
#include <string_view>

#include "absl/strings/escaping.h"
#include "absl/strings/str_cat.h"
#include "device_interface.h"
#include "extras/test_tools/string_io_connection.h"
#include "inline_literal.h"
#include "logging.h"
#include "mcucore/extrashost/ethernet5500/ethernet_config.h"
#include "o_print_stream.h"
#include "server_connection.h"
#include "server_description.h"
#include "tiny_alpaca_server.h"
#include "utils/array_view.h"

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
  TAS_CHECK(!connection_is_open_);
  TAS_CHECK(!connection_is_writeable_);
  StringIoConnection conn(sock_num_, input, peer_half_closed);
  server_connection_.OnConnect(conn);
  if (repeat_until_stable) {
    RepeatedlyAnnounceCanRead(conn);
    MaybeHalfClose(conn, peer_half_closed);
  }
  connection_is_open_ = conn.connected();
  connection_is_writeable_ = connection_is_open_ && !peer_half_closed;
  return ConnectionResult{
      .remaining_input = std::string(conn.remaining_input()),
      .output = conn.output(),
      .connection_closed = !conn.connected()};
}

ConnectionResult TestTinyAlpacaServer::AnnounceCanRead(std::string_view input,
                                                       bool repeat_until_stable,
                                                       bool peer_half_closed) {
  TAS_CHECK(server_connection_.has_socket());
  TAS_CHECK_EQ(sock_num_, server_connection_.sock_num());
  TAS_CHECK(connection_is_open_);
  TAS_CHECK(connection_is_writeable_);
  TAS_CHECK(!(input.empty() && peer_half_closed))
      << TAS_FLASHSTR("Call AnnounceHalfClosed instead");
  StringIoConnection conn(sock_num_, input, peer_half_closed);
  server_connection_.OnCanRead(conn);
  if (repeat_until_stable) {
    RepeatedlyAnnounceCanRead(conn);
    MaybeHalfClose(conn, peer_half_closed);
  }
  connection_is_open_ = conn.connected();
  connection_is_writeable_ = connection_is_open_ && !peer_half_closed;
  return ConnectionResult{
      .remaining_input = std::string(conn.remaining_input()),
      .output = conn.output(),
      .connection_closed = !conn.connected()};
}

void TestTinyAlpacaServer::RepeatedlyAnnounceCanRead(StringIoConnection& conn) {
  while (conn.connected() && !conn.remaining_input().empty()) {
    const auto start_size = conn.remaining_input().size();
    server_connection_.OnCanRead(conn);
    if (conn.remaining_input().size() == start_size) {
      break;
    }
  }
}

void TestTinyAlpacaServer::MaybeHalfClose(StringIoConnection& conn,
                                          bool peer_half_closed) {
  if (peer_half_closed && conn.connected()) {
    RepeatedlyAnnounceHalfClosed(conn);
  }
}

ConnectionResult TestTinyAlpacaServer::AnnounceHalfClosed(
    bool repeat_until_stable) {
  TAS_CHECK(server_connection_.has_socket());
  TAS_CHECK_EQ(sock_num_, server_connection_.sock_num());
  TAS_CHECK(connection_is_open_);
  StringIoConnection conn(sock_num_, "", /*half_closed=*/true);
  server_connection_.OnHalfClosed(conn);
  if (repeat_until_stable && !conn.output().empty()) {
    RepeatedlyAnnounceHalfClosed(conn);
  }
  connection_is_open_ = conn.connected();
  connection_is_writeable_ = false;
  return ConnectionResult{.output = conn.output(),
                          .connection_closed = !conn.connected()};
}

void TestTinyAlpacaServer::RepeatedlyAnnounceHalfClosed(
    StringIoConnection& conn) {
  while (conn.connected()) {
    const auto start_size = conn.output().size();
    server_connection_.OnHalfClosed(conn);
    if (conn.output().size() == start_size) {
      break;
    }
  }
}

void TestTinyAlpacaServer::AnnounceDisconnect() {
  TAS_CHECK(server_connection_.has_socket());
  TAS_CHECK_EQ(sock_num_, server_connection_.sock_num());
  TAS_CHECK(connection_is_open_);
  server_connection_.OnDisconnect();
  sock_num_ = (sock_num_ + 1) % MAX_SOCK_NUM;
  connection_is_open_ = connection_is_writeable_ = false;
}

std::string ConnectionResult::ToDebugString() const {
  std::ostringstream oss;
  oss << "ConnectionResult{closed: " << (connection_closed ? "true" : "false");
  if (!remaining_input.empty()) {
    oss << ", remaining_input: \"" << absl::CHexEscape(remaining_input) << "\"";
  }
  if (!output.empty()) {
    oss << ", output: \"" << absl::CHexEscape(output) << "\"";
  }
  oss << "}";
  return oss.str();
}

}  // namespace test
}  // namespace alpaca
