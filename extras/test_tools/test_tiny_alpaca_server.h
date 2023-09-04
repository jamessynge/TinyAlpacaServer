#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_TEST_TINY_ALPACA_SERVER_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_TEST_TINY_ALPACA_SERVER_H_

// Provides a base class for testing the decoding and dispatching of Alpaca
// requests for one or more device instances. This is basically parts of
// TinyAlpacaServer, but skipping dealing with PlatformEthernet, for which I
// don't yet have a fake implemented.
//
// Author: james.synge@gmail.com

#include <McuCore.h>
#include <stddef.h>
#include <stdint.h>

#include <string>
#include <string_view>

#include "device_interface.h"
#include "mcunet/extras/test_tools/string_io_stream_impl.h"
#include "server_connection.h"
#include "server_context.h"
#include "server_description.h"
#include "tiny_alpaca_network_server.h"

namespace alpaca {
namespace test {

struct ConnectionResult {
  std::string ToDebugString() const;

  // Input remaining after processing. For AnnounceHalfClosed, this is always
  // empty.
  std::string remaining_input;

  // Output produced during processing.
  std::string output;

  // True if the connection was explicitly closed while handling the input,
  // else false.
  bool connection_closed;
};

class TestTinyAlpacaServer : public TinyAlpacaDeviceServer {
 public:
  TestTinyAlpacaServer(ServerContext& server_context,
                       const ServerDescription& server_description,
                       mcucore::ArrayView<DeviceInterface*> devices);

  template <size_t N>
  TestTinyAlpacaServer(ServerContext& server_context,
                       const ServerDescription& server_description,
                       DeviceInterface* (&devices)[N])
      : TestTinyAlpacaServer(server_context, server_description,
                             mcucore::ArrayView<DeviceInterface*>(devices, N)) {
  }

  // These dispatch to the appropriate methods of the ServerConnection.

  // Announces that a connection has been established, with input being the
  // characters available for reading (assumed to be bytes for our purposes).
  // If repeat_until_stable is true and there is input remaining to read,
  // calls ServerConnection::OnCanRead until as long as the ServerConnection
  // keeps consuming input. If peer_half_closed, then
  // Connection::peer_half_closed() will be true once the input has all been
  // read. Returns the bytes of input that weren't read, the output produced
  // while handling the connect event, and whether the connection was closed.
  ConnectionResult AnnounceConnect(std::string_view input,
                                   bool repeat_until_stable = true,
                                   bool peer_half_closed = false);

  // Announces that there may be more to read from the connection, and that the
  // callee can also write to the connection. The parameters and return value
  // are the same as for AnnounceConnect.
  ConnectionResult AnnounceCanRead(std::string_view input,
                                   bool repeat_until_stable = true,
                                   bool peer_half_closed = false);

  // Announces that the callee can write to the connection, which is half-closed
  // (i.e. simulating that the client has half-closed, but is waiting for a
  // response). If repeat_until_stable is true, then calls
  // ServerConnection::OnHalfClosed until the ServerConnection stops writing
  // more output. Returns the return type is the same as for the above.
  ConnectionResult AnnounceHalfClosed(bool repeat_until_stable = true);

  // Announces that a previously open connection has been completely closed.
  void AnnounceDisconnect();

  // Based on the above methods, should the connection be considered open by the
  // client?
  bool connection_is_open() const { return connection_is_open_; }

  // Based on the above methods, should the connection be considered writeable
  // by the client?
  bool connection_is_writeable() const { return connection_is_writeable_; }

 private:
  void RepeatedlyAnnounceCanRead(mcunet::test::StringIoConnection& conn);
  void RepeatedlyAnnounceHalfClosed(mcunet::test::StringIoConnection& conn);
  void MaybeHalfClose(mcunet::test::StringIoConnection& conn,
                      bool peer_half_closed);

  ServerConnection server_connection_;
  uint8_t sock_num_;

  // These are based on the Announce calls that have been made, and on whether
  // the ServerConnection closed the passed in connection.
  bool connection_is_open_{false};
  bool connection_is_writeable_{false};  // Client can write.
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_TEST_TINY_ALPACA_SERVER_H_
