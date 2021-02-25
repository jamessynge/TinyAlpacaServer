#ifndef TINY_ALPACA_SERVER_SRC_TOKENS_H_
#define TINY_ALPACA_SERVER_SRC_TOKENS_H_

// Defines the set of recognized HTTP methods, and ASCOM Alpaca Device Types,
// Methods and Parameters.
//
// Author: james.synge@gmail.com

#include "src/config.h"
#include "src/decoder_constants.h"
#include "src/platform.h"
#include "src/string_view.h"
#include "src/token.h"

namespace alpaca {

TAS_CONSTEXPR_VAR Token<EHttpMethod> kRecognizedHttpMethods[] = {
    // These strings must be UPPERCASE.
    MakeToken("GET", EHttpMethod::GET),
    MakeToken("PUT", EHttpMethod::PUT),
    MakeToken("HEAD", EHttpMethod::HEAD),
};

TAS_CONSTEXPR_VAR Token<EDeviceType> kRecognizedDeviceTypes[] = {
    // Devices types we're currently supporting.
    // These strings must be LOWERCASE.
    MakeToken("observingconditions", EDeviceType::kObservingConditions),
    MakeToken("safetymonitor", EDeviceType::kSafetyMonitor),
};

TAS_CONSTEXPR_VAR Token<EMethod> kRecognizedAscomMethods[] = {
    // TBD which of these will actually be supported.
    // These strings must be LOWERCASE.

    // ASCOM common methods:
    MakeToken("connected", EMethod::kConnected),
    MakeToken("description", EMethod::kDescription),
    MakeToken("driverinfo", EMethod::kDriverInfo),
    MakeToken("driverversion", EMethod::kDriverVersion),
    MakeToken("interfaceversion", EMethod::kInterfaceVersion),
    MakeToken("name", EMethod::kName),
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

TAS_CONSTEXPR_VAR Token<EParameter> kRecognizedParameters[] = {
    // These strings must be LOWERCASE.
    MakeToken("clientid", EParameter::kClientId),
    MakeToken("clienttransactionid", EParameter::kClientTransactionId),
    MakeToken("connected", EParameter::kConnected),
};

TAS_CONSTEXPR_VAR Token<EHttpHeader> kRecognizedHttpHeaders[] = {
    // These strings must be LOWERCASE.
    MakeToken("accept", EHttpHeader::kHttpAccept),
    MakeToken("content-length", EHttpHeader::kHttpContentLength),
    MakeToken("content-type", EHttpHeader::kHttpContentType),
    MakeToken("content-encoding", EHttpHeader::kHttpContentEncoding),
};

namespace internal {

template <size_t N, typename E>
TAS_CONSTEXPR_FUNC StringView::size_type MaxTokenSizeHelper(
    const Token<E> (&tokens)[N], const size_t ndx) {
  return (ndx >= N) ? 0
                    : MaxOf2(tokens[ndx].str.size(),
                             (MaxTokenSizeHelper<N, E>(tokens, ndx + 1)));
}

template <size_t N, typename E>
TAS_CONSTEXPR_FUNC StringView::size_type MaxTokenSize(
    const Token<E> (&tokens)[N]) {
  return MaxTokenSizeHelper<N, E>(tokens, 0);
}

}  // namespace internal

// To help guide the selection of the buffer size to be used when reading an
// HTTP request from the TCP connection. If the size isn't at least this size,
// then if the client sends a request with a larger token, the decoder will
// repeatedly return kNeedMoreInput, but there won't be enough room in the
// buffer for more input.
TAS_CONSTEXPR_VAR StringView::size_type kMinRequiredBufferSize =
    MaxOf4(internal::MaxTokenSize(kRecognizedDeviceTypes),
           internal::MaxTokenSize(kRecognizedAscomMethods),
           internal::MaxTokenSize(kRecognizedParameters),
           internal::MaxTokenSize(kRecognizedHttpHeaders));

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_TOKENS_H_
