#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_ETHERNET_CLIENT_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_ETHERNET_CLIENT_H_

#include "extras/host/ethernet3/ethernet_client.h"
#include "googletest/gmock.h"

class MockEthernetClient : public EthernetClient {
 public:
  MOCK_METHOD(size_t, write, (uint8_t), (override));

  MOCK_METHOD(size_t, write, (const uint8_t *, size_t), (override));

  MOCK_METHOD(void, flush, (), (override));

  MOCK_METHOD(int, available, (), (override));

  MOCK_METHOD(int, read, (), (override));

  MOCK_METHOD(int, peek, (), (override));

  MOCK_METHOD(int, connect, (class IPAddress, uint16_t), (override));

  MOCK_METHOD(int, connect, (const char *, uint16_t), (override));

  MOCK_METHOD(int, read, (uint8_t *, size_t), (override));

  MOCK_METHOD(void, flush, (), (override));

  MOCK_METHOD(void, stop, (), (override));

  MOCK_METHOD(uint8_t, connected, (), (override));

  MOCK_METHOD(_Bool, operator bool, (), (override));

  MOCK_METHOD(int, connect, (class IPAddress, uint16_t), (override));

  MOCK_METHOD(int, connect, (const char *, uint16_t), (override));

  MOCK_METHOD(size_t, write, (uint8_t), (override));

  MOCK_METHOD(size_t, write, (const uint8_t *, size_t), (override));

  MOCK_METHOD(int, available, (), (override));

  MOCK_METHOD(int, read, (), (override));

  MOCK_METHOD(int, read, (uint8_t *, size_t), (override));

  MOCK_METHOD(int, peek, (), (override));

  MOCK_METHOD(void, flush, (), (override));

  MOCK_METHOD(void, stop, (), (override));

  MOCK_METHOD(uint8_t, connected, (), (override));

  MOCK_METHOD(_Bool, operator bool, (), (override));

  MOCK_METHOD(uint8_t, status, (), (override));

  MOCK_METHOD(uint8_t, getSocketNumber, (), (const, override));
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_ETHERNET_CLIENT_H_
