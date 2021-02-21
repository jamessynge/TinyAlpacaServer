#include "request_decoder.h"

// Tests of RequestDecoder, AlpacaRequest and RequestDecoderListener.
//
// Author: james.synge@gmail.com

#include <algorithm>
#include <cstddef>
#include <string>
#include <tuple>
#include <vector>

#include "absl/flags/declare.h"
#include "absl/flags/flag.h"
#include "absl/strings/escaping.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "alpaca_request.h"
#include "decoder_constants.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "logging.h"
#include "request_decoder_listener.h"
#include "string_view.h"
#include "tests/request_decoder_listener_mock.h"

ABSL_DECLARE_FLAG(int, v);

namespace alpaca {
namespace {
constexpr const size_t kDecodeBufferSize = 40;

using ::testing::AnyNumber;
using ::testing::EndsWith;
using ::testing::Eq;
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
#if TAS_ENABLE_DEBUGGING
    copy.append("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
#endif
    StringView view(copy.data(), initial_size);

    const bool was_empty = buffer.empty();
    const bool now_at_end = at_end && initial_size == buffer.size();
    const bool buffer_is_full = view.size() >= max_decode_buffer_size;
    auto status = decoder.DecodeBuffer(view, buffer_is_full, now_at_end);

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

// This was used for debugging an error in the test fixture.
// TEST(RequestDecoderTest, Splitter) {
//   SplitEveryN("01234567890123456789012345678901234567890123456", 23);
// }

TEST(RequestDecoderTest, LogSizes) {
  LOG(INFO) << "sizeof(AlpacaRequest) " << sizeof(AlpacaRequest);
  LOG(INFO) << "sizeof(RequestDecoderListener) "
            << sizeof(RequestDecoderListener);
  LOG(INFO) << "sizeof(RequestDecoder) " << sizeof(RequestDecoder);
}

TEST(RequestDecoderTest, Unused) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);
}

TEST(RequestDecoderTest, ResetOnly) {
  AlpacaRequest alpaca_request;
  StrictMock<RequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);
  decoder.Reset();
}

TEST(RequestDecoderTest, ResetRequired) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string full_request(
      "GET /api/v1/safetymonitor/0/issafe HTTP/1.1\r\n"
      "\r\n");
  auto buffer = full_request;

  EXPECT_EQ(DecodeBuffer(decoder, buffer, /*at_end=*/true),
            EDecodeStatus::kHttpInternalServerError);
  EXPECT_EQ(buffer, full_request);  // No input has been consumed.
}

TEST(RequestDecoderTest, SmallestGetRequest) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
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
  StrictMock<MockRequestDecoderListener> listener;
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
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string body = "a=1&connected=abc&&ClienttransACTIONid=9";
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
    InSequence s;
    EXPECT_CALL(listener, OnUnknownParameterName(Eq("AbC")));
    EXPECT_CALL(listener, OnUnknownParameterValue(Eq("xYz")));
    EXPECT_CALL(listener, OnUnknownHeaderName(Eq("Host")));
    EXPECT_CALL(listener, OnUnknownHeaderValue(Eq("example.com")));
    EXPECT_CALL(listener, OnUnknownHeaderName(Eq("Connection")));
    EXPECT_CALL(listener, OnUnknownHeaderValue(Eq("keep-alive")));
    EXPECT_CALL(listener, OnUnknownHeaderName(Eq("Another-Header")));
    EXPECT_CALL(listener,
                OnUnknownHeaderValue(Eq("Some Text, e.g. foo@example.com!")));
    EXPECT_CALL(listener, OnUnknownParameterName(Eq("a")));
    EXPECT_CALL(listener, OnUnknownParameterValue(Eq("1")));
    EXPECT_CALL(listener, OnExtraParameter(EParameter::kConnected, Eq("abc")));

    auto result = DecodePartitionedRequest(decoder, partition);

    Mock::VerifyAndClearExpectations(&listener);

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

TEST(RequestDecoderTest, ParamSeparatorsAtEndOfBody) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  std::string body = "ClientId=876&&&&&&&&&";

  std::string request =
      absl::StrCat("PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",
                   "Content-Length: ", body.size(), "\r\n", "\r\n", body);
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request), EDecodeStatus::kHttpOk);
  ASSERT_TRUE(alpaca_request.found_client_id);
  ASSERT_EQ(alpaca_request.client_id, 876);
  ASSERT_FALSE(alpaca_request.found_client_transaction_id);

  // Extra spaces at the end, not acceptable.
  body = "ClientId=654&&&&&&&&&   ";
  request = absl::StrCat("PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",
                         "Content-Length: ", body.size(), "\r\n", "\r\n", body);
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpBadRequest);
  ASSERT_TRUE(alpaca_request.found_client_id);
  ASSERT_EQ(alpaca_request.client_id, 654);
  ASSERT_FALSE(alpaca_request.found_client_transaction_id);
}

