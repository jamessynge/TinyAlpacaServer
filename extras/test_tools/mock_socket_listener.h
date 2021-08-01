#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_SOCKET_LISTENER_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_SOCKET_LISTENER_H_

#include "gmock/gmock.h"
#include "utils/connection.h"
#include "utils/socket_listener.h"

namespace alpaca {
namespace test {

class MockSocketListener : public SocketListener {
 public:
  MOCK_METHOD(void, OnCanRead, (class alpaca::Connection &), (override));

  MOCK_METHOD(void, OnHalfClosed, (class alpaca::Connection &), (override));

  MOCK_METHOD(void, OnDisconnect, (), (override));
};

class MockServerSocketListener : public ServerSocketListener {
 public:
  MOCK_METHOD(void, OnCanRead, (class alpaca::Connection &), (override));

  MOCK_METHOD(void, OnHalfClosed, (class alpaca::Connection &), (override));

  MOCK_METHOD(void, OnDisconnect, (), (override));

  MOCK_METHOD(void, OnConnect, (class alpaca::Connection &), (override));
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_SOCKET_LISTENER_H_
