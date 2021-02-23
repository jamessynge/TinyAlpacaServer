#ifndef TINY_ALPACA_SERVER_EXTRAS_TESTS_REQUEST_DECODER_LISTENER_MOCK_H_
#define TINY_ALPACA_SERVER_EXTRAS_TESTS_REQUEST_DECODER_LISTENER_MOCK_H_

// Mock implementation of RequestDecoderListener, used in tests.
//
// Author: james.synge@gmail.com

#include "googletest/gmock.h"
#include "src/request_decoder_listener.h"

namespace alpaca {

class MockRequestDecoderListener : public RequestDecoderListener {
 public:
  MOCK_METHOD(EDecodeStatus, OnExtraParameter, (EParameter, const StringView&),
              (override));
  MOCK_METHOD(EDecodeStatus, OnExtraHeader, (EHttpHeader, const StringView&),
              (override));
  MOCK_METHOD(EDecodeStatus, OnUnknownParameterName, (const StringView&),
              (override));
  MOCK_METHOD(EDecodeStatus, OnUnknownParameterValue, (const StringView&),
              (override));
  MOCK_METHOD(EDecodeStatus, OnUnknownHeaderName, (const StringView&),
              (override));
  MOCK_METHOD(EDecodeStatus, OnUnknownHeaderValue, (const StringView&),
              (override));
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TESTS_REQUEST_DECODER_LISTENER_MOCK_H_