TEST(RequestDecoderTest, DetectsOutOfRangeDeviceNumber) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);
  decoder.Reset();

  std::string full_request(
      "GET /api/v1/safetymonitor/4294967300/issafe "
      "HTTP/1.1\r\n\r\n");

  alpaca_request.client_id = kResetClientId;
  EXPECT_EQ(DecodeBuffer(decoder, full_request, true, kDecodeBufferSize),
            EDecodeStatus::kHttpNotFound);
  EXPECT_EQ(alpaca_request.client_id,
            kResetClientId);  // Hasn't been overwritten.
  // It isn't important how much of the request has been processed, however we
  // don't otherwise have a great way to confirm that the reason for the failure
  // was the device number, vs. the ASCOM method name.
  EXPECT_THAT(full_request, EndsWith("issafe HTTP/1.1\r\n\r\n"));
}

TEST(RequestDecoderTest, DetectsOutOfRangeClientId) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);
  decoder.Reset();

  std::string full_request(
      "GET /api/v1/safetymonitor/0000004294967295/issafe?ClientId=4294967296 "
      "HTTP/1.1\r\n\r\n");

  EXPECT_CALL(listener,
              OnExtraParameter(EParameter::kClientId, Eq("4294967296")));
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
  StrictMock<MockRequestDecoderListener> listener;
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
  EXPECT_CALL(listener, OnExtraParameter(EParameter::kClientTransactionId,
                                         Eq("4444444444")));
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
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  // Start with a missing Content-Length.
  std::string request =
      "PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "\r\n";
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpLengthRequired);
  EXPECT_EQ(alpaca_request.device_number, 1);
  EXPECT_EQ(alpaca_request.ascom_method, EMethod::kIsSafe);

  // Now a non-integer Content-Length.
  request =
      "PUT /api/v1/safetymonitor/2/issafe HTTP/1.1\r\n"
      "Content-Length: .0\r\n"
      "\r\n";
  EXPECT_CALL(listener,
              OnExtraHeader(EHttpHeader::kHttpContentLength, Eq(".0")));
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpBadRequest);
  EXPECT_EQ(alpaca_request.device_number, 2);
  EXPECT_EQ(alpaca_request.ascom_method, EMethod::kIsSafe);

  // Now provide a size that is too large.
  request =
      "PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "CONTENT-LENGTH: 256\r\n"
      "\r\n";
  EXPECT_CALL(listener,
              OnExtraHeader(EHttpHeader::kHttpContentLength, Eq("256")));
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpPayloadTooLarge);
  Mock::VerifyAndClearExpectations(&listener);
  EXPECT_EQ(alpaca_request.device_number, 1);

  // But that size will not be a problem for a GET request because we don't need
  // to decode a payload.
  request =
      "GET /api/v1/safetymonitor/3/issafe HTTP/1.1\r\n"
      "CONTENT-LENGTH: 256\r\n"
      "\r\n";
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request), EDecodeStatus::kHttpOk);
  EXPECT_EQ(alpaca_request.device_number, 3);

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
  EXPECT_CALL(listener, OnUnknownParameterName(Eq("nineteen_characters")))
      .Times(AnyNumber());
  EXPECT_CALL(listener, OnUnknownParameterValue(Eq("nineteen_characters")))
      .Times(AnyNumber());
  EXPECT_CALL(listener, OnUnknownParameterName(Eq("a")));
  EXPECT_CALL(listener, OnUnknownParameterValue(Eq("0124567890123")));
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request), EDecodeStatus::kHttpOk);
  EXPECT_EQ(alpaca_request.device_number, 1);
}

TEST(RequestDecoderTest, DetectsPayloadTruncated) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
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

TEST(RequestDecoderTest, DetectsPayloadTooLong) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  std::string request =
      "PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "Content-Length: 1\r\n"
      "\r\n"
      "12";
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpPayloadTooLarge);
}

