#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_LISTENER_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_LISTENER_H_

// Mock version of alpaca::RequestListener, where only the pure virtual methods
// are mocked.
//
// Author: james.synge@gmail.com

#include "constants.h"
#include "gmock/gmock.h"
#include "request_listener.h"

namespace alpaca {
namespace test {

class MockRequestListener : public RequestListener {
 public:
  MOCK_METHOD(void, OnStartDecoding, (struct AlpacaRequest &), (override));

  MOCK_METHOD(bool, OnRequestDecoded, (struct AlpacaRequest &, class Print &),
              (override));

  MOCK_METHOD(void, OnRequestDecodingError,
              (struct AlpacaRequest &, enum EHttpStatusCode, class Print &),
              (override));
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_LISTENER_H_
