#include "extras/test_tools/test_tiny_alpaca_server.h"

#include <McuCore.h>

#include <sstream>
#include <string>
#include <string_view>

#include "absl/strings/escaping.h"
#include "device_interface.h"
#include "mcunet/extras/host/ethernet5500/ethernet_config.h"
#include "mcunet/extras/test_tools/string_io_stream_impl.h"
#include "server_connection.h"
#include "server_description.h"

namespace alpaca {
namespace test {

using ::mcunet::test::StringIoConnection;

TestTinyAlpacaServer::TestTinyAlpacaServer(
    ServerContext& server_context, const ServerDescription& server_description,
    mcucore::ArrayView<DeviceInterface*> devices)
    : TinyAlpacaDeviceServer(server_context, server_description, devices),
      server_connection_(*this),
      sock_num_(0) {}

ConnectionResult TestTinyAlpacaServer::AnnounceConnect(std::string_view input,
                                                       bool repeat_until_stable,
                                                       bool peer_half_closed) {
  MCU_VLOG(9) << "TestTinyAlpacaServer::AnnounceConnect";
  MCU_CHECK(!server_connection_.has_socket());
  MCU_CHECK(!connection_is_open_);
  MCU_CHECK(!connection_is_writeable_);
  StringIoConnection conn(sock_num_, input);
  server_connection_.OnConnect(conn);
  if (repeat_until_stable) {
    RepeatedlyAnnounceCanRead(conn);
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
  MCU_VLOG(9) << "TestTinyAlpacaServer::AnnounceCanRead";
  MCU_CHECK(server_connection_.has_socket());
  MCU_CHECK_EQ(sock_num_, server_connection_.sock_num());
  MCU_CHECK(connection_is_open_);
  MCU_CHECK(connection_is_writeable_);
  MCU_CHECK(!(input.empty() && peer_half_closed))
      << MCU_FLASHSTR("Call AnnounceHalfClosed instead");
  StringIoConnection conn(sock_num_, input);
  if (repeat_until_stable) {
    RepeatedlyAnnounceCanRead(conn);
  } else {
    server_connection_.OnCanRead(conn);
  }
  connection_is_open_ = conn.connected();
  connection_is_writeable_ = connection_is_open_ && !peer_half_closed;
  return ConnectionResult{
      .remaining_input = std::string(conn.remaining_input()),
      .output = conn.output(),
      .connection_closed = !conn.connected()};
}

void TestTinyAlpacaServer::RepeatedlyAnnounceCanRead(StringIoConnection& conn) {
  MCU_VLOG(9) << "TestTinyAlpacaServer::RepeatedlyAnnounceCanRead";
  while (conn.connected()) {
    MaintainDevices();
    const auto remaining_input_size = conn.remaining_input().size();
    const auto output_size = conn.output().size();
    server_connection_.OnCanRead(conn);
    if (conn.remaining_input().size() == remaining_input_size &&
        conn.output().size() == output_size) {
      break;
    }
  }
}

void TestTinyAlpacaServer::AnnounceDisconnect() {
  MCU_VLOG(9) << "TestTinyAlpacaServer::AnnounceDisconnect";
  MCU_CHECK(server_connection_.has_socket());
  MCU_CHECK_EQ(sock_num_, server_connection_.sock_num());
  MCU_CHECK(connection_is_open_);
  server_connection_.OnDisconnect();
  sock_num_ = (sock_num_ + 1) % MAX_SOCK_NUM;
  connection_is_open_ = connection_is_writeable_ = false;
}

std::string ConnectionResult::ToDebugString() const {
  std::ostringstream oss;
  oss << "ConnectionResult{closed: " << (connection_closed ? "true" : "false");
  oss << ", remaining_input: \"" << absl::CHexEscape(remaining_input) << "\"";
  oss << ", output: \"" << absl::CHexEscape(output) << "\"";
  oss << "}";
  return oss.str();
}

}  // namespace test
}  // namespace alpaca