TEST(RequestDecoderTest, DetectsParameterValueIsTooLong) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  for (int max_size = 20; max_size <= kDecodeBufferSize; ++max_size) {
    std::string long_value = absl::StrCat(std::string(max_size, '0'), max_size);
    long_value.erase(0, long_value.size() - max_size);
    DCHECK_EQ(long_value.size(), max_size);
    const std::string ok_value = long_value.substr(1);

    std::string ok_request =
        absl::StrCat("GET /api/v1/safetymonitor/1/issafe?ClientId=", ok_value,
                     " HTTP/1.1\r\n\r\n");

    alpaca_request.client_id = kResetClientId;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, ok_request, max_size),
              EDecodeStatus::kHttpOk);
    EXPECT_EQ(alpaca_request.client_id, max_size);
    EXPECT_THAT(ok_request, IsEmpty());

    std::string long_request =
        absl::StrCat("GET /api/v1/safetymonitor/1/issafe?ClientId=", long_value,
                     " HTTP/1.1\r\n\r\n");

    alpaca_request.client_id = kResetClientId;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, long_request, max_size),
              EDecodeStatus::kHttpRequestHeaderFieldsTooLarge);
    EXPECT_EQ(alpaca_request.client_id, kResetClientId);
    EXPECT_THAT(long_request, StartsWith(long_value));
  }
}

TEST(RequestDecoderTest, DetectsHeaderValueIsTooLong) {
  // Leading whitespace can be removed from a value one character at a time, but
  // trailing whitespace requires buffer space for the entire value and all of
  // the trailing whitespace and a non-value character (i.e. '\r') at the end.
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  std::string long_whitespace;
  while (long_whitespace.size() <= kDecodeBufferSize) {
    long_whitespace += "\t ";
  }

  for (int max_size = 20; max_size <= kDecodeBufferSize; ++max_size) {
    const auto max_size_str = absl::StrCat(max_size);
    std::string long_value = absl::StrCat(max_size_str, long_whitespace);
    long_value.erase(max_size, std::string::npos);
    DCHECK_EQ(long_value.size(), max_size);
    const std::string ok_value = long_value.substr(0, max_size - 1);

    std::string ok_request =
        absl::StrCat("GET /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",
                     "Some-Name:", long_whitespace, ok_value, "\r\n\r\n");

    alpaca_request.client_id = kResetClientId;
    EXPECT_CALL(listener, OnUnknownHeaderName(Eq("Some-Name")));
    EXPECT_CALL(listener, OnUnknownHeaderValue(Eq(max_size_str)));
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, ok_request, max_size),
              EDecodeStatus::kHttpOk);
    EXPECT_EQ(alpaca_request.client_id, kResetClientId);
    EXPECT_THAT(ok_request, IsEmpty());

    std::string long_request =
        absl::StrCat("GET /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",
                     "Some-Name:", long_whitespace, long_value, "\r\n\r\n");

    alpaca_request.client_id = kResetClientId;
    EXPECT_CALL(listener, OnUnknownHeaderName(Eq("Some-Name")));
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, long_request, max_size),
              EDecodeStatus::kHttpRequestHeaderFieldsTooLarge);
    EXPECT_EQ(alpaca_request.client_id, kResetClientId);
    EXPECT_THAT(long_request, StartsWith(long_value));
  }
}

TEST(RequestDecoderTest, RejectsUnsupportedHttpMethod) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string request_after_method =
      "/api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "Content-Length: 0\r\n"
      "\r\n";

  for (std::string method :
       {"CONNECT", "DELETE", "OPTIONS", "PATCH", "POST", "TRACE"}) {
    const std::string full_request = method + " " + request_after_method;
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
              EDecodeStatus::kHttpMethodNotImplemented);
    EXPECT_THAT(request, EndsWith(request_after_method));
  }
}

TEST(RequestDecoderTest, RejectsUnsupportedAscomMethod) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string request_before_ascom_method("GET /api/v1/safetymonitor/1");
  const std::string request_after_ascom_method(
      " HTTP/1.1\r\n"
      "\r\n");

  for (std::string bogus_ascom_method : {
           "",         // Missing /method.
           "/",        // Missing method.
           "/NAME",    // Wrong case.
           "//name",   // Extra slash at start.
           "/name/",   // Extra slash at end.
           "/name[",   // Wrong terminator at end.
           "/name\t",  // Wrong terminator at end.
       }) {
    const std::string full_request = request_before_ascom_method +
                                     bogus_ascom_method +
                                     request_after_ascom_method;
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
              EDecodeStatus::kHttpNotFound);
    EXPECT_THAT(full_request, EndsWith(request));
    EXPECT_THAT(request, EndsWith(request_after_ascom_method));
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

  for (std::string bogus_path_start : {
           "*",  // "GET*safetymonitor"
           "/",  // "GET/safetymonitor"
       }) {
    const std::string full_request =
        "GET" + bogus_path_start + request_after_path_start;
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
              EDecodeStatus::kHttpBadRequest);
    EXPECT_THAT(full_request, EndsWith(request));
    EXPECT_THAT(request, EndsWith(request_after_path_start));
  }

  for (std::string bogus_path_start : {
           " ",           // "GET safetymonitor" (missing "/api/v1")
           " *",          // "GET *safetymonitor"
           " //api/v1/",  // "GET //api/v1/safetymonitor" (extra "/")
           " /api//v1/",  // "GET /api//v1/safetymonitor" (extra "/")
           " /api/v2/",   // "GET /api/v2/safetymonitor" (wrong version)
           " /API/v1/",   // "GET /API/v1/safetymonitor" (wrong case)
       }) {
    const std::string full_request =
        "GET" + bogus_path_start + request_after_path_start;
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
              EDecodeStatus::kHttpNotFound);
    EXPECT_THAT(full_request, EndsWith(request));
    EXPECT_THAT(request, EndsWith(request_after_path_start));
  }
}

