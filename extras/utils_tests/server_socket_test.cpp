#include "utils/server_socket.h"

#include <stdint.h>

#include "extras/test_tools/mock_socket_listener.h"
#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "utils/socket_listener.h"

namespace alpaca {
namespace test {
namespace {

using ::testing::AnyNumber;
using ::testing::EndsWith;
using ::testing::Eq;
using ::testing::HasSubstr;
using ::testing::InSequence;
using ::testing::IsEmpty;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Not;
using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::StartsWith;
using ::testing::StrictMock;

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
