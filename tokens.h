#ifndef ALPACA_DECODER_TOKENS_H_
#define ALPACA_DECODER_TOKENS_H_

#include "alpaca-decoder/config.h"
#include "alpaca-decoder/constants.h"
#include "alpaca-decoder/string_view.h"
#include "alpaca-decoder/token.h"

namespace alpaca {

ALPACA_CONSTEXPR_VAR Token<EHttpMethod> kRecognizedHttpMethods[] = {
    MakeToken("GET", EHttpMethod::GET),
    MakeToken("PUT", EHttpMethod::PUT),
    MakeToken("HEAD", EHttpMethod::HEAD),
};

ALPACA_CONSTEXPR_VAR Token<EDeviceType> kRecognizedDeviceTypes[] = {
    // Devices types we're currently supporting.
    MakeToken("observingconditions", EDeviceType::kObservingConditions),
    MakeToken("safetymonitor", EDeviceType::kSafetyMonitor),
};

ALPACA_CONSTEXPR_VAR Token<EMethod> kRecognizedAscomMethods[] = {
    // TBD which of these will actually be supported.

    // ASCOM common methods:
    MakeToken("connected", EMethod::kConnected),
    MakeToken("description", EMethod::kDescription),
    MakeToken("driverinfo", EMethod::kDriverInfo),
    MakeToken("driverversion", EMethod::kDriverVersion),
    MakeToken("interfaceversion", EMethod::kInterfaceVersion),
    MakeToken("names", EMethod::kNames),
    MakeToken("supportedactions", EMethod::kSupportedActions),

    // ObservingConditions methods:
    MakeToken("averageperiod", EMethod::kAveragePeriod),
    MakeToken("cloudcover", EMethod::kCloudCover),
    MakeToken("dewpoint", EMethod::kDewPoint),
    MakeToken("humidity", EMethod::kHumidity),
    MakeToken("pressure", EMethod::kPressure),
    MakeToken("refresh", EMethod::kRefresh),
    MakeToken("rainrate", EMethod::kRainRate),
    MakeToken("temperature", EMethod::kTemperature),
    MakeToken("sensordescription", EMethod::kSensorDescription),

    // SafetyMonitor methods:
    MakeToken("issafe", EMethod::kIsSafe),
};

ALPACA_CONSTEXPR_VAR Token<EParameter> kRecognizedParameters[] = {
    MakeToken("clientid", EParameter::kClientId),
    MakeToken("clienttransactionid", EParameter::kClientTransactionId),
    MakeToken("connected", EParameter::kConnected),
};

ALPACA_CONSTEXPR_VAR Token<EHttpHeader> kRecognizedHttpHeaders[] = {
    MakeToken("accept", EHttpHeader::kHttpAccept),
    MakeToken("content-length", EHttpHeader::kHttpContentLength),
    MakeToken("content-type", EHttpHeader::kHttpContentType),
};

namespace internal {

template <size_t N, typename E>
ALPACA_CONSTEXPR_FUNC StringView::size_type MaxTokenSizeHelper(
    const Token<E> (&tokens)[N], const size_t ndx) {
  if (ndx >= N) {
    return 0;
  } else {
    return std::max(tokens[ndx].str.size(),
                    MaxTokenSizeHelper<N, E>(tokens, ndx + 1));
  }
}

template <size_t N, typename E>
ALPACA_CONSTEXPR_FUNC StringView::size_type MaxTokenSize(
    const Token<E> (&tokens)[N]) {
  return MaxTokenSizeHelper<N, E>(tokens, 0);
}
}  // namespace internal

// To help guide the selection of the buffer size to be used when reading an
// HTTP request from the TCP connection. If the size isn't at least this size,
// then if the client sends a request with a larger token, the decoder will
// repeatedly return kNeedMoreInput, but there won't be enough room in the
// buffer for more input.
ALPACA_CONSTEXPR_VAR StringView::size_type kMinRequiredBufferSize =
    std::max({internal::MaxTokenSize(kRecognizedDeviceTypes),
              internal::MaxTokenSize(kRecognizedAscomMethods),
              internal::MaxTokenSize(kRecognizedParameters),
              internal::MaxTokenSize(kRecognizedHttpHeaders)});

}  // namespace alpaca

#endif  // ALPACA_DECODER_TOKENS_H_