TEST(RequestDecoderTest, RejectsUnknownOrMalformedDeviceType) {
  AlpacaRequest alpaca_request;
  RequestDecoderListener listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string request_before_device_type = "GET /api/v1";
  const std::string request_after_device_number =
      "issafe HTTP/1.1\r\n"
      "Content-Length: 0\r\n"
      "\r\n";

  for (std::string bogus_device_type : {
           "",                  // Missing /devicetype/
           "/",                 // Missing devicetype/
           "//",                // Empty devicetype
           "/safetymonitor",    // Missing / after device type.
           "/SafetyMonitor/",   // Wrong case
           "//safetymonitor/",  // Extra slash at start.
           "/safetymonitor//",  // Extra slash at end.
       }) {
    const std::string full_request = request_before_device_type +
                                     bogus_device_type + "1/" +
                                     request_after_device_number;
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
              EDecodeStatus::kHttpNotFound);
    EXPECT_THAT(full_request, EndsWith(request));
    EXPECT_THAT(request, EndsWith(request_after_device_number));
  }
}

TEST(RequestDecoderTest, RejectsUnsupportedHttpVersion) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  std::string request(
      "GET /api/v1/safetymonitor/0/name HTTP/1.0\r\n"
      "\r\n");
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpVersionNotSupported);
}

TEST(RequestDecoderTest, RejectsInvalidParamNameValueSeparator) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  std::string request(
      "GET /api/v1/safetymonitor/0/name?ClientId:1 HTTP/1.1\r\n"
      "\r\n");
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpBadRequest);
}

TEST(RequestDecoderTest, RejectsInvalidParamSeparator) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  std::string request(
      "GET /api/v1/safetymonitor/0/name?ClientId=1] HTTP/1.1\r\n"
      "\r\n");
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpBadRequest);
}

TEST(RequestDecoderTest, BadHeaderNameEnd) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  std::string request(
      "GET /api/v1/safetymonitor/0/name HTTP/1.1\r\n"
      "Content-Length : ");

  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpBadRequest);
  ASSERT_EQ(request, " : ");
}

TEST(RequestDecoderTest, BadHeaderLineEnd) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string full_request(
      "PUT /api/v1/safetymonitor/0/connected HTTP/1.1\r\n"
      "Content-Length: 10\n\r"
      "\r\n"
      "abc=123456");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    auto result = DecodePartitionedRequest(decoder, partition);

    const EDecodeStatus status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    ASSERT_EQ(status, EDecodeStatus::kHttpBadRequest);
    ASSERT_EQ(remainder, "\n\r\r\nabc=123456");
    ASSERT_EQ(alpaca_request.http_method, EHttpMethod::PUT);
    ASSERT_EQ(alpaca_request.device_type, EDeviceType::kSafetyMonitor);
    ASSERT_EQ(alpaca_request.device_number, 0);
    ASSERT_EQ(alpaca_request.ascom_method, EMethod::kConnected);
  }
}

TEST(RequestDecoderTest, NotifiesListenerOfUnexpectedAccept) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string full_request(
      "GET /api/v1/safetymonitor/0/supportedactions HTTP/1.1\r\n"
      "Content-Length:0\r\n"
      "Accept:  application/x-www-form-urlencoded  \r\n"
      "\r\n");

  EXPECT_CALL(listener, OnExtraHeader(EHttpHeader::kHttpAccept,
                                      Eq("application/x-www-form-urlencoded")))
      .WillOnce(Return(EDecodeStatus::kContinueDecoding));
  auto request = full_request;
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request), EDecodeStatus::kHttpOk);

  EXPECT_CALL(listener, OnExtraHeader(EHttpHeader::kHttpAccept,
                                      Eq("application/x-www-form-urlencoded")))
      .WillOnce(Return(EDecodeStatus::kHttpUnsupportedMediaType));
  request = full_request;
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpUnsupportedMediaType);
}

