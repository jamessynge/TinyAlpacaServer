#include "alpaca-decoder/request_decoder.h"

#include <cstddef>
#include <ios>

#include "absl/flags/flag.h"
#include "absl/strings/escaping.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "alpaca-decoder/constants.h"
#include "alpaca-decoder/request.h"
#include "alpaca-decoder/request_decoder_listener.h"
#include "alpaca-decoder/string_view.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

ABSL_DECLARE_FLAG(int, v);

namespace alpaca {
namespace {
constexpr const size_t kDecodeBufferSize = 40;

using ::testing::EndsWith;
using ::testing::InSequence;
using ::testing::IsEmpty;
using ::testing::Mock;
using ::testing::Return;
using ::testing::StartsWith;
using ::testing::StrictMock;

std::vector<std::string> SplitEveryN(const std::string& full_request,
                                     const size_t n) {
  DVLOG(5) << "SplitEveryN " << n
           << ", full_request.size: " << full_request.size();
  CHECK_LE(n, StringView::kMaxSize);
  std::vector<std::string> partition;
  for (size_t pos = 0; pos < full_request.size(); pos += n) {
    DVLOG(6) << "pos: " << pos << ", pos+n: " << pos + n;
    partition.push_back(full_request.substr(pos, n));
    DVLOG(6) << "part: \"" << absl::CHexEscape(partition.back()) << "\"";
  }
  return partition;
}

std::vector<std::vector<std::string>> GenerateMultipleRequestPartitions(
    const std::string& full_request) {
  DLOG(INFO) << "GenerateMultipleRequestPartitions; full_request (size="
             << full_request.size() << "):\n"
             << full_request;
  std::vector<std::vector<std::string>> partitions;
  size_t n =
      std::min(static_cast<size_t>(StringView::kMaxSize), full_request.size());
  bool first = true;
  do {
    auto partition = SplitEveryN(full_request, n);
    partitions.push_back(partition);
    if (first) {
      // Start with an empty string.
      partition.insert(partition.begin(), "");
      first = false;
    }
  } while (--n > 0);
  return partitions;
}

std::string AppendRemainder(const std::string& buffer,
                            const std::vector<std::string>& partition,
                            int ndx) {
  std::string result = buffer;
  for (; ndx < partition.size(); ++ndx) {
    result += partition[ndx];
  }
  return result;
}

bool TestHasFailed() {
  auto test_info = testing::UnitTest::GetInstance()->current_test_info();
  return test_info->result()->Failed();
}

// Decode the contents of buffer until the decoder needs more input or returns
// an error.
EDecodeStatus DecodeBuffer(
    RequestDecoder& decoder, std::string& buffer, const bool at_end,
    const size_t max_decode_buffer_size = kDecodeBufferSize) {
  CHECK_GT(max_decode_buffer_size, 0);
  CHECK_LE(max_decode_buffer_size, StringView::kMaxSize);

  while (true) {
    // We deliberately copy into another string, and may append some "garbage",
    // to check for cases where the decoder tries to read too far.
    const size_t initial_size = std::min(max_decode_buffer_size, buffer.size());
    std::string copy = buffer.substr(0, initial_size);
#ifndef NDEBUG
    copy.append("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
#endif
    StringView view(copy.data(), initial_size);

    const bool was_empty = buffer.empty();
    const bool now_at_end = at_end && initial_size == buffer.size();
    auto status = decoder.DecodeBuffer(view, now_at_end);

    // Make sure that the decoder only removed the prefix of the view.
    EXPECT_GE(initial_size, view.size());
    const size_t removed_size = initial_size - view.size();
    EXPECT_EQ(copy.data() + removed_size, view.data());

    // Make sure that the decoder didn't modify the passed in buffer.
    EXPECT_EQ(buffer.substr(0, initial_size), copy.substr(0, initial_size));

    // Remove the decoded prefix of buffer.
    buffer.erase(0, initial_size - view.size());

    if (status != EDecodeStatus::kNeedMoreInput) {
      EXPECT_GE(status, EDecodeStatus::kHttpOk);
      return status;
    } else if (was_empty) {
      return status;
    } else if (removed_size == 0) {
      return status;
    }
  }
}

EDecodeStatus ResetAndDecodeFullBuffer(
    RequestDecoder& decoder, std::string& buffer,
    const size_t max_decode_buffer_size = kDecodeBufferSize) {
  decoder.Reset();
  return DecodeBuffer(decoder, buffer, true, max_decode_buffer_size);
}

// Apply the decoder to decoding the provided partition of a request. Returns
// the final decode status, the remainder of the last buffer passed in, and
// all the remaining undecoded text.
std::tuple<EDecodeStatus, std::string, std::string> DecodePartitionedRequest(
    RequestDecoder& decoder, const std::vector<std::string>& partition,
    const size_t max_decode_buffer_size = kDecodeBufferSize) {
  CHECK_NE(partition.size(), 0);
  CHECK_GT(max_decode_buffer_size, 0);
  CHECK_LE(max_decode_buffer_size, StringView::kMaxSize);
  decoder.Reset();
  std::string buffer;
  for (int ndx = 0; ndx < partition.size(); ++ndx) {
    const bool at_end = (ndx + 1) == partition.size();
    buffer += partition[ndx];
    auto status = DecodeBuffer(decoder, buffer, at_end, max_decode_buffer_size);
    if (status != EDecodeStatus::kNeedMoreInput) {
      return {status, buffer, AppendRemainder(buffer, partition, ndx + 1)};
    }
  }
  return {EDecodeStatus::kNeedMoreInput, buffer, buffer};
}

TEST(RequestDecoderTest, Splitter) {
  SplitEveryN("01234567890123456789012345678901234567890123456", 23);
}

TEST(RequestDecoderTest, Unused) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);
}

TEST(RequestDecoderTest, ResetOnly) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);
  decoder.Reset();
}

