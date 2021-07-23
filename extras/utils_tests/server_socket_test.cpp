#include "utils/server_socket.h"

#include <stdint.h>

#include "extras/test_tools/mock_socket_listener.h"
#include "googletest/gtest.h"

namespace alpaca {
namespace test {
namespace {

const uint16_t kTcpPort = 9999;

class ServerSocketTest : public testing::Test {
 protected:
  ServerSocketTest() : server_socket_(kTcpPort, mock_listener_) {}
  void SetUp() override {}

  MockServerSocketListener mock_listener_;
  ServerSocket server_socket_;
};

TEST_F(ServerSocketTest, FixtureTest) {
  EXPECT_FALSE(server_socket_.HasSocket());
  EXPECT_FALSE(server_socket_.IsConnected());
}

}  // namespace
}  // namespace test
}  // namespace alpaca
