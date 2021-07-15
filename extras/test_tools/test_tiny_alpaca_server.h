#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_TEST_TINY_ALPACA_SERVER_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_TEST_TINY_ALPACA_SERVER_H_

// Provides a base class for testing the decoding and dispatching of Alpaca
// requests for one or more device instances. This is basically parts of
// TinyAlpacaServer, but skipping dealing with PlatformEthernet, for which I
// don't yet have a fake implemented.

#include <iostream>
#include <string>

// TODO Remove excessive headers.

#include "absl/strings/str_cat.h"
#include "alpaca_devices.h"
#include "alpaca_discovery_server.h"
#include "alpaca_response.h"
#include "constants.h"
#include "device_types/device_impl_base.h"
#include "extras/test_tools/mock_device_interface.h"
#include "extras/test_tools/mock_switch_group.h"
#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "literals.h"
#include "request_listener.h"
#include "server_connection.h"
#include "server_description.h"
#include "server_sockets_and_connections.h"
#include "tiny_alpaca_server.h"
#include "utils/array_view.h"
#include "utils/platform.h"
#include "utils/platform_ethernet.h"
#include "utils/status.h"
#include "utils/string_view.h"

namespace alpaca {
namespace test {

struct ConnectionResult {
  // Input remaining after processing. For AnnounceHalfClosed, this is always
  // empty.
  std::string_view remaining_input;

  // Output produced during processing.
  std::string output;

  // True if the connection was explicitly closed while handling the input,
  // else false.
  bool connection_closed;
};

class TestTinyAlpacaServer : public TinyAlpacaServerBase {
 public:
  TestTinyAlpacaServer(const ServerDescription& server_description,
                       ArrayView<DeviceInterface*> devices);

  // These dispatch to the appropriate methods of the ServerConnection.

  // Announces that a connection has been established, with input being the
  // characters available for reading (assumed to be bytes for our purposes).
  // Returns the bytes of input that weren't read, the output produced while
  // handling the connect event, and whether the connection was closed.
  ConnectionResult AnnounceConnect(std::string_view input);

  // Announces that there may be more to read from the connection, and that the
  // callee can also write to the connection. The parameter and return value are
  // the same as for AnnounceConnect.
  ConnectionResult AnnounceCanRead(std::string_view input);

  // Announces that the callee can write to the connection, which is half-closed
  // (i.e. simulating that the client has half-closed, but is waiting for a
  // response). Returns the return type is the same as for the above.
  ConnectionResult AnnounceHalfClosed();

  // Announces that a previously open connection has been completely closed.
  void AnnounceOnDisconnect();

 private:
  ServerConnection server_connection_;
  uint8_t sock_num_;
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_TEST_TINY_ALPACA_SERVER_H_
