// Implementation of an HTTP Request Decoder for the ASCOM Alpaca REST API.
// Author: james.synge@gmail.com

// TODO(jamessynge): Consider whether to just return 400 Bad Request for almost
// all problems with parsing, rather than including "elaborate" efforts to
// validate the input. One choice is to add a macro guard (i.e. for #ifdef
// blocks) so that we can enable the features when there is enough code space,
// but omit when there isn't.
//
// Another way to reduce the size of *this* code is to pass the buck to the
// calling code (i.e. via the listener). If we limit this decoder to just the
// request headers, and not the body, then the we don't need to know about any
// of the header names.
//
// Author: james.synge@gmail.com

#ifndef REQUEST_DECODER_ENABLE_MCU_VLOG
#define MCU_DISABLE_VLOG
#endif

#include "request_decoder.h"

#include <McuCore.h>

#include "config.h"
#include "constants.h"
#include "literals.h"
#include "match_literals.h"

// NOTE: The syntax for the query portion of a URI is not as clearly specified
// as the rest of HTTP (AFAICT), so I'm assuming that:
//
// 1) A name is composed of upper and/or lower case ASCII letters (because those
//    are used by ASCOM Alpaca);
// 2) A value is any non-control character value, excluding space.
// 3) A name is followed by '=' and then by a value.
// 4) A name may not be empty, but some values may be empty; for example,
//    ClientID must be specified in order for this decoder to translate from
//    string to int, but some other parameters have their semantics provided by
//    the calling program, so we defer validation to that calling program
// 5) The HTTP client will not send percent encoded characters; these are are
//    not detected and decoded by this decoder, and are just treated as regular
//    characters. If they're in values that must be explicitly matched or
//    decoded by this decoder, that operation is likely to fail.

