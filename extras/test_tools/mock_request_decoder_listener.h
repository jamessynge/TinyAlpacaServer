#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_DECODER_LISTENER_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_DECODER_LISTENER_H_

// Mock implementation of RequestDecoderListener, used in tests.
//
// Author: james.synge@gmail.com

#include "constants.h"
#include "googletest/gmock.h"
#include "request_decoder_listener.h"
#include "utils/string_view.h"

namespace alpaca {
namespace test {

class MockRequestDecoderListener : public RequestDecoderListener {
 public:
  MOCK_METHOD(EHttpStatusCode, OnExtraParameter,
              (EParameter, const StringView&), (override));
  MOCK_METHOD(EHttpStatusCode, OnExtraHeader, (EHttpHeader, const StringView&),
              (override));
  MOCK_METHOD(EHttpStatusCode, OnUnknownParameterName, (const StringView&),
              (override));
  MOCK_METHOD(EHttpStatusCode, OnUnknownParameterValue, (const StringView&),
              (override));
  MOCK_METHOD(EHttpStatusCode, OnUnknownHeaderName, (const StringView&),
              (override));
  MOCK_METHOD(EHttpStatusCode, OnUnknownHeaderValue, (const StringView&),
              (override));
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_DECODER_LISTENER_H_
