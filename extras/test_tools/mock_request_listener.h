#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_LISTENER_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_LISTENER_H_

#include "googletest/gmock.h"
#include "request_listener.h"

namespace alpaca {

class MockRequestListener : public RequestListener {
 public:
  MOCK_METHOD(void, OnStartDecoding, (struct AlpacaRequest &), (override));

  MOCK_METHOD(_Bool, OnRequestDecoded, (struct AlpacaRequest &, class Print &),
              (override));

  MOCK_METHOD(void, OnRequestDecodingError,
              (struct AlpacaRequest &, enum EHttpStatusCode, class Print &),
              (override));
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_LISTENER_H_
