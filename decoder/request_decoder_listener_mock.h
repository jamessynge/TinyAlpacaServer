#ifndef TINY_ALPACA_SERVER_DECODER_REQUEST_DECODER_LISTENER_MOCK_H_
#define TINY_ALPACA_SERVER_DECODER_REQUEST_DECODER_LISTENER_MOCK_H_

#include "gmock/gmock.h"
#include "tiny-alpaca-server/common/string_view.h"
#include "tiny-alpaca-server/decoder/constants.h"
#include "tiny-alpaca-server/decoder/request_decoder_listener.h"

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

#endif  // TINY_ALPACA_SERVER_DECODER_REQUEST_DECODER_LISTENER_MOCK_H_