TEST(RequestDecoderTest, ResetRequired) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string full_request(
      "GET /api/v1/safetymonitor/0/issafe HTTP/1.1\r\n"
      "\r\n");
  auto buffer = full_request;

  EXPECT_EQ(DecodeBuffer(decoder, buffer, /*at_end=*/true),
            EDecodeStatus::kInternalError);
  EXPECT_EQ(buffer, full_request);  // No input has been consumed.
}

TEST(RequestDecoderTest, SmallestGetRequest) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string full_request(
      "GET /api/v1/safetymonitor/0/issafe HTTP/1.1\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    auto result = DecodePartitionedRequest(decoder, partition);

    const EDecodeStatus status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    ASSERT_EQ(status, EDecodeStatus::kHttpOk);
    ASSERT_THAT(buffer, IsEmpty());
    ASSERT_THAT(remainder, IsEmpty());
    ASSERT_EQ(alpaca_request.http_method, EHttpMethod::GET);
    ASSERT_EQ(alpaca_request.device_type, EDeviceType::kSafetyMonitor);
    ASSERT_EQ(alpaca_request.device_number, 0);
    ASSERT_EQ(alpaca_request.ascom_method, EMethod::kIsSafe);
    ASSERT_FALSE(alpaca_request.found_client_id);
    ASSERT_FALSE(alpaca_request.found_client_transaction_id);
  }
}

TEST(RequestDecoderTest, SmallestPutRequest) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string full_request(
      "PUT /api/v1/observingconditions/0/refresh"
      "?ClientId=123&ClientTransactionId=432 "
      "HTTP/1.1\r\n"
      "Content-Length: 0\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    auto result = DecodePartitionedRequest(decoder, partition);

    const EDecodeStatus status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    ASSERT_EQ(status, EDecodeStatus::kHttpOk);
    ASSERT_THAT(buffer, IsEmpty());
    ASSERT_THAT(remainder, IsEmpty());
    ASSERT_EQ(alpaca_request.http_method, EHttpMethod::PUT);
    ASSERT_EQ(alpaca_request.device_type, EDeviceType::kObservingConditions);
    ASSERT_EQ(alpaca_request.device_number, 0);
    ASSERT_EQ(alpaca_request.ascom_method, EMethod::kRefresh);
    ASSERT_TRUE(alpaca_request.found_client_id);
    ASSERT_TRUE(alpaca_request.found_client_transaction_id);
    ASSERT_EQ(alpaca_request.client_id, 123);
    ASSERT_EQ(alpaca_request.client_transaction_id, 432);
  }
}

