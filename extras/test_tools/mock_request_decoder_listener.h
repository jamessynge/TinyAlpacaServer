#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_DECODER_LISTENER_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_DECODER_LISTENER_H_

// Mock implementation of RequestDecoderListener, used in tests.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "constants.h"
#include "gmock/gmock.h"
#include "request_decoder_listener.h"

namespace alpaca {
namespace test {

#if TAS_ENABLE_REQUEST_DECODER_LISTENER

class MockRequestDecoderListener : public RequestDecoderListener {
 public:
#if TAS_ENABLE_ASSET_PATH_DECODING
  MOCK_METHOD(EHttpStatusCode, OnAssetPathSegment, (const mcucore::StringView&),
              (override));
#endif  // TAS_ENABLE_ASSET_PATH_DECODING

#if TAS_ENABLE_EXTRA_PARAMETER_DECODING
  MOCK_METHOD(EHttpStatusCode, OnExtraParameter,
              (EParameter, const mcucore::StringView&), (override));
#endif  // TAS_ENABLE_EXTRA_PARAMETER_DECODING

#if TAS_ENABLE_UNKNOWN_PARAMETER_DECODING
  MOCK_METHOD(EHttpStatusCode, OnUnknownParameterName,
              (const mcucore::StringView&), (override));
  MOCK_METHOD(EHttpStatusCode, OnUnknownParameterValue,
              (const mcucore::StringView&), (override));
#endif  // TAS_ENABLE_UNKNOWN_PARAMETER_DECODING

#if TAS_ENABLE_EXTRA_HEADER_DECODING
  MOCK_METHOD(EHttpStatusCode, OnExtraHeader,
              (EHttpHeader, const mcucore::StringView&), (override));
#endif  // TAS_ENABLE_EXTRA_HEADER_DECODING

#if TAS_ENABLE_UNKNOWN_HEADER_DECODING
  MOCK_METHOD(EHttpStatusCode, OnUnknownHeaderName,
              (const mcucore::StringView&), (override));
  MOCK_METHOD(EHttpStatusCode, OnUnknownHeaderValue,
              (const mcucore::StringView&), (override));
#endif  // TAS_ENABLE_UNKNOWN_HEADER_DECODING
};

#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_MOCK_REQUEST_DECODER_LISTENER_H_