namespace alpaca {
namespace {
using DecodeFunction = RequestDecoderState::DecodeFunction;
using CharMatchFunction = bool (*)(char c);

////////////////////////////////////////////////////////////////////////////////
// Helpers for decoder functions.

bool DecodeTrueFalse(const mcucore::StringView& value, bool& output) {
  if (mcucore::CaseEqual(value, ProgmemStringViews::False())) {
    output = false;
    return true;
  } else if (mcucore::CaseEqual(value, ProgmemStringViews::True())) {
    output = true;
    return true;
  } else {
    return false;
  }
}

MCU_MAYBE_UNUSED_FUNCTION EHttpStatusCode
EnsureIsError(EHttpStatusCode status,
              EHttpStatusCode default_code = EHttpStatusCode::kHttpBadRequest) {
  MCU_DCHECK_NE(status, EHttpStatusCode::kNeedMoreInput);
  if (status < EHttpStatusCode::kHttpBadRequest) {
    return default_code;
  } else {
    return status;
  }
}

bool HttpMethodIsRead(EHttpMethod method) {
  return method == EHttpMethod::GET || method == EHttpMethod::HEAD;
}

bool HttpMethodHasBody(EHttpMethod method) {
  return method == EHttpMethod::PUT;
}

bool IsOptionalWhitespace(const char c) { return c == ' ' || c == '\t'; }

bool IsParamSeparator(const char c) { return c == '&'; }

bool IsEndOfPath(const char c) { return c == ' ' || c == '?'; }

// Per RFC7230, Section 3.2, Header-Fields.
bool IsFieldContent(const char c) { return isPrintable(c) || c == '\t'; }

// Match characters in either a URI query param or a header name; actually, just
// the subset of such characters we need to match for ASCOM Alpaca. Since we
// compare matching strings against tokens to find those we're interested in,
// having this set contain extra characters for some context doesn't really
// matter.
MCU_CONSTEXPR_VAR mcucore::StringView kExtraNameChars("-_.");
bool IsNameChar(const char c) {
  return isAlphaNumeric(c) || kExtraNameChars.contains(c);
}

// Match characters allowed in a URL encoded parameter value, whether in the
// path or in the body of a PUT request.
MCU_CONSTEXPR_VAR mcucore::StringView kExtraParamValueChars("-+_=%.");
bool IsParamValueChar(const char c) {
  return isAlphaNumeric(c) || kExtraParamValueChars.contains(c);
}

mcucore::StringView::size_type FindFirstNotOf(const mcucore::StringView& view,
                                              bool (*test)(char)) {
  for (mcucore::StringView::size_type pos = 0; pos < view.size(); ++pos) {
    if (!test(view.at(pos))) {
      return pos;
    }
  }
  return mcucore::StringView::kMaxSize;
}

// Removes leading whitespace characters, returns true when the first character
// is not a whitespace.
bool SkipLeadingOptionalWhitespace(mcucore::StringView& view) {
  const auto beyond = FindFirstNotOf(view, IsOptionalWhitespace);
  if (beyond == mcucore::StringView::kMaxSize) {
    // They're all whitespace (or it is empty). Get rid of them. Choosing here
    // to treat this as a remove_prefix rather than a clear, so that tests see
    // that the pointer in the view is moved forward by the number of removed
    // characters.
    view.remove_prefix(view.size());
    // Since there are no characters in the view, we don't know if the next
    // input character will be a space or not, so we can't report true yet.
    return false;
  } else {
    view.remove_prefix(beyond);
    return true;
  }
}

void TrimTrailingOptionalWhitespace(mcucore::StringView& view) {
  while (!view.empty() && IsOptionalWhitespace(view.back())) {
    view.remove_suffix(1);
  }
}

bool ExtractMatchingPrefix(mcucore::StringView& view,
                           mcucore::StringView& extracted_prefix,
                           CharMatchFunction char_matcher) {
  auto beyond = FindFirstNotOf(view, char_matcher);
  MCU_VLOG(3) << MCU_PSD("ExtractMatchingPrefix of ")
              << mcucore::HexEscaped(view) << MCU_PSD(" found ") << (beyond + 0)
              << MCU_PSD(" matching characters");
  if (beyond == mcucore::StringView::kMaxSize) {
    return false;
  }
  extracted_prefix = view.prefix(beyond);
  view.remove_prefix(beyond);
  return true;
}

using NameProcessor = EHttpStatusCode (*)(
    RequestDecoderState& state, const mcucore::StringView& matched_text,
    mcucore::StringView& remainder_view);

EHttpStatusCode ExtractAndProcessName(
    RequestDecoderState& state, mcucore::StringView& view,
    const char valid_terminating_char, const NameProcessor processor,
    const bool consume_terminator_char,
    const EHttpStatusCode bad_terminator_error) {
  MCU_DCHECK_GT(bad_terminator_error, EHttpStatusCode::kHttpOk);
  mcucore::StringView matched_text;
  if (!ExtractMatchingPrefix(view, matched_text, IsNameChar)) {
    // We didn't find a character that IsNameChar doesn't match, so we don't
    // know if we have enough input yet.
    return EHttpStatusCode::kNeedMoreInput;
  }
  MCU_DCHECK(!view.empty());

  if (!view.starts_with(valid_terminating_char)) {
    // Doesn't end with something appropriate for the path to end in. Perhaps an
    // unexpected/unsupported delimiter. Reporting Not Found because the error
    // is with the path.
    return bad_terminator_error;
  } else if (consume_terminator_char) {
    view.remove_prefix(1);
  }

  return processor(state, matched_text, view);
}

EHttpStatusCode ExtractAndProcessName(RequestDecoderState& state,
                                      mcucore::StringView& view,
                                      const NameProcessor processor) {
  mcucore::StringView matched_text;
  if (!ExtractMatchingPrefix(view, matched_text, IsNameChar)) {
    // We didn't find a character that IsNameChar doesn't match, so we don't
    // know if we have enough input yet.
    return EHttpStatusCode::kNeedMoreInput;
  }
  MCU_DCHECK(!view.empty());
  return processor(state, matched_text, view);
}

////////////////////////////////////////////////////////////////////////////////
// Decoder functions for different phases of decoding. Generally in reverse
// order to avoid forward declarations.

// Necessary forward declarations (whereever we have a cycle in the grammar).
#if TAS_ENABLE_ASSET_PATH_DECODING
EHttpStatusCode DecodeAssetPath(RequestDecoderState& state,
                                mcucore::StringView& view);
#endif  // TAS_ENABLE_ASSET_PATH_DECODING
EHttpStatusCode DecodeParamName(RequestDecoderState& state,
                                mcucore::StringView& view);
EHttpStatusCode DecodeHeaderLines(RequestDecoderState& state,
                                  mcucore::StringView& view);

EHttpStatusCode DecodeHeaderLineEnd(RequestDecoderState& state,
                                    mcucore::StringView& view) {
  // We expect "\r\n" at the end of a header line.
  const auto kEndOfHeaderLine = ProgmemStringViews::HttpEndOfLine();
  if (mcucore::SkipPrefix(view, kEndOfHeaderLine)) {
    return state.SetDecodeFunction(DecodeHeaderLines);
  } else if (mcucore::StartsWith(kEndOfHeaderLine, view)) {
    // Need more input.
    return EHttpStatusCode::kNeedMoreInput;
  } else {
    // The header line doesn't end where or as expected; perhaps the EOL
    // terminator isn't correct (e.g. a "\n" instead of a "\r\n").
    return EHttpStatusCode::kHttpBadRequest;
  }
}

EHttpStatusCode ReportInvalidHeaderValue(
    RequestDecoderState& state, mcucore::StringView& value,
    EHttpStatusCode default_code = EHttpStatusCode::kHttpBadRequest) {
#if TAS_ENABLE_EXTRA_HEADER_DECODING
  if (state.listener) {
    return EnsureIsError(
        state.listener->OnExtraHeader(state.current_header, value),
        default_code);
  }
#endif  // TAS_ENABLE_EXTRA_HEADER_DECODING
  return default_code;
}

// TODO(jamessynge): Define a better API for handling LONG header values without
// having to buffer them, i.e. those which can be quite long, such as Accept.
EHttpStatusCode DecodeHeaderValue(RequestDecoderState& state,
                                  mcucore::StringView& view) {
  // Skip leading OWS (optional whitespace: space or horizontal tab), then take
  // all of the characters matching IsFieldContent, up to the first
  // non-matching character. If we can't find a non-matching character, we need
  // more input.
  mcucore::StringView value;
  if (!SkipLeadingOptionalWhitespace(view) ||
      !ExtractMatchingPrefix(view, value, IsFieldContent)) {
    return EHttpStatusCode::kNeedMoreInput;
  }
  MCU_VLOG(1) << MCU_PSD("DecodeHeaderValue raw value: ")
              << mcucore::HexEscaped(value);
  // Trim OWS from the end of the header value.
  TrimTrailingOptionalWhitespace(value);
  MCU_VLOG(1) << MCU_PSD("DecodeHeaderValue trimmed value: ")
              << mcucore::HexEscaped(value);
  EHttpStatusCode status = EHttpStatusCode::kContinueDecoding;
  if (state.current_header == EHttpHeader::kContentLength) {
    if (state.found_content_length) {
      // Can't combine two Content-Length header fields.
      return ReportInvalidHeaderValue(state, value);
    }
    uint32_t content_length = 0;
    const bool converted_ok = value.to_uint32(content_length);
    if (!converted_ok) {
      // Illformed.
      return ReportInvalidHeaderValue(state, value);
    } else if (content_length > 0 &&
               !HttpMethodHasBody(state.request.http_method)) {
      // We could choose to skip over the payload of the request, but we don't
      // know if the client intended the payload to have meaning. Thus, we
      // reject a request with an unexpected payload. For more info, see:
      // https://tools.ietf.org/html/rfc7231#section-4.3.1
      return ReportInvalidHeaderValue(state, value);
    } else if (content_length > RequestDecoderState::kMaxPayloadSize) {
      // It's out of range for our decoder.
      return ReportInvalidHeaderValue(state, value,
                                      EHttpStatusCode::kHttpPayloadTooLarge);
    } else {
      // Looks OK. Note that this isn't stored in the AlpacaRequest because the
      // decoder takes care of processing the body and extracting parameters
      // from it.
      state.remaining_content_length = content_length;
      state.found_content_length = true;
    }
  } else if (state.current_header == EHttpHeader::kContentType) {
    // Note that the syntax for the Content-Type header is more complex than
    // allowed for here; after the media-type there may be a semi-colon and some
    // additional details (charset and boundary). So far we only support
    // url-encoded, so if any other value is provided, return an error.
    if (value != ProgmemStringViews::MimeTypeWwwFormUrlEncoded()) {
      return ReportInvalidHeaderValue(
          state, value, EHttpStatusCode::kHttpUnsupportedMediaType);
    }
    if (!HttpMethodHasBody(state.request.http_method)) {
      // Don't expect this header for GET requests.
      return ReportInvalidHeaderValue(state, value);
    }
    // Note that we don't store the value, just validate it. If it isn't
    // provided, we assume the body will have the correct type. The HTTP RFC
    // says that the header field should be provided, but doesn't say that it
    // must be.
  } else if (state.current_header == EHttpHeader::kConnection) {
    if (mcucore::CaseEqual(ProgmemStringViews::close(), value)) {
      state.request.do_close = true;
    }
  } else if (state.current_header != EHttpHeader::kUnknown) {
#if TAS_ENABLE_EXTRA_HEADER_DECODING
    // Recognized but no built-in support.
    if (state.listener) {
      status = state.listener->OnExtraHeader(state.current_header, value);
    }
#endif  // TAS_ENABLE_EXTRA_HEADER_DECODING
#if TAS_ENABLE_UNKNOWN_HEADER_DECODING
  } else if (state.current_header == EHttpHeader::kUnknown) {
    if (state.listener) {
      status = state.listener->OnUnknownHeaderValue(value);
    }
#endif  // TAS_ENABLE_UNKNOWN_HEADER_DECODING
  }
  return state.SetDecodeFunctionAfterListenerCall(DecodeHeaderLineEnd, status);
}

// We've determined that we don't need to examine the value of this header,
// so we just skip forward to the end of the line. This allows us to skip
// past large headers (e.g. User-Agent) whose value we don't care about.
EHttpStatusCode SkipHeaderValue(RequestDecoderState& state,
                                mcucore::StringView& view) {
  while (!view.empty()) {
    if (view.starts_with('\r')) {
      return state.SetDecodeFunction(DecodeHeaderLineEnd);
    }
    view.remove_prefix(1);
  }
  return EHttpStatusCode::kNeedMoreInput;
}

EHttpStatusCode ProcessHeaderName(RequestDecoderState& state,
                                  const mcucore::StringView& matched_text,
                                  mcucore::StringView& view) {
  if (MatchHttpHeader(matched_text, state.current_header)) {
    return state.SetDecodeFunction(DecodeHeaderValue);
  }
  if (matched_text.empty()) {
    return EHttpStatusCode::kHttpBadRequest;
  }
  state.current_header = EHttpHeader::kUnknown;
#if TAS_ENABLE_UNKNOWN_HEADER_DECODING
  if (state.listener) {
    EHttpStatusCode status = state.listener->OnUnknownHeaderName(matched_text);
    return state.SetDecodeFunctionAfterListenerCall(DecodeHeaderValue, status);
  }
#endif  // TAS_ENABLE_UNKNOWN_HEADER_DECODING
  return state.SetDecodeFunction(SkipHeaderValue);
}

EHttpStatusCode DecodeHeaderName(RequestDecoderState& state,
                                 mcucore::StringView& view) {
  const char kHeaderNameValueSeparator = ':';
  return ExtractAndProcessName(
      state, view, kHeaderNameValueSeparator, ProcessHeaderName,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

EHttpStatusCode DecodeHeaderLines(RequestDecoderState& state,
                                  mcucore::StringView& view) {
  const auto kEndOfHeaderLine = ProgmemStringViews::HttpEndOfLine();
  if (mcucore::SkipPrefix(view, kEndOfHeaderLine)) {
    // We've reached the end of the headers.
    if (state.request.http_method == EHttpMethod::GET) {
      // The standard requires that we not examine the body of a GET
      // request, if present, so we're done.
      return EHttpStatusCode::kHttpOk;
    } else if (state.request.http_method != EHttpMethod::PUT) {
      // Shouldn't get here unless support for a new method is added to
      // DecodeHttpMethod, but not to here, or else if there is a bug.
      return EHttpStatusCode::kHttpInternalServerError;  // COV_NF_LINE
    } else if (!state.found_content_length) {
      // We need to know the length in order to decode the body.
      return EHttpStatusCode::kHttpLengthRequired;
    } else if (state.remaining_content_length == 0) {
      // Very odd, but it is possible that all of the parameters are in the
      // query parameters in the start line of the request. For example, the
      // "refresh" method of the "observingconditions" device type requires
      // no parameters.
      return EHttpStatusCode::kHttpOk;
    } else {
      // There is a body of known length to be decoded.
      state.is_decoding_header = false;
      state.decode_function = DecodeParamName;
      return EHttpStatusCode::kNeedMoreInput;
    }
  } else if (mcucore::StartsWith(kEndOfHeaderLine, view)) {
    // view might be empty, or it might be the start of a HttpEndOfLine.
    // To decide what to do next, we need more input.
    return EHttpStatusCode::kNeedMoreInput;
  } else {
    return state.SetDecodeFunction(DecodeHeaderName);
  }
}

// An HTTP/1.1 Request Start Line should always end with "HTTP/1.1\r\n".
// (We're not supporting HTTP/1.0 or earlier.)
EHttpStatusCode MatchHttpVersion(RequestDecoderState& state,
                                 mcucore::StringView& view) {
  const auto expected = ProgmemStringViews::HttpVersionEndOfLine();
  if (mcucore::StartsWith(view, expected)) {
    view.remove_prefix(expected.size());
    state.is_decoding_start_line = false;
    return state.SetDecodeFunction(DecodeHeaderLines);
  } else if (view.size() < expected.size()) {
    return EHttpStatusCode::kNeedMoreInput;
  } else {
    return EHttpStatusCode::kHttpVersionNotSupported;
  }
}

EHttpStatusCode DecodeParamSeparator(RequestDecoderState& state,
                                     mcucore::StringView& view) {
  // Forget what we were decoding previously.
  state.current_parameter = EParameter::kUnknown;

  // If there are multiple separators, treat them as one.
  const auto beyond = FindFirstNotOf(view, IsParamSeparator);
  if (beyond == mcucore::StringView::kMaxSize) {
    MCU_VLOG(3) << MCU_PSD("DecodeParamSeparator found no non-separators in ")
                << mcucore::HexEscaped(view);
    // All the available characters are separators, or the view is empty.
    if (!state.is_decoding_header && state.is_final_input) {
      // We've reached the end of the body of the request.
      view.remove_prefix(view.size());
      return EHttpStatusCode::kHttpOk;
    }
    // We don't know if the next character will also be a separator or not,
    // so we remove all but one of the separator characters, and return here
    // next time when there is more input.
    if (view.size() > 1) {
      view.remove_prefix(view.size() - 1);
    }
    return EHttpStatusCode::kNeedMoreInput;
  }

  // There are zero or more separators, followed by a non-separator. This
  // means that this isn't the body of a request with one of these
  // separators as the last char in the body, so we don't need to worry
  // about that case.
  MCU_VLOG(3) << MCU_PSD("DecodeParamSeparator found ") << (beyond + 0)
              << MCU_PSD(" separators, followed by a non-separator");
  MCU_DCHECK(!view.empty());

  view.remove_prefix(beyond);
  if (view.front() == ' ') {
    if (state.is_decoding_header) {
      view.remove_prefix(1);
      return state.SetDecodeFunction(MatchHttpVersion);
    }
    MCU_VLOG(3) << MCU_PSD("Found an unexpected space in body");
    return EHttpStatusCode::kHttpBadRequest;
  } else {
    return state.SetDecodeFunction(DecodeParamName);
  }
}

EHttpStatusCode RemoveInvalidParamValue(RequestDecoderState& state,
                                        mcucore::StringView& view) {
#if TAS_ENABLE_EXTRA_PARAMETER_DECODING
  if (state.listener) {
    return EnsureIsError(
        state.listener->OnExtraParameter(state.current_parameter, view));
  }
#endif  // TAS_ENABLE_EXTRA_PARAMETER_DECODING
  return EHttpStatusCode::kHttpBadRequest;
}

// Note that a parameter value may be empty, which makes detecting the end
// of it tricky if also at the end of the body of a request.
EHttpStatusCode DecodeParamValue(RequestDecoderState& state,
                                 mcucore::StringView& view) {
  mcucore::StringView value;
  if (!ExtractMatchingPrefix(view, value, IsParamValueChar)) {
    // view doesn't contain a character that can't be in a parameter value.
    // We may need more input.
    if (state.is_decoding_header || !state.is_final_input) {
      return EHttpStatusCode::kNeedMoreInput;
    }
    // Ah, we're decoding the body of the request, and this is last buffer
    // of input from the client, so we can treat the end of input as the
    // separator.
    MCU_DCHECK_EQ(state.remaining_content_length, view.size());
    value = view;
    view.remove_prefix(value.size());
  }
  MCU_VLOG(1) << MCU_PSD("DecodeParamValue param: ") << state.current_parameter
              << MCU_PSD(", value: ") << mcucore::HexEscaped(value);
  EHttpStatusCode status = EHttpStatusCode::kContinueDecoding;
  if (state.current_parameter == EParameter::kClientID) {
    uint32_t id;
    bool converted_ok = value.to_uint32(id);
    if (!converted_ok) {
      return RemoveInvalidParamValue(state, value);
    } else {
      // If already decoded, then we overwrite the previous value.
      state.request.set_client_id(id);
    }
  } else if (state.current_parameter == EParameter::kClientTransactionID) {
    uint32_t id;
    bool converted_ok = value.to_uint32(id);
    if (!converted_ok) {
      return RemoveInvalidParamValue(state, value);
    } else {
      // If already decoded, then we overwrite the previous value.
      state.request.set_client_transaction_id(id);
    }
  } else if (state.current_parameter == EParameter::kId) {
    uint32_t id;
    bool converted_ok = value.to_uint32(id);
    if (!converted_ok) {
      return RemoveInvalidParamValue(state, value);
    } else {
      // If already decoded, then we overwrite the previous value.
      state.request.set_id(id);
    }
  } else if (state.current_parameter == EParameter::kBrightness) {
    int32_t brightness;
    bool converted_ok = value.to_int32(brightness);
    if (!converted_ok) {
      return RemoveInvalidParamValue(state, value);
    } else {
      // If already decoded, then we overwrite the previous value.
      state.request.set_brightness(brightness);
    }
  } else if (state.current_parameter == EParameter::kValue) {
    double d;
    bool converted_ok = value.to_double(d);
    if (!converted_ok) {
      return RemoveInvalidParamValue(state, value);
    } else {
      // If already decoded, then we overwrite the previous value.
      state.request.set_value(d);
    }
  } else if (state.current_parameter == EParameter::kAveragePeriod) {
    double d;
    bool converted_ok = value.to_double(d);
    if (!converted_ok) {
      return RemoveInvalidParamValue(state, value);
    } else {
      // If already decoded, then we overwrite the previous value.
      state.request.set_average_period(d);
    }
  } else if (state.current_parameter == EParameter::kConnected) {
    bool b;
    bool converted_ok = DecodeTrueFalse(value, b);
    if (!converted_ok) {
      return RemoveInvalidParamValue(state, value);
    } else {
      // If already decoded, then we overwrite the previous value.
      state.request.set_connected(b);
    }
  } else if (state.current_parameter == EParameter::kState) {
    bool b;
    bool converted_ok = DecodeTrueFalse(value, b);
    if (!converted_ok) {
      return RemoveInvalidParamValue(state, value);
    } else {
      // If already decoded, then we overwrite the previous value.
      state.request.set_state(b);
    }
  } else if (state.current_parameter == EParameter::kSensorName) {
    ESensorName matched;
    if (!MatchSensorName(value, matched)) {
      return RemoveInvalidParamValue(state, value);
    } else {
      state.request.sensor_name = matched;
    }
  } else if (state.current_parameter == EParameter::kName) {
    // We don't yet have have unique storage for name, vs. any other
    // parameter that might need to use the AlpacaRequest.string_value field
    // (none so far). Throwing caution to the wind, I'm just assuming
    // another use won't be added soon.
    // TODO(jamessynge): Switch to using SerialMap<EParameter> (or similar)
    // for storing the values of parameters.
    state.request.have_string_value = 0;
    if (!state.request.set_string_value(value)) {
      return RemoveInvalidParamValue(state, value);
    }
#if TAS_ENABLE_EXTRA_PARAMETER_DECODING
  } else if (state.current_parameter != EParameter::kUnknown) {
    // Recognized but no built-in support.
    if (state.listener) {
      status = state.listener->OnExtraParameter(state.current_parameter, value);
    }
#endif  // TAS_ENABLE_EXTRA_PARAMETER_DECODING
#if TAS_ENABLE_UNKNOWN_PARAMETER_DECODING
  } else if (state.current_parameter == EParameter::kUnknown) {
    if (state.listener) {
      status = state.listener->OnUnknownParameterValue(value);
    }
#endif  // TAS_ENABLE_UNKNOWN_PARAMETER_DECODING
  }
  return state.SetDecodeFunctionAfterListenerCall(DecodeParamSeparator, status);
}

EHttpStatusCode ProcessParamName(RequestDecoderState& state,
                                 const mcucore::StringView& matched_text,
                                 mcucore::StringView& view) {
  state.current_parameter = EParameter::kUnknown;
  if (MatchParameter(matched_text, state.current_parameter)) {
    return state.SetDecodeFunction(DecodeParamValue);
  }
  if (matched_text.empty()) {
    // Missing parameter name.
    return EHttpStatusCode::kHttpBadRequest;
  }
  // Unrecognized parameter name.
#if TAS_ENABLE_UNKNOWN_PARAMETER_DECODING
  if (state.listener) {
    EHttpStatusCode status =
        state.listener->OnUnknownParameterName(matched_text);
    return state.SetDecodeFunctionAfterListenerCall(DecodeParamValue, status);
  }
#endif  // TAS_ENABLE_UNKNOWN_PARAMETER_DECODING
  return state.SetDecodeFunction(DecodeParamValue);
}

EHttpStatusCode DecodeParamName(RequestDecoderState& state,
                                mcucore::StringView& view) {
  // Not supporting the case here of a param without a value, i.e. followed
  // by a space marking the end of the path, a '&' separating it from the
  // next param, or end-of-input in the body. Not needed for ASCOM, but
  // would be for a somewhat more general decoder.
  const char kParamNameValueSeparator = '=';
  return ExtractAndProcessName(
      state, view, kParamNameValueSeparator, ProcessParamName,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

// We've read what should be the final segment of the path, and expect
// either a
// '?' marking the beginning of a query (i.e. parameter names and values),
// or the ' ' (space) that appears before the HTTP version number.
EHttpStatusCode DecodeEndOfPath(RequestDecoderState& state,
                                mcucore::StringView& view) {
  // A separator/terminating character should be present, else we would not
  // have been able to determine that the previous segment was done.
  MCU_DCHECK(!view.empty());
  DecodeFunction next_decode_function;
  if (view.match_and_consume('?')) {
    next_decode_function = DecodeParamName;
  } else if (view.match_and_consume(' ')) {
    next_decode_function = MatchHttpVersion;
  } else {
    // We expected the path to end, but maybe the client send more path
    // segments?
    return EHttpStatusCode::kHttpBadRequest;
  }
  return state.SetDecodeFunction(next_decode_function);
}

EHttpStatusCode ProcessDeviceMethod(RequestDecoderState& state,
                                    const mcucore::StringView& matched_text,
                                    mcucore::StringView& view) {
  MCU_VLOG(3) << MCU_PSD("ProcessDeviceMethod matched_text: ")
              << mcucore::HexEscaped(matched_text);

  // A separator/terminating character should be present after the method.
  MCU_CHECK(!view.empty());

  EDeviceMethod method;
  if (MatchDeviceMethod(state.request.api_group, state.request.device_type,
                        matched_text, method)) {
    MCU_DCHECK(method == EDeviceMethod::kSetup ||
               state.request.api == EAlpacaApi::kDeviceApi)
        << MCU_PSD("Wrong combo: method=") << method << MCU_PSD(", api=")
        << state.request.api;
    MCU_DCHECK(method != EDeviceMethod::kSetup ||
               state.request.api == EAlpacaApi::kDeviceSetup)
        << MCU_PSD("Wrong combo: method=") << method << MCU_PSD(", api=")
        << state.request.api;
    state.request.device_method = method;
    return state.SetDecodeFunction(DecodeEndOfPath);
  }
  return EHttpStatusCode::kHttpBadRequest;
}

EHttpStatusCode DecodeDeviceMethod(RequestDecoderState& state,
                                   mcucore::StringView& view) {
  mcucore::StringView matched_text;
  if (!ExtractMatchingPrefix(view, matched_text, IsNameChar)) {
    // We didn't find a character that IsNameChar doesn't match, so we don't
    // know if we have enough input yet.
    return EHttpStatusCode::kNeedMoreInput;
  }
  MCU_DCHECK(!view.empty());

  // The device method is supposed to be the end of the path; it may be
  // followed by a query string, or a space and then the HTTP version.
  const char next = view.front();
  if (IsEndOfPath(next)) {
    return ProcessDeviceMethod(state, matched_text, view);
  } else {
    // Doesn't end with something appropriate for the path to end in.
    // Perhaps an unexpected/unsupported delimiter. Reporting Bad Request
    // because the error is with the format of the request... though we
    // could report Not Found if what follows is a '/' and another
    // well-formed but invalid path element.
    return EHttpStatusCode::kHttpBadRequest;
  }
}

EHttpStatusCode ProcessDeviceNumber(RequestDecoderState& state,
                                    const mcucore::StringView& matched_text,
                                    mcucore::StringView& view) {
  if (!matched_text.to_uint32(state.request.device_number)) {
    return EHttpStatusCode::kHttpBadRequest;
  }
  return state.SetDecodeFunction(DecodeDeviceMethod);
}

EHttpStatusCode DecodeDeviceNumber(RequestDecoderState& state,
                                   mcucore::StringView& view) {
  const char kPathSeparator = '/';
  return ExtractAndProcessName(
      state, view, kPathSeparator, ProcessDeviceNumber,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

EHttpStatusCode ProcessDeviceType(RequestDecoderState& state,
                                  const mcucore::StringView& matched_text,
                                  mcucore::StringView& view) {
  EDeviceType device_type;
  if (MatchDeviceType(matched_text, device_type)) {
    MCU_VLOG(3) << MCU_PSD("device_type: ") << device_type;
    state.request.device_type = device_type;
    return state.SetDecodeFunction(DecodeDeviceNumber);
  }
  return EHttpStatusCode::kHttpBadRequest;
}

// After the path prefix, we expect the name of a supported device type.
EHttpStatusCode DecodeDeviceType(RequestDecoderState& state,
                                 mcucore::StringView& view) {
  const char kPathSeparator = '/';
  return ExtractAndProcessName(
      state, view, kPathSeparator, ProcessDeviceType,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

// Process the word that starts the path (i.e. right after the leading /).
EHttpStatusCode ProcessApiVersion(RequestDecoderState& state,
                                  const mcucore::StringView& matched_text,
                                  mcucore::StringView& view) {
  if (matched_text == ProgmemStringViews::v1()) {
    return state.SetDecodeFunction(DecodeDeviceType);
  } else {
    return EHttpStatusCode::kHttpBadRequest;
  }
}

EHttpStatusCode DecodeApiVersion(RequestDecoderState& state,
                                 mcucore::StringView& view) {
  const char kPathSeparator = '/';
  return ExtractAndProcessName(
      state, view, kPathSeparator, ProcessApiVersion,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

EHttpStatusCode ProcessManagementMethod(RequestDecoderState& state,
                                        const mcucore::StringView& matched_text,
                                        mcucore::StringView& view) {
  MCU_DCHECK(!view.empty());
  EManagementMethod method;
  if (MatchManagementMethod(matched_text, method)) {
    MCU_VLOG(3) << MCU_PSD("method: ") << method;
    if (method == EManagementMethod::kDescription) {
      state.request.api = EAlpacaApi::kManagementDescription;
    } else if (method == EManagementMethod::kConfiguredDevices) {
      state.request.api = EAlpacaApi::kManagementConfiguredDevices;
    } else {
      // COV_NF_START
      MCU_DCHECK(false) << MCU_PSD("method (") << method
                        << MCU_PSD(") unexpected");
      return EHttpStatusCode::kHttpInternalServerError;
      // COV_NF_END
    }
    return state.SetDecodeFunction(DecodeEndOfPath);
  }
  return EHttpStatusCode::kHttpBadRequest;
}

EHttpStatusCode DecodeManagementMethod(RequestDecoderState& state,
                                       mcucore::StringView& view) {
  return ExtractAndProcessName(state, view, ProcessManagementMethod);
}

// What kind of management operation is this?
EHttpStatusCode ProcessManagementType(RequestDecoderState& state,
                                      const mcucore::StringView& matched_text,
                                      mcucore::StringView& view) {
  MCU_DCHECK(!view.empty());
  if (matched_text == ProgmemStringViews::v1()) {
    if (view.match_and_consume('/')) {
      return state.SetDecodeFunction(DecodeManagementMethod);
    } else {
      return EHttpStatusCode::kHttpBadRequest;
    }
  } else if (matched_text == ProgmemStringViews::apiversions()) {
    state.request.api = EAlpacaApi::kManagementApiVersions;
    return state.SetDecodeFunction(DecodeEndOfPath);
  } else {
    return EHttpStatusCode::kHttpBadRequest;
  }
}

// The path starts "/management/". What's next?
EHttpStatusCode DecodeManagementType(RequestDecoderState& state,
                                     mcucore::StringView& view) {
  return ExtractAndProcessName(state, view, ProcessManagementType);
}

#if TAS_ENABLE_ASSET_PATH_DECODING
// We use this method, rather than having DecodeAssetPath act as a single
// function loop, so as to avoid violating the expectation below that
// kContinueDecoding is only returned when the decode function has changed.
EHttpStatusCode DecodeAssetPathSeparator(RequestDecoderState& state,
                                         mcucore::StringView& view) {
  if (view.match_and_consume('/')) {
    return state.SetDecodeFunction(DecodeAssetPath);
  } else if (view.empty()) {
    return EHttpStatusCode::kNeedMoreInput;
  } else {
    return state.SetDecodeFunction(DecodeEndOfPath);
  }
}

// Handle the next segment in the path under /asset/. The preceding character
// was a slash.
EHttpStatusCode ProcessAssetPath(RequestDecoderState& state,
                                 const mcucore::StringView& matched_text,
                                 mcucore::StringView& view) {
  MCU_DCHECK(!view.empty());
  MCU_DCHECK_NE(state.listener, nullptr);

  // Check for probing of the file system. For example, don't want a long path
  // with many segments all equal ".", nor trying to go upwards with "..".
  if (matched_text.starts_with('.')) {
    if (matched_text.size() == 1) {
      // "." not allowed.
      return EHttpStatusCode::kHttpBadRequest;
    } else if (matched_text.size() == 2 && matched_text.at(1) == '.') {
      // ".." not allowed.
      return EHttpStatusCode::kHttpBadRequest;
    }
  }

  bool is_last_segment = true;
  if (view.starts_with('/')) {
    // Not the end of the path.
    if (matched_text.empty()) {
      // Two slashes in a row, not valid.
      return EHttpStatusCode::kHttpBadRequest;
    }
    is_last_segment = false;
  }
  return state.SetDecodeFunctionAfterListenerCall(
      DecodeAssetPathSeparator,
      state.listener->OnAssetPathSegment(matched_text, is_last_segment));
}

// Handle the next segment in the path under /asset/. The preceding character
// was a slash.
EHttpStatusCode DecodeAssetPath(RequestDecoderState& state,
                                mcucore::StringView& view) {
  MCU_DCHECK_NE(state.listener, nullptr);
  return ExtractAndProcessName(state, view, ProcessAssetPath);
}
#endif  // TAS_ENABLE_ASSET_PATH_DECODING

// Process the word that starts the path, right after the leading '/'.
EHttpStatusCode ProcessApiGroup(RequestDecoderState& state,
                                const mcucore::StringView& matched_text,
                                mcucore::StringView& view) {
  MCU_DCHECK(!view.empty());
  EApiGroup group;
  if (!MatchApiGroup(matched_text, group)) {
    if (!view.empty() && IsEndOfPath(view.front())) {
      // Empty path, which we'll treat as a status page request, reserving
      // "/setup" for configuring the device.
      state.request.api_group = EApiGroup::kServerStatus;
      state.request.api = EAlpacaApi::kServerStatus;
      return state.SetDecodeFunction(DecodeEndOfPath);
    }
    return EHttpStatusCode::kHttpBadRequest;
  }
  state.request.api_group = group;
  if (view.match_and_consume('/')) {
    // The path continues.
    // NOTE: If adding support for more paths (e.g. a PUT or POST request to
    // handle updating parameters in EEPROM), we'll need to adjust this
    // code.
    if (!HttpMethodIsRead(state.request.http_method) &&
        group != EApiGroup::kDevice) {
      return EHttpStatusCode::kHttpMethodNotAllowed;
    }
    if (group == EApiGroup::kDevice) {
      MCU_DCHECK_EQ(group, EApiGroup::kDevice);
      state.request.api = EAlpacaApi::kDeviceApi;
    } else if (group == EApiGroup::kManagement) {
      return state.SetDecodeFunction(DecodeManagementType);
    } else if (group == EApiGroup::kSetup) {
      state.request.api = EAlpacaApi::kDeviceSetup;
    } else if (group == EApiGroup::kAsset) {
#if TAS_ENABLE_ASSET_PATH_DECODING
      if (state.listener != nullptr) {
        state.request.api = EAlpacaApi::kAsset;
        return state.SetDecodeFunction(DecodeAssetPath);
      }
#endif  // TAS_ENABLE_ASSET_PATH_DECODING
      return EHttpStatusCode::kHttpNotFound;
    }
    MCU_DCHECK(group == EApiGroup::kDevice || group == EApiGroup::kSetup)
        << MCU_PSD("group: ") << group;
    return state.SetDecodeFunction(DecodeApiVersion);
  }
  if (group != EApiGroup::kSetup) {
    return EHttpStatusCode::kHttpBadRequest;
  }
  state.request.api = EAlpacaApi::kServerSetup;
  if (!HttpMethodIsRead(state.request.http_method)) {
    return EHttpStatusCode::kHttpMethodNotAllowed;
  }
  // We appear to have reached the end of the path. Handle what comes
  // next.
  return state.SetDecodeFunction(DecodeEndOfPath);
}

// After the '/' at the start of a path, we expect the name of an API group.
EHttpStatusCode DecodeApiGroup(RequestDecoderState& state,
                               mcucore::StringView& view) {
  return ExtractAndProcessName(state, view, ProcessApiGroup);
}

// View should start with '/', once we have at least a character of input.
EHttpStatusCode MatchStartOfPath(RequestDecoderState& state,
                                 mcucore::StringView& view) {
  if (view.empty()) {
    return EHttpStatusCode::kNeedMoreInput;
  } else if (view.match_and_consume('/')) {
    return state.SetDecodeFunction(DecodeApiGroup);
  } else {
    // Don't know what to make of the 'path': it doesn't start with "/".
    return EHttpStatusCode::kHttpBadRequest;
  }
}

// Process the word at the start of the request, which should be the HTTP
// method name. The space following matched_text has already been removed
// from the start of view.
EHttpStatusCode ProcessHttpMethod(RequestDecoderState& state,
                                  const mcucore::StringView& matched_text,
                                  mcucore::StringView& view) {
  EHttpMethod method;
  if (MatchHttpMethod(matched_text, method)) {
    MCU_VLOG(3) << MCU_PSD("method: ") << method;
    state.request.http_method = method;
    return state.SetDecodeFunction(MatchStartOfPath);
  } else if (matched_text.empty()) {
    return EHttpStatusCode::kHttpBadRequest;
  } else {
    return EHttpStatusCode::kHttpNotImplemented;
  }
}

// Decode one of the few supported HTTP methods. If definitely not present,
// returns an error. We *could* allow for leading whitespace, which has been
// supported implementations in the past, perhaps to deal with multiple
// requests (or multiple responses) in a row without clear delimiters.
// However HTTP/1.1 requires clear delimiters, so we won't tolerate extra
// whitespace.
EHttpStatusCode DecodeHttpMethod(RequestDecoderState& state,
                                 mcucore::StringView& view) {
  const char kHttpMethodTerminator = ' ';
  return ExtractAndProcessName(
      state, view, kHttpMethodTerminator, ProcessHttpMethod,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

}  // namespace

size_t PrintValueTo(DecodeFunction decode_function, Print& out) {
#define OUTPUT_METHOD_NAME(symbol) \
  if (decode_function == symbol) return out.print(MCU_FLASHSTR(#symbol))

  OUTPUT_METHOD_NAME(DecodeApiGroup);
  OUTPUT_METHOD_NAME(DecodeApiVersion);
  OUTPUT_METHOD_NAME(DecodeDeviceMethod);
  OUTPUT_METHOD_NAME(DecodeDeviceNumber);
  OUTPUT_METHOD_NAME(DecodeDeviceType);
  OUTPUT_METHOD_NAME(DecodeEndOfPath);
  OUTPUT_METHOD_NAME(DecodeHeaderLineEnd);
  OUTPUT_METHOD_NAME(DecodeHeaderLines);
  OUTPUT_METHOD_NAME(DecodeHeaderName);
  OUTPUT_METHOD_NAME(DecodeHeaderValue);
  OUTPUT_METHOD_NAME(DecodeHttpMethod);
  OUTPUT_METHOD_NAME(DecodeManagementMethod);
  OUTPUT_METHOD_NAME(DecodeManagementType);
  OUTPUT_METHOD_NAME(DecodeParamName);
  OUTPUT_METHOD_NAME(DecodeParamSeparator);
  OUTPUT_METHOD_NAME(DecodeParamValue);
  OUTPUT_METHOD_NAME(MatchHttpVersion);
  OUTPUT_METHOD_NAME(MatchStartOfPath);
  OUTPUT_METHOD_NAME(SkipHeaderValue);

#if TAS_ENABLE_ASSET_PATH_DECODING
  OUTPUT_METHOD_NAME(DecodeAssetPath);
#endif  // TAS_ENABLE_ASSET_PATH_DECODING

#undef OUTPUT_METHOD_NAME

  // COV_NF_START
  MCU_CHECK(false) << MCU_PSD("Haven't implemented a case for decode_function");
  return 0;
  // COV_NF_END
}

#if TAS_ENABLE_REQUEST_DECODER_LISTENER
RequestDecoderState::RequestDecoderState(AlpacaRequest& request)
    : decode_function(nullptr), request(request), listener(nullptr) {}

RequestDecoderState::RequestDecoderState(AlpacaRequest& request,
                                         RequestDecoderListener* listener)
    : decode_function(nullptr), request(request), listener(listener) {}
#else
RequestDecoderState::RequestDecoderState(AlpacaRequest& request)
    : decode_function(nullptr), request(request) {}
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER

void RequestDecoderState::Reset() {
  MCU_VLOG(1) << MCU_FLASHSTR_128(
      "Reset "
      "################################################################");
  decode_function = DecodeHttpMethod;
  request.Reset();
  is_decoding_header = true;
  is_decoding_start_line = true;
  is_final_input = false;
  found_content_length = false;
  decoder_status = RequestDecoderStatus::kReset;
}

EHttpStatusCode RequestDecoderState::DecodeBuffer(mcucore::StringView& buffer,
                                                  const bool buffer_is_full) {
  MCU_VLOG(1) << MCU_PSD("DecodeBuffer ") << mcucore::HexEscaped(buffer);
  if (decode_function == nullptr) {
    // Need to call Reset first.
    //
    // Why not call Reset automatically from the ctor? Because we assume that
    // these objects will have static storage when in the embedded system, and
    // will be used for decoding multiple requests; therefore it doesn't make
    // sense to have special behavior in the caller to omit the first call to
    // Reset.
    return EHttpStatusCode::kHttpInternalServerError;
  }

  MCU_DCHECK_NE(decoder_status, RequestDecoderStatus::kDecoded);
  if (decoder_status == RequestDecoderStatus::kReset) {
    decoder_status = RequestDecoderStatus::kDecoding;
  }

  const auto start_size = buffer.size();
  EHttpStatusCode status;
  if (is_decoding_header) {
    status = DecodeMessageHeader(buffer);
  } else {
    status = DecodeMessageBody(buffer);
  }
  MCU_DCHECK_NE(status, EHttpStatusCode::kContinueDecoding);

  if (buffer_is_full && status == EHttpStatusCode::kNeedMoreInput &&
      start_size == buffer.size()) {
    MCU_VLOG(1) << MCU_FLASHSTR_128(
        "Need more input, but buffer is already full "
        "(has no room for additional input).");
    status = EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge;
  }
  if (status >= EHttpStatusCode::kHttpOk) {
    decode_function = nullptr;
    decoder_status = RequestDecoderStatus::kDecoded;
  }
  MCU_VLOG(1) << MCU_PSD("DecodeBuffer --> ") << status;
  return status;
}

// Decoding the start line, header lines, or end of header line. We don't know
// how many bytes are supposed to be in the header, so we rely on
// DecodeHeaderLines to find the end.
EHttpStatusCode RequestDecoderState::DecodeMessageHeader(
    mcucore::StringView& buffer) {
  MCU_VLOG(1) << MCU_PSD("DecodeMessageHeader ") << mcucore::HexEscaped(buffer);

  EHttpStatusCode status;
  do {
#ifdef REQUEST_DECODER_EXTRA_CHECKS
    const auto buffer_size_before_decode = buffer.size();
    const auto old_decode_function = decode_function;
    MCU_VLOG(2) << decode_function << ' ' << mcucore::HexEscaped(buffer)
                << MCU_PSD(" (") << static_cast<size_t>(buffer.size())
                << MCU_PSD(" chars))");
#endif

    status = decode_function(*this, buffer);

#ifdef REQUEST_DECODER_EXTRA_CHECKS
    MCU_CHECK_LE(buffer.size(), buffer_size_before_decode);
    auto consumed_chars = buffer_size_before_decode - buffer.size();

    MCU_VLOG(3) << MCU_PSD("decode_function ") << MCU_PSD("returned ") << status
                << MCU_PSD(", consumed ") << consumed_chars
                << MCU_PSD(" characters, ") << MCU_PSD("decode_function ")
                << (old_decode_function == decode_function
                        ? MCU_FLASHSTR("unchanged")
                        : MCU_FLASHSTR("changed"));

    if (status == EHttpStatusCode::kContinueDecoding) {
      // This is a check on the currently expected behavior; none of the
      // current decode functions represents a loop all by itself, which isn't
      // handled inside the decode function; i.e. none of them remove some,
      // but not all, of the input from buffer, and then return
      // kContinueDecoding without also calling SetDecodeFunction to specify
      // the next (different) function to handle the decoding.
      MCU_CHECK_NE(old_decode_function, decode_function)
          << MCU_PSD("Should have changed the decode function");  // COV_NF_LINE
    }
#endif
  } while (status == EHttpStatusCode::kContinueDecoding);

  if (status == EHttpStatusCode::kNeedMoreInput) {
    if (!is_decoding_header) {
      // We've just finished the message header, and this is a request with a
      // body.
      return DecodeMessageBody(buffer);
    }
  }

  return status;
}

// Decode the body of a PUT request where a Content-Length header was provided
// (i.e. remaining_content_length tells us how many ASCII characters remain).
EHttpStatusCode RequestDecoderState::DecodeMessageBody(
    mcucore::StringView& buffer) {
  MCU_VLOG(1) << MCU_PSD("DecodeMessageBody ") << mcucore::HexEscaped(buffer);
  MCU_CHECK(found_content_length);
  MCU_CHECK_EQ(request.http_method, EHttpMethod::PUT);

  if (buffer.size() > remaining_content_length) {
    // We assume that the HTTP client has not sent pipelined requests.
    MCU_VLOG(2) << MCU_PSD(
                       "There is more input than Content-Length indicated: ")
                << buffer.size() << MCU_PSD(" > ") << remaining_content_length;
    return EHttpStatusCode::kHttpPayloadTooLarge;
  } else if (buffer.size() == remaining_content_length) {
    is_final_input = true;
  } else {
    // buffer.size() < remaining_content_length
    if (is_final_input) {
      // The available input is not sufficient to reach Content-Length.
      //
      // TO DO Consider adding a "const char*" http_reason_phrase field to
      // Request, and setting it when we have something to say about why the
      // message failed. This could also be used in the ErrorMessage field of
      // the JSON response body.
      return EHttpStatusCode::kHttpBadRequest;
    }
  }

  EHttpStatusCode status;
  do {
    const auto buffer_size_before_decode = buffer.size();
#ifdef REQUEST_DECODER_EXTRA_CHECKS
    const auto old_decode_function = decode_function;
    MCU_VLOG(2) << decode_function << ' ' << mcucore::HexEscaped(buffer)
                << MCU_PSD(" (") << (buffer.size() + 0) << MCU_PSD(" chars))");
#endif

    status = decode_function(*this, buffer);
    const auto consumed_chars = buffer_size_before_decode - buffer.size();

#ifdef REQUEST_DECODER_EXTRA_CHECKS
    MCU_VLOG(3) << MCU_PSD("decode_function ") << MCU_PSD("returned ") << status
                << MCU_PSD(", consumed ") << consumed_chars
                << MCU_PSD(" characters, ") << MCU_PSD("decode_function ")
                << (old_decode_function == decode_function
                        ? MCU_FLASHSTR("unchanged")
                        : MCU_FLASHSTR("changed"));
    MCU_CHECK_LE(buffer.size(), buffer_size_before_decode);
    MCU_CHECK_LE(consumed_chars, remaining_content_length);
    if (decode_function == old_decode_function) {
      // This is a check on the currently expected behavior; none of the
      // current decode functions represents a loop all by itself, which isn't
      // handled inside the decode function; i.e. none of them remove some,
      // but not all, of the input from buffer, and then return
      // kContinueDecoding without also calling SetDecodeFunction to specify
      // the next (different) function to handle the decoding.
      MCU_CHECK_NE(status, EHttpStatusCode::kContinueDecoding);
    }
    if (buffer_size_before_decode == 0) {
      // We don't bother checking whether the buffer is empty at the start or
      // end of the loop because it saves a little bit of code space, in
      // exchange for another pass through the loop, thus requiring a
      // DecodeFunction to notice that there isn't enough input for it to
      // succeed.
      MCU_CHECK_NE(status, EHttpStatusCode::kContinueDecoding);
    }
#endif

    remaining_content_length -= consumed_chars;
  } while (status == EHttpStatusCode::kContinueDecoding);

  MCU_CHECK_NE(status, EHttpStatusCode::kContinueDecoding);

  if (status >= EHttpStatusCode::kHttpOk) {
#ifdef REQUEST_DECODER_EXTRA_CHECKS
    if (status == EHttpStatusCode::kHttpOk) {
      MCU_CHECK_EQ(remaining_content_length, 0);
    }
#endif
    return status;
  }

  MCU_CHECK_EQ(status, EHttpStatusCode::kNeedMoreInput);
  MCU_CHECK_GT(remaining_content_length, 0);
  return status;
}

EHttpStatusCode RequestDecoderState::SetDecodeFunction(
    const DecodeFunction func) {
  MCU_VLOG(3) << MCU_PSD("SetDecodeFunction(") << func << ')';
  MCU_CHECK_NE(decode_function, nullptr);
  MCU_CHECK_NE(decode_function, func);
  decode_function = func;
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderState::SetDecodeFunctionAfterListenerCall(
    DecodeFunction func, EHttpStatusCode status) {
  MCU_DCHECK_NE(status, EHttpStatusCode::kNeedMoreInput);
  if (status == EHttpStatusCode::kContinueDecoding) {
    return SetDecodeFunction(func);
  } else if (status < EHttpStatusCode::kHttpOk) {
    return EHttpStatusCode::kHttpInternalServerError;  // COV_NF_LINE
  } else {
    return status;
  }
}

}  // namespace alpaca
