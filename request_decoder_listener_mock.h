#ifndef ALPACA_DECODER_REQUEST_DECODER_LISTENER_MOCK_H_
#define ALPACA_DECODER_REQUEST_DECODER_LISTENER_MOCK_H_

#include "alpaca-decoder/request_decoder_listener.h"
#include "gmock/gmock.h"

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

#endif  // ALPACA_DECODER_REQUEST_DECODER_LISTENER_MOCK_H_