TEST(RequestDecoderTest, AllSupportedFeatures) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string body = "a=1&ClienttransACTIONid=9";
  const std::string full_request = absl::StrCat(
      "PUT /api/v1/safetymonitor/9999/connected?ClientId=321&AbC=xYz "
      "HTTP/1.1\r\n",
      "Host:example.com    \r\n",      // Optional whitespace after value.
      "Connection:   keep-alive\r\n",  // Optional whitespace before value.
      "Another-Header:Some Text, e.g. foo@example.com!\r\n",
      "accept: application/json \r\n",  // Whitespace on both sides.
      "content-TYPE:application/x-www-form-urlencoded\r\n",
      "Content-Length:", body.size(), "\r\n",  // Last header line.
      "\r\n",  // End of headers, separator between message header and body.
      body);

  LOG(INFO) << "full_request:\n" << full_request << "\n";

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    auto result = DecodePartitionedRequest(decoder, partition);
    const EDecodeStatus status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    ASSERT_EQ(status, EDecodeStatus::kHttpOk);
    ASSERT_THAT(buffer, IsEmpty());
    ASSERT_THAT(remainder, IsEmpty());
    ASSERT_EQ(alpaca_request.http_method, EHttpMethod::PUT);
    ASSERT_EQ(alpaca_request.device_type, EDeviceType::kSafetyMonitor);
    ASSERT_EQ(alpaca_request.device_number, 9999);
    ASSERT_EQ(alpaca_request.ascom_method, EMethod::kConnected);
    ASSERT_TRUE(alpaca_request.found_client_id);
    ASSERT_TRUE(alpaca_request.found_client_transaction_id);
    ASSERT_EQ(alpaca_request.client_id, 321);
    ASSERT_EQ(alpaca_request.client_transaction_id, 9);
  }
}

TEST(RequestDecoderTest, ParamSeparatorAtEndOfBody) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  std::string request =
      "PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "Content-Length: 13\r\n"
      "\r\n"
      "ClientId=876&";
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request), EDecodeStatus::kHttpOk);
  ASSERT_TRUE(alpaca_request.found_client_id);
  ASSERT_EQ(alpaca_request.client_id, 876);
  ASSERT_FALSE(alpaca_request.found_client_transaction_id);
}

TEST(RequestDecoderTest, DetectsOutOfRangeDeviceType) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);
  decoder.Reset();

  std::string full_request(
      "GET /api/v1/safetymonitor/4294967300/issafe "
      "HTTP/1.1\r\n\r\n");

  alpaca_request.client_id = kResetClientId;
  EXPECT_EQ(DecodeBuffer(decoder, full_request, true, kDecodeBufferSize),
            EDecodeStatus::kHttpBadRequest);
  EXPECT_EQ(full_request, "4294967300/issafe HTTP/1.1\r\n\r\n");
  EXPECT_EQ(alpaca_request.client_id,
            kResetClientId);  // Hasn't been overwritten.
}

TEST(RequestDecoderTest, DetectsOutOfRangeClientId) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);
  decoder.Reset();

  std::string full_request(
      "GET /api/v1/safetymonitor/0000004294967295/issafe?ClientId=4294967296 "
      "HTTP/1.1\r\n\r\n");

  alpaca_request.client_id = kResetClientId;
  EXPECT_EQ(DecodeBuffer(decoder, full_request, true, kDecodeBufferSize),
            EDecodeStatus::kHttpBadRequest);
  EXPECT_EQ(alpaca_request.device_number, 4294967295UL);
  EXPECT_FALSE(alpaca_request.found_client_id);
  EXPECT_EQ(alpaca_request.client_id,
            kResetClientId);  // Hasn't been overwritten.
}

TEST(RequestDecoderTest, DetectsOutOfRangeClientTransactionId) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  // Initially OK
  std::string body = "ClientTransactionId=444444444&ClientId=1";
  std::string request =
      absl::StrCat("PUT /api/v1/safetymonitor/7/connected HTTP/1.1\r\n",
                   "Content-Length:", body.size(), "\r\n\r\n", body);

  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request), EDecodeStatus::kHttpOk);
  EXPECT_EQ(alpaca_request.device_number, 7);
  EXPECT_TRUE(alpaca_request.found_client_transaction_id);
  EXPECT_EQ(alpaca_request.client_transaction_id, 444444444);
  EXPECT_TRUE(alpaca_request.found_client_id);
  EXPECT_EQ(alpaca_request.client_id, 1);

  // Append another digit, now too big to fit in a uint32_t. This will prevent
  // ClientId from ever being examined.
  body = "ClientTransactionId=4444444444&ClientId=1";
  request = absl::StrCat("PUT /api/v1/safetymonitor/7/connected HTTP/1.1\r\n",
                         "Content-Length:", body.size(), "\r\n\r\n", body);

  alpaca_request.client_id = kResetClientId;
  alpaca_request.client_transaction_id = kResetClientTransactionId;
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpBadRequest);
  EXPECT_EQ(alpaca_request.device_number, 7);
  EXPECT_FALSE(alpaca_request.found_client_id);
  EXPECT_FALSE(alpaca_request.found_client_transaction_id);
  // Confirm that neither client id hasn't been overwritten.
  EXPECT_EQ(alpaca_request.client_id, kResetClientId);
  EXPECT_EQ(alpaca_request.client_transaction_id, kResetClientTransactionId);
}