TEST(RequestDecoderTest, NotifiesListenerOfUnsupportedContentType) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string full_request(
      "PUT /api/v1/safetymonitor/0/connected HTTP/1.1\r\n"
      "Content-Length: 0\r\n"
      "Accept: application/json\r\n"
      "Content-Type: application/json\r\n"
      "\r\n");

  // Decoder will override status if listener doesn't return an error status.
  EXPECT_CALL(listener, OnExtraHeader(EHttpHeader::kHttpContentType,
                                      Eq("application/json")))
      .WillOnce(Return(EDecodeStatus::kContinueDecoding));
  auto request = full_request;
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpUnsupportedMediaType);

  // But will return an error status provided by the listener.
  EXPECT_CALL(listener, OnExtraHeader(EHttpHeader::kHttpContentType,
                                      Eq("application/json")))
      .WillOnce(Return(EDecodeStatus::kHttpNotFound));
  request = full_request;
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpNotFound);
}

TEST(RequestDecoderTest, NotifiesListenerOfUnsupportedAndUnknownHeaders) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  const std::string full_request(
      "GET /api/v1/safetymonitor/0/connected HTTP/1.1\r\n"
      "Content-Encoding: gzip\r\n"
      "Accept-Encoding: deflate\r\n"
      "\r\n");

  // OK if the listener says "continue decoding".
  EXPECT_CALL(listener,
              OnExtraHeader(EHttpHeader::kHttpContentEncoding, Eq("gzip")))
      .WillOnce(Return(EDecodeStatus::kContinueDecoding));
  EXPECT_CALL(listener, OnUnknownHeaderName(Eq("Accept-Encoding")))
      .WillOnce(Return(EDecodeStatus::kContinueDecoding));
  EXPECT_CALL(listener, OnUnknownHeaderValue(Eq("deflate")))
      .WillOnce(Return(EDecodeStatus::kContinueDecoding));
  auto request = full_request;
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request), EDecodeStatus::kHttpOk);

  // But bails if the listeners returns an error status.
  EXPECT_CALL(listener,
              OnExtraHeader(EHttpHeader::kHttpContentEncoding, Eq("gzip")))
      .WillOnce(Return(EDecodeStatus::kContinueDecoding));
  EXPECT_CALL(listener, OnUnknownHeaderName(Eq("Accept-Encoding")))
      .WillOnce(Return(EDecodeStatus::kContinueDecoding));
  EXPECT_CALL(listener, OnUnknownHeaderValue(Eq("deflate")))
      .WillOnce(Return(EDecodeStatus::kHttpMethodNotImplemented));
  request = full_request;
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpMethodNotImplemented);

  EXPECT_CALL(listener,
              OnExtraHeader(EHttpHeader::kHttpContentEncoding, Eq("gzip")))
      .WillOnce(Return(EDecodeStatus::kContinueDecoding));
  EXPECT_CALL(listener, OnUnknownHeaderName(Eq("Accept-Encoding")))
      .WillOnce(Return(EDecodeStatus::kHttpInternalServerError));
  request = full_request;
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpInternalServerError);

  EXPECT_CALL(listener,
              OnExtraHeader(EHttpHeader::kHttpContentEncoding, Eq("gzip")))
      .WillOnce(Return(EDecodeStatus::kHttpPayloadTooLarge));
  request = full_request;
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
            EDecodeStatus::kHttpPayloadTooLarge);
}

TEST(RequestDecoderDeathTest, ListenerReturnsInvalidResponse) {
  AlpacaRequest alpaca_request;
  StrictMock<MockRequestDecoderListener> listener;
  RequestDecoder decoder(alpaca_request, listener);

  std::string request(
      "GET /api/v1/safetymonitor/0/connected HTTP/1.1\r\n"
      "Content-Encoding: gzip\r\n"
      "\r\n");

  // kNeedMoreInput is not allowed as a status for a listener, as it is intended
  // to mean that the current buffer doesn't hold an entire 'token' to be
  // decoded. If the listener returns that, it is converted to an internal
  // error.
  EXPECT_DEBUG_DEATH(
      {
        EXPECT_CALL(listener, OnExtraHeader(EHttpHeader::kHttpContentEncoding,
                                            Eq("gzip")))
            .WillOnce(Return(EDecodeStatus::kNeedMoreInput));
        EXPECT_EQ(ResetAndDecodeFullBuffer(decoder, request),
                  EDecodeStatus::kHttpInternalServerError);
        EXPECT_EQ(request, "\r\n\r\n");
      },
      "kNeedMoreInput");
}

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
