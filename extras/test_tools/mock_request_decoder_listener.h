#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_DECODER_LISTENER_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_DECODER_LISTENER_H_

// Mock implementation of RequestDecoderListener, used in tests.
//
// Author: james.synge@gmail.com

#include "constants.h"
#include "gmock/gmock.h"
#include "request_decoder_listener.h"
#include "string_view.h"

namespace alpaca {
namespace test {

class MockRequestDecoderListener : public RequestDecoderListener {
 public:
  MOCK_METHOD(EHttpStatusCode, OnExtraParameter,
              (EParameter, const mcucore::StringView&), (override));
  MOCK_METHOD(EHttpStatusCode, OnExtraHeader,
              (EHttpHeader, const mcucore::StringView&), (override));
  MOCK_METHOD(EHttpStatusCode, OnUnknownParameterName,
              (const mcucore::StringView&), (override));
  MOCK_METHOD(EHttpStatusCode, OnUnknownParameterValue,
              (const mcucore::StringView&), (override));
  MOCK_METHOD(EHttpStatusCode, OnUnknownHeaderName,
              (const mcucore::StringView&), (override));
  MOCK_METHOD(EHttpStatusCode, OnUnknownHeaderValue,
              (const mcucore::StringView&), (override));
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_DECODER_LISTENER_H_