// The decoder can only read short (<=255 chars) bodies... else we'd have to
// have a bigger var for tracking how far through twe are OR we'd have to ignore
// the length and simply decode until there was no more input.
TEST(RequestDecoderTest, DetectsOutOfRangeContentLength) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  // Start with a missing Content-Length.
  std::string request =
      "PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "\r\n";
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpLengthRequired);
  EXPECT_EQ(alpaca_request.device_number, 1);
  EXPECT_EQ(alpaca_request.ascom_method, EMethod::kIsSafe);

  // Now provide a size that is too large.
  request =
      "PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "CONTENT-LENGTH: 256\r\n"
      "\r\n";
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpPayloadTooLarge);
  EXPECT_EQ(alpaca_request.device_number, 1);

  // But that size will be ignored for a GET request because we don't need to
  // decode it.
  request =
      "GET /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "CONTENT-LENGTH: 256\r\n"
      "\r\n";
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request), EDecodeStatus::kHttpOk);
  EXPECT_EQ(alpaca_request.device_number, 1);

  // A 255 char length can be decoded. Need to make a body of that size which is
  // valid.
  std::string frag39 = "nineteen_characters=nineteen_characters";
  std::string frag239 =
      absl::StrJoin({frag39, frag39, frag39, frag39, frag39, frag39}, "&");
  ASSERT_EQ(frag239.size(), 239);
  std::string body = frag239 + "&a=0124567890123";
  ASSERT_EQ(body.size(), 255);
  request = absl::StrCat("PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",
                         "CONTENT-LENGTH: 255\r\n", "\r\n", body);
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request), EDecodeStatus::kHttpOk);
  EXPECT_EQ(alpaca_request.device_number, 1);
}

TEST(RequestDecoderTest, DetectsPayloadTruncated) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  // Body is missing. There doesn't appear to be a better response code than
  // 400 for missing data.
  std::string request =
      "PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "Content-Length: 1\r\n"
      "\r\n";
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpBadRequest);

  // "=value" is missing after a parameter name.
  request =
      "PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "Content-Length: 10\r\n"
      "\r\n"
      "param_name";
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpBadRequest);
}

TEST(RequestDecoderTest, RejectsUnsupportedHttpMethod) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string request_after_method =
      " /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "Content-Length: 0\r\n"
      "\r\n";

  for (std::string method :
       {"CONNECT", "DELETE", "OPTIONS", "PATCH", "POST", "TRACE"}) {
    const std::string full_request = method + request_after_method;
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
              EDecodeStatus::kHttpMethodNotImplemented);
    EXPECT_EQ(request, request_after_method);
  }
}

TEST(RequestDecoderTest, RejectsInvalidPathStart) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string request_after_path_start =
      "safetymonitor/1/issafe HTTP/1.1\r\n"
      "Content-Length: 0\r\n"
      "\r\n";

  for (std::string bogus_path_start :
       {" ", "*", " *", "/", " //api/v1/", " /api/v2/", " /API/v1/"}) {
    const std::string full_request =
        "GET" + bogus_path_start + request_after_path_start;
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
              EDecodeStatus::kHttpBadRequest);
    EXPECT_THAT(full_request, EndsWith(request));
    EXPECT_THAT(request, EndsWith(request_after_path_start));
  }
}

/*
TEST(RequestDecoderTest, NoQueryRequestWithBody) {
  const std::string full_request("GET /some/path HTTP/1.1\r\n\r\n\r\nA
body."); StrictMock<MockRequestDecoderListener> listener; RequestDecoder
decoder(listener); for (auto partition :
GenerateMultipleRequestPartitions(full_request)) { InSequence s;
    EXPECT_CALL(listener, OnStartDecoding);
    EXPECT_CALL(listener, OnHttpMethod(EHttpMethod::GET));
    EXPECT_CALL(listener, OnPath(StringView("/some/path")));
    EXPECT_CALL(listener, OnHttpVersion(EHttpVersion::kHttp11));
    EXPECT_CALL(listener, OnEndDecoding);

    auto result = DecodePartitionedRequest(decoder, partition);
    const EDecodeStatus status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    ASSERT_EQ(status, EDecodeStatus::kDecodeDone);
    ASSERT_EQ(remainder, "\r\nA body.");
    ASSERT_THAT(remainder, StartsWith(buffer));
    ASSERT_TRUE(decoder.is_done());
    ASSERT_EQ(decoder.http_decode_status(), EDecodeStatus::kHttpOk);

    Mock::VerifyAndClearExpectations(&listener);
  }
}

TEST(RequestDecoderTest, PathWithShortestQueryParam) {
  const std::string full_request("PUT /?A=a HTTP/1.1\r\n\r\n\nBody");
  const std::string expected_remainder("\nBody");
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(listener);
  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    InSequence s;
    EXPECT_CALL(listener, OnStartDecoding);
    EXPECT_CALL(listener, OnHttpMethod(EHttpMethod::PUT));
    EXPECT_CALL(listener, OnPath(StringView("/")));
    EXPECT_CALL(listener, OnQueryParamName(StringView("A")));
    EXPECT_CALL(listener, OnQueryParamValue(StringView("a")));
    EXPECT_CALL(listener, OnHttpVersion(EHttpVersion::kHttp11));
    EXPECT_CALL(listener, OnEndDecoding);

    auto result = DecodePartitionedRequest(decoder, partition);
    const EDecodeStatus status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    ASSERT_EQ(status, EDecodeStatus::kDecodeDone);
    ASSERT_EQ(remainder, expected_remainder);
    ASSERT_THAT(expected_remainder, StartsWith(buffer));
    ASSERT_TRUE(decoder.is_done());
    ASSERT_EQ(decoder.http_decode_status(), EDecodeStatus::kHttpOk);

    Mock::VerifyAndClearExpectations(&listener);
  }
}

TEST(RequestDecoderTest, ErrorIfStartDecodingNotCalled) {
  const std::string full_request("GET /some/path?symbol HTTP/1.1\r\n\r\n");
  const std::string expected_remainder("symbol HTTP/1.1\r\n\r\n");
  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    StrictMock<MockRequestDecoderListener> listener;
    RequestDecoder decoder(listener);

    StringView view(partition.front());
    auto status = decoder.DecodeBuffer(view);

    ASSERT_EQ(status, EDecodeStatus::kDecodeError);
    ASSERT_TRUE(decoder.is_done());
    ASSERT_EQ(view, partition.front());
    ASSERT_EQ(decoder.http_decode_status(), EDecodeStatus::kNoRequestParsed);
  }
}

TEST(RequestDecoderTest, ErrorIfMissingQueryParamName) {
  const std::string full_request("GET /some/path? HTTP/1.1\r\n\r\n");
  const std::string expected_remainder(" HTTP/1.1\r\n\r\n");
  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    StrictMock<MockRequestDecoderListener> listener;
    RequestDecoder decoder(listener);

    {
      InSequence s;
      EXPECT_CALL(listener, OnStartDecoding);
      EXPECT_CALL(listener, OnHttpMethod(EHttpMethod::GET));
      EXPECT_CALL(listener, OnPath(StringView("/some/path")));
    }

    auto result = DecodePartitionedRequest(decoder, partition);
    const EDecodeStatus status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    ASSERT_EQ(status, EDecodeStatus::kDecodeError);
    ASSERT_EQ(remainder, expected_remainder);
    ASSERT_THAT(expected_remainder, StartsWith(buffer));
    ASSERT_TRUE(decoder.is_done());
    ASSERT_EQ(decoder.http_decode_status(), EDecodeStatus::kParamHasNoName);

    // If the caller fails to notice the kDecodeError, it will be returned
next
    // time too, whether we pass the same buffer, or the full remainder of the
    // request.
    StringView view(buffer);
    ASSERT_EQ(decoder.DecodeBuffer(view), EDecodeStatus::kDecodeError);
    ASSERT_EQ(view, buffer);
    ASSERT_TRUE(decoder.is_done());
    ASSERT_EQ(decoder.http_decode_status(), EDecodeStatus::kParamHasNoName);

    StringView view2(remainder);
    ASSERT_EQ(decoder.DecodeBuffer(view), EDecodeStatus::kDecodeError);
    ASSERT_EQ(view2, remainder);
    ASSERT_TRUE(decoder.is_done());
    ASSERT_EQ(decoder.http_decode_status(), EDecodeStatus::kParamHasNoName);
  }
}

TEST(RequestDecoderTest, ErrorIfMissingEqualAfterQueryParamName) {
  const std::string full_request("GET /some/path?symbol HTTP/1.1\r\n\r\n");
  const std::string expected_remainder("symbol HTTP/1.1\r\n\r\n");
  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    StrictMock<MockRequestDecoderListener> listener;
    RequestDecoder decoder(listener);

    {
      InSequence s;
      EXPECT_CALL(listener, OnStartDecoding);
      EXPECT_CALL(listener, OnHttpMethod(EHttpMethod::GET));
      EXPECT_CALL(listener, OnPath(StringView("/some/path")));
    }

    auto result = DecodePartitionedRequest(decoder, partition);
    const EDecodeStatus status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    ASSERT_EQ(status, EDecodeStatus::kDecodeError);
    ASSERT_EQ(remainder, expected_remainder);
    ASSERT_THAT(expected_remainder, StartsWith(remainder));
    ASSERT_TRUE(decoder.is_done());
    ASSERT_EQ(decoder.http_decode_status(), EDecodeStatus::kParamHasNoValue);

    // If the caller fails to notice the kDecodeError, it will be returned
next
    // time too, whether we pass the same buffer, or the full remainder of the
    // request. DecodePartitionedRequest does the former, so we test the
latter
    // here.
    StringView view(remainder);
    ASSERT_EQ(decoder.DecodeBuffer(view), EDecodeStatus::kDecodeError);
    ASSERT_EQ(view, remainder);
    ASSERT_TRUE(decoder.is_done());
    ASSERT_EQ(decoder.http_decode_status(), EDecodeStatus::kParamHasNoValue);
  }
}

void VerifyDetectsInvalidHeaderName(
    const std::string& bad_header_line, const std::string& expected_remainder,
    const EDecodeStatus expected_http_decode_status) {
  const std::string full_request =
      std::string("GET /some/path HTTP/1.1\r\n") + bad_header_line;
  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    StrictMock<MockRequestDecoderListener> listener;
    RequestDecoder decoder(listener);

    InSequence s;
    EXPECT_CALL(listener, OnStartDecoding);
    EXPECT_CALL(listener, OnHttpMethod(EHttpMethod::GET));
    EXPECT_CALL(listener, OnPath(StringView("/some/path")));
    EXPECT_CALL(listener, OnHttpVersion(EHttpVersion::kHttp11));

    auto result = DecodePartitionedRequest(decoder, partition);
    const EDecodeStatus status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    ASSERT_EQ(status, EDecodeStatus::kDecodeError);
    ASSERT_EQ(remainder, expected_remainder);
    ASSERT_THAT(expected_remainder, StartsWith(buffer));
    ASSERT_TRUE(decoder.is_done());
    ASSERT_EQ(decoder.http_decode_status(), expected_http_decode_status);
  }
}

TEST(RequestDecoderTest, ErrorIfInvalidHeaderName) {
  for (const std::string header_line :
       {":Value", "?Name", "\rName", "\nName", " Name", "\tName", "@Name"}) {
    VerifyDetectsInvalidHeaderName(header_line, header_line,
                                   EDecodeStatus::kHeaderHasNoName);
  }
}

TEST(RequestDecoderTest, ErrorIfInvalidHeaderSeparator) {
  for (const std::string header_line :
       {"Name :", "Name?:", "Name\r:", "Name\n:", "Name\377", "Name\"",
        "Name@"}) {
    VerifyDetectsInvalidHeaderName(header_line, header_line,
                                   EDecodeStatus::kHeaderSeparatorError);
  }
}

TEST(RequestDecoderTest, ErrorIfMissingMalformedHeaderName) {
  const std::string full_request(
      "GET /some/path HTTP/1.1\r\n"
      ": Header-Value\r\n");
  const std::string expected_remainder(": Header-Value\r\n");
  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    StrictMock<MockRequestDecoderListener> listener;
    RequestDecoder decoder(listener);

    {
      InSequence s;
      EXPECT_CALL(listener, OnStartDecoding);
      EXPECT_CALL(listener, OnHttpMethod(EHttpMethod::GET));
      EXPECT_CALL(listener, OnPath(StringView("/some/path")));
      EXPECT_CALL(listener, OnHttpVersion(EHttpVersion::kHttp11));
    }

    auto result = DecodePartitionedRequest(decoder, partition);
    const EDecodeStatus status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    ASSERT_EQ(status, EDecodeStatus::kDecodeError);
    ASSERT_EQ(remainder, expected_remainder);
    ASSERT_THAT(expected_remainder, StartsWith(buffer));
    ASSERT_TRUE(decoder.is_done());
    ASSERT_EQ(decoder.http_decode_status(), EDecodeStatus::kHeaderHasNoName);
  }
}

TEST(RequestDecoderTest, MalformedHeaderValue) {
  const std::string full_request(
      "GET /some/path HTTP/1.1\r\n"
      ": Header-Value\r\n");
  const std::string expected_remainder(": Header-Value\r\n");
  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    StrictMock<MockRequestDecoderListener> listener;
    RequestDecoder decoder(listener);

    {
      InSequence s;
      EXPECT_CALL(listener, OnStartDecoding);
      EXPECT_CALL(listener, OnHttpMethod(EHttpMethod::GET));
      EXPECT_CALL(listener, OnPath(StringView("/some/path")));
      EXPECT_CALL(listener, OnHttpVersion(EHttpVersion::kHttp11));
    }

    auto result = DecodePartitionedRequest(decoder, partition);
    const EDecodeStatus status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    ASSERT_EQ(status, EDecodeStatus::kDecodeError);
    ASSERT_EQ(remainder, expected_remainder);
    ASSERT_THAT(expected_remainder, StartsWith(buffer));
    ASSERT_TRUE(decoder.is_done());
    ASSERT_EQ(decoder.http_decode_status(), EDecodeStatus::kHeaderHasNoName);
  }
}
*/

// Ideally we'd have a way to divert the logs elsewhere for this test so they
// don't swamp the log file.
TEST(RequestDecoderTest, VerboseLogging) {
  absl::SetFlag(&FLAGS_v, 10);

  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string body = "a=1&ClienttransACTIONid=9";
  const std::string full_request = absl::StrCat(
      "PUT /api/v1/safetymonitor/9999/connected?ClientId=321&AbC=xYz "
      "HTTP/1.1\r\n",
      "Host:example.com    \r\n",      // Optional whitespace after value.
      "Connection:   keep-alive\r\n",  // Optional whitespace before value.
      "Another-Header:Some Text, e.g. foo@example.com!\r\n",
      "accept: application/json \r\n",  // Whitespace on both sides.
      "content-TYPE:application/x-www-form-urlencoded\r\n",
      "Content-Length:", body.size(), "\r\n",  // Last header line.
      "\r\n",  // End of headers, separator between message header and body.
      body);

  LOG(INFO) << "full_request:\n" << full_request << "\n";

  // Consider using std::shuffle to pick a diverse set of partitions to decode
  // with lots of logging enabled.

  int count = 0;
  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    if (count++ % 5) {
      continue;
    }

    auto result = DecodePartitionedRequest(decoder, partition);
    const EDecodeStatus status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    ASSERT_EQ(status, EDecodeStatus::kHttpOk);
    ASSERT_THAT(buffer, IsEmpty());
    ASSERT_THAT(remainder, IsEmpty());
    ASSERT_EQ(alpaca_request.http_method, EHttpMethod::PUT);
    ASSERT_EQ(alpaca_request.device_type, EDeviceType::kSafetyMonitor);
    ASSERT_EQ(alpaca_request.device_number, 9999);
    ASSERT_EQ(alpaca_request.ascom_method, EMethod::kConnected);
    ASSERT_TRUE(alpaca_request.found_client_id);
    ASSERT_TRUE(alpaca_request.found_client_transaction_id);
    ASSERT_EQ(alpaca_request.client_id, 321);
    ASSERT_EQ(alpaca_request.client_transaction_id, 9);
  }
}

}  // namespace
}  // namespace alpaca
