// Tests of RequestDecoder, AlpacaRequest and RequestDecoderListener.
//
// After a bunch of changes, I think there are too many test cases (i.e. they
// overlap in what they test); in addition, I think the individual test cases
// likely test too many things (i.e. aren't well focused).
// TODO(jamessynge): Reduce overlap, increase focus.
//
// Author: james.synge@gmail.com

#include "request_decoder.h"

#include <McuCore.h>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "absl/flags/declare.h"
#include "absl/flags/flag.h"
#include "absl/log/log.h"
#include "absl/strings/escaping.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "alpaca_request.h"
#include "config.h"
#include "constants.h"
#include "extras/test_tools/mock_request_decoder_listener.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mcucore/extras/test_tools/http1/string_utils.h"
#include "mcucore/extras/test_tools/string_view_utils.h"  // IWYU pragma: keep (yes, really used via gtest-matchers.h).
#include "mcucore/extras/test_tools/test_has_failed.h"
#include "request_decoder_listener.h"

#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
#include "extra_parameters.h"
#endif

ABSL_DECLARE_FLAG(int, v);

namespace alpaca {
namespace test {
namespace {

constexpr const size_t kDecodeBufferSize = 40;

using ::mcucore::test::AppendRemainder;
using ::mcucore::test::GenerateMultipleRequestPartitions;
using ::mcucore::test::TestHasFailed;
using ::testing::EndsWith;
using ::testing::HasSubstr;
using ::testing::InSequence;
using ::testing::IsEmpty;
using ::testing::Mock;
using ::testing::StartsWith;
using ::testing::StrictMock;

#if TAS_ENABLE_REQUEST_DECODER_LISTENER
using ::testing::Eq;
using ::testing::Return;
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER

std::vector<std::vector<std::string>> GenerateMultipleRequestPartitions(
    const std::string& full_request) {
  return GenerateMultipleRequestPartitions(full_request, kDecodeBufferSize,
                                           kDecodeBufferSize);
}

bool IsErrorStatus(EHttpStatusCode status) {
  return status >= EHttpStatusCode::kHttpBadRequest;
}

// Decode the contents of buffer until the decoder needs more input or returns
// an error.
EHttpStatusCode DecodeBuffer(
    RequestDecoder& decoder, std::string& buffer,
    const size_t max_decode_buffer_size = kDecodeBufferSize) {
  CHECK_GT(max_decode_buffer_size, 0);
  CHECK_LE(max_decode_buffer_size, mcucore::StringView::kMaxSize);

  while (true) {
    // We deliberately copy into another string, and may append some "garbage",
    // to check for cases where the decoder tries to read too far.
    const size_t initial_size = std::min(max_decode_buffer_size, buffer.size());
    std::string copy = buffer.substr(0, initial_size);
#if MCU_ENABLE_DEBUGGING
    copy.append("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
#endif
    mcucore::StringView view(copy.data(), initial_size);

    const bool was_empty = buffer.empty();
    const bool buffer_is_full = view.size() >= max_decode_buffer_size;
    auto status = decoder.DecodeBuffer(view, buffer_is_full);

    // Make sure that the decoder only removed the prefix of the view.
    EXPECT_GE(initial_size, view.size());
    const size_t removed_size = initial_size - view.size();
    EXPECT_EQ(copy.data() + removed_size, view.data());

    // Make sure that the decoder didn't modify the passed in buffer.
    EXPECT_EQ(buffer.substr(0, initial_size), copy.substr(0, initial_size));

    // Remove the decoded prefix of buffer.
    buffer.erase(0, initial_size - view.size());

    if (status != EHttpStatusCode::kNeedMoreInput) {
      EXPECT_GE(status, EHttpStatusCode::kHttpOk);
      return status;
    } else if (was_empty) {
      return status;
    } else if (removed_size == 0) {
      return status;
    }
  }
}

EHttpStatusCode ResetAndDecodeFullBuffer(
    RequestDecoder& decoder, std::string& buffer,
    const size_t max_decode_buffer_size = kDecodeBufferSize) {
  decoder.Reset();
  return DecodeBuffer(decoder, buffer, max_decode_buffer_size);
}

// Apply the decoder to decoding the provided partition of a request. Returns
// the final decode status, the remainder of the last buffer passed in, and
// all the remaining undecoded text.
std::tuple<EHttpStatusCode, std::string, std::string> DecodePartitionedRequest(
    RequestDecoder& decoder, const std::vector<std::string>& partition,
    const size_t max_decode_buffer_size = kDecodeBufferSize) {
  CHECK_NE(partition.size(), 0);
  CHECK_GT(max_decode_buffer_size, 0);
  CHECK_LE(max_decode_buffer_size, mcucore::StringView::kMaxSize);
  decoder.Reset();
  std::string buffer;
  for (int ndx = 0; ndx < partition.size(); ++ndx) {
    buffer += partition[ndx];
    auto status = DecodeBuffer(decoder, buffer, max_decode_buffer_size);
    if (status != EHttpStatusCode::kNeedMoreInput) {
      return {status, buffer, AppendRemainder(buffer, partition, ndx + 1)};
    }
  }
  return {EHttpStatusCode::kNeedMoreInput, buffer, buffer};
}

size_t GetNumExtraParameters(const AlpacaRequest& request) {
#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
  return request.extra_parameters.size();
#else
  return 0;
#endif
}

// TODO(jamessynge): Add parameter to test that controls whether the listener is
// installed. Use TAS_ENABLE_REQUEST_DECODER_LISTENER to decide whether the test
// cases are executed just once (i.e. if it equals 0), or are executed twice,
// once with and once without a listener.
class RequestDecoderTest : public ::testing::Test {
 public:
  RequestDecoderTest()
      :
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
        decoder_(alpaca_request_, &listener_)
#else
        decoder_(alpaca_request_)
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
  {
    // ctor body.
  }

 protected:
  // TODO(jamessynge): Adjust based on the parameter.
  bool HasListener() const { return TAS_ENABLE_REQUEST_DECODER_LISTENER != 0; }

  // Returns the implied expectation on the final decoder result.
  EHttpStatusCode MaybeExpectAssetPathSegment(
      std::string_view segment, bool is_last_segment,
      EHttpStatusCode listener_return_value =
          EHttpStatusCode::kContinueDecoding,
      EHttpStatusCode non_listener_decode_status =
          EHttpStatusCode::kContinueDecoding) {
#if TAS_ENABLE_ASSET_PATH_DECODING
    if (HasListener()) {
      EXPECT_CALL(listener_, OnAssetPathSegment(Eq(segment), is_last_segment))
          .WillOnce(Return(listener_return_value));
      // If there is no listener
      return ToImpliedDecoderStatus(listener_return_value,
                                    non_listener_decode_status);
    }
#endif  // TAS_ENABLE_ASSET_PATH_DECODING
    // If there is no listener, then a Not Found status is returned.
    return EHttpStatusCode::kHttpNotFound;
  }

  EHttpStatusCode MaybeExpectExtraParameter(
      EParameter parameter, std::string_view value,
      EHttpStatusCode listener_return_value =
          EHttpStatusCode::kContinueDecoding,
      EHttpStatusCode non_listener_decode_status =
          EHttpStatusCode::kContinueDecoding) {
#if TAS_ENABLE_EXTRA_PARAMETER_DECODING
    if (HasListener()) {
      EXPECT_CALL(listener_, OnExtraParameter(parameter, Eq(value)))
          .WillOnce(Return(listener_return_value));
      return ToImpliedDecoderStatus(listener_return_value,
                                    non_listener_decode_status);
    }
#endif  // TAS_ENABLE_EXTRA_PARAMETER_DECODING
    return non_listener_decode_status;
  }

  EHttpStatusCode MaybeExpectUnknownParameterName(
      std::string_view name,
      EHttpStatusCode listener_return_value =
          EHttpStatusCode::kContinueDecoding,
      EHttpStatusCode non_listener_decode_status =
          EHttpStatusCode::kContinueDecoding) {
#if TAS_ENABLE_UNKNOWN_PARAMETER_DECODING
    if (HasListener()) {
      EXPECT_CALL(listener_, OnUnknownParameterName(Eq(name)))
          .WillOnce(Return(listener_return_value));
      return ToImpliedDecoderStatus(listener_return_value,
                                    non_listener_decode_status);
    }
#endif  // TAS_ENABLE_UNKNOWN_PARAMETER_DECODING
    return non_listener_decode_status;
  }

  EHttpStatusCode MaybeExpectUnknownParameterValue(
      std::string_view value,
      EHttpStatusCode listener_return_value =
          EHttpStatusCode::kContinueDecoding,
      EHttpStatusCode non_listener_decode_status =
          EHttpStatusCode::kContinueDecoding) {
#if TAS_ENABLE_UNKNOWN_PARAMETER_DECODING
    if (HasListener()) {
      EXPECT_CALL(listener_, OnUnknownParameterValue(Eq(value)))
          .WillOnce(Return(listener_return_value));
      return ToImpliedDecoderStatus(listener_return_value,
                                    non_listener_decode_status);
    }
#endif  // TAS_ENABLE_UNKNOWN_PARAMETER_DECODING
    return non_listener_decode_status;
  }

  EHttpStatusCode MaybeExpectUnknownParameter(
      std::string name, std::string_view value,
      EHttpStatusCode listener_return_value =
          EHttpStatusCode::kContinueDecoding,
      EHttpStatusCode non_listener_decode_status =
          EHttpStatusCode::kContinueDecoding) {
    MaybeExpectUnknownParameterName(name);
    return MaybeExpectUnknownParameterValue(value, listener_return_value,
                                            non_listener_decode_status);
  }

  EHttpStatusCode MaybeExpectExtraHeader(
      EHttpHeader header, std::string_view value,
      EHttpStatusCode listener_return_value =
          EHttpStatusCode::kContinueDecoding,
      EHttpStatusCode non_listener_decode_status =
          EHttpStatusCode::kContinueDecoding) {
#if TAS_ENABLE_EXTRA_HEADER_DECODING
    if (HasListener()) {
      EXPECT_CALL(listener_, OnExtraHeader(header, Eq(value)))
          .WillOnce(Return(listener_return_value));
      return ToImpliedDecoderStatus(listener_return_value,
                                    non_listener_decode_status);
    }
#endif  // TAS_ENABLE_EXTRA_HEADER_DECODING
    return ToImpliedDecoderStatus(EHttpStatusCode::kContinueDecoding,
                                  non_listener_decode_status);
  }

  EHttpStatusCode MaybeExpectUnknownHeaderName(
      std::string_view name,
      EHttpStatusCode listener_return_value =
          EHttpStatusCode::kContinueDecoding,
      EHttpStatusCode non_listener_decode_status = EHttpStatusCode::kHttpOk) {
#if TAS_ENABLE_UNKNOWN_HEADER_DECODING
    if (HasListener()) {
      EXPECT_CALL(listener_, OnUnknownHeaderName(Eq(name)))
          .WillOnce(Return(listener_return_value));
      return ToImpliedDecoderStatus(listener_return_value,
                                    non_listener_decode_status);
    }
#endif  // TAS_ENABLE_UNKNOWN_HEADER_DECODING
    return non_listener_decode_status;
  }

  EHttpStatusCode MaybeExpectUnknownHeaderValue(
      std::string_view value,
      EHttpStatusCode listener_return_value =
          EHttpStatusCode::kContinueDecoding,
      EHttpStatusCode non_listener_decode_status =
          EHttpStatusCode::kContinueDecoding) {
#if TAS_ENABLE_UNKNOWN_HEADER_DECODING
    if (HasListener()) {
      EXPECT_CALL(listener_, OnUnknownHeaderValue(Eq(value)))
          .WillOnce(Return(listener_return_value));
      return listener_return_value;
    }
#endif  // TAS_ENABLE_UNKNOWN_HEADER_DECODING
    return EHttpStatusCode::kHttpOk;
    // return ToImpliedDecoderStatus(EHttpStatusCode::kContinueDecoding,
    //                               non_listener_decode_status);
  }

  EHttpStatusCode MaybeExpectUnknownHeader(
      std::string name, std::string_view value,
      EHttpStatusCode listener_return_value =
          EHttpStatusCode::kContinueDecoding,
      EHttpStatusCode non_listener_decode_status =
          EHttpStatusCode::kContinueDecoding) {
    MaybeExpectUnknownHeaderName(name);
    return MaybeExpectUnknownHeaderValue(value, listener_return_value,
                                         non_listener_decode_status);
  }

  void VerifyAndClearListenerExpectations() {
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
    Mock::VerifyAndClearExpectations(&listener_);
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
  }

  AlpacaRequest alpaca_request_;
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
  StrictMock<MockRequestDecoderListener> listener_;
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
  RequestDecoder decoder_;

 private:
  EHttpStatusCode ToImpliedDecoderStatus(
      EHttpStatusCode listener_return_value,
      EHttpStatusCode non_listener_decode_status) {
    EXPECT_NE(non_listener_decode_status, EHttpStatusCode::kNeedMoreInput);
    if (listener_return_value == EHttpStatusCode::kNeedMoreInput) {
      LOG(INFO) << "MCU DCHECK expected";
    }
    if (IsErrorStatus(listener_return_value)) {
      return listener_return_value;
    } else if (IsErrorStatus(non_listener_decode_status)) {
      // The non_listener_decode_status implies that a failure is going to be
      // forced by the decoder if the listener doesn't return an error.
      return non_listener_decode_status;
    } else {
      // Implies that (so far) the request is OK, so unless things change after
      // this listener call, the decoder should return OK.
      return EHttpStatusCode::kHttpOk;
    }
  }
};

// Death tests are executed first, so I'm placing such tests first.
using RequestDecoderDeathTest = RequestDecoderTest;

#if TAS_ENABLE_EXTRA_HEADER_DECODING
// kNeedMoreInput is not allowed as a return status from a listener method, as
// it is intended to mean that the current buffer doesn't hold an entire 'token'
// to be decoded. If the listener returns that, it is converted to an internal
// error.
TEST_F(RequestDecoderDeathTest, OnExtraHeaderReturnsInvalidStatus) {
  std::string request(
      "GET /api/v1/safetymonitor/0/connected HTTP/1.1\r\n"
      "Date: anything\r\n"
      "\r\n");

  EXPECT_DEBUG_DEATH(
      {
        MaybeExpectExtraHeader(EHttpHeader::kDate, "anything",
                               EHttpStatusCode::kNeedMoreInput);
        EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
                  EHttpStatusCode::kHttpInternalServerError);
        EXPECT_EQ(request, "\r\n\r\n");
      },
      "kNeedMoreInput");
}
#endif  // TAS_ENABLE_EXTRA_HEADER_DECODING

TEST_F(RequestDecoderTest, UnusedDecoder) {
  LOG(INFO) << "sizeof(AlpacaRequest) " << sizeof(AlpacaRequest);
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
  LOG(INFO) << "sizeof(RequestDecoderListener) "
            << sizeof(RequestDecoderListener);
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
  LOG(INFO) << "sizeof(RequestDecoder) " << sizeof(RequestDecoder);
}

TEST_F(RequestDecoderTest, ResetOnly) { decoder_.Reset(); }

TEST_F(RequestDecoderTest, ResetRequiredBeforeDecoding) {
  const std::string full_request(
      "GET /api/v1/safetymonitor/0/issafe HTTP/1.1\r\n"
      "\r\n");
  auto buffer = full_request;

  EXPECT_EQ(DecodeBuffer(decoder_, buffer),
            EHttpStatusCode::kHttpInternalServerError);
  EXPECT_EQ(buffer, full_request);  // No input has been consumed.
}

TEST_F(RequestDecoderTest, SmallestHomePageRequest) {
  const std::string full_request(
      "GET / HTTP/1.1\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
    EXPECT_THAT(buffer, IsEmpty());
    EXPECT_THAT(remainder, IsEmpty());
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::GET);

    EXPECT_EQ(alpaca_request_.api_group, EApiGroup::kServerStatus);
    EXPECT_EQ(alpaca_request_.api, EAlpacaApi::kServerStatus);
    EXPECT_FALSE(alpaca_request_.have_client_id);
    EXPECT_FALSE(alpaca_request_.have_client_transaction_id);

    if (TestHasFailed()) {
      break;
    }
  }
}

TEST_F(RequestDecoderTest, SmallestDeviceApiGetRequest) {
  const std::string full_request(
      "GET /api/v1/safetymonitor/0/issafe HTTP/1.1\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
    EXPECT_THAT(buffer, IsEmpty());
    EXPECT_THAT(remainder, IsEmpty());
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::GET);

    EXPECT_EQ(alpaca_request_.api_group, EApiGroup::kDevice);
    EXPECT_EQ(alpaca_request_.api, EAlpacaApi::kDeviceApi);
    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kSafetyMonitor);
    EXPECT_EQ(alpaca_request_.device_number, 0);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kIsSafe);
    EXPECT_FALSE(alpaca_request_.have_client_id);
    EXPECT_FALSE(alpaca_request_.have_client_transaction_id);

    if (TestHasFailed()) {
      break;
    }
  }
}

TEST_F(RequestDecoderTest, SmallestDeviceSetupRequest) {
  const std::string full_request(
      "GET /setup/v1/safetymonitor/9/setup HTTP/1.1\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
    EXPECT_THAT(buffer, IsEmpty());
    EXPECT_THAT(remainder, IsEmpty());
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::GET);
    EXPECT_EQ(alpaca_request_.api_group, EApiGroup::kSetup);
    EXPECT_EQ(alpaca_request_.api, EAlpacaApi::kDeviceSetup);
    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kSafetyMonitor);
    EXPECT_EQ(alpaca_request_.device_number, 9);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kSetup);
    EXPECT_FALSE(alpaca_request_.have_client_id);
    EXPECT_FALSE(alpaca_request_.have_client_transaction_id);
    EXPECT_EQ(GetNumExtraParameters(alpaca_request_), 0);

    if (TestHasFailed()) {
      break;
    }
  }
}

TEST_F(RequestDecoderTest, SmallestApiVersionsRequest) {
  const std::string full_request(
      "GET /management/apiversions HTTP/1.1\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
    EXPECT_THAT(buffer, IsEmpty());
    EXPECT_THAT(remainder, IsEmpty());
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::GET);
    EXPECT_EQ(alpaca_request_.api_group, EApiGroup::kManagement);
    EXPECT_EQ(alpaca_request_.api, EAlpacaApi::kManagementApiVersions);

    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kUnknown);
    EXPECT_EQ(alpaca_request_.device_number, kResetDeviceNumber);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kUnknown);
    EXPECT_FALSE(alpaca_request_.have_client_id);
    EXPECT_FALSE(alpaca_request_.have_client_transaction_id);
    EXPECT_EQ(GetNumExtraParameters(alpaca_request_), 0);

    if (TestHasFailed()) {
      break;
    }
  }
}

TEST_F(RequestDecoderTest, SmallestConfiguredDevicesRequest) {
  const std::string full_request(
      "GET /management/v1/configureddevices HTTP/1.1\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
    EXPECT_THAT(buffer, IsEmpty());
    EXPECT_THAT(remainder, IsEmpty());
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::GET);
    EXPECT_EQ(alpaca_request_.api_group, EApiGroup::kManagement);
    EXPECT_EQ(alpaca_request_.api, EAlpacaApi::kManagementConfiguredDevices);

    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kUnknown);
    EXPECT_EQ(alpaca_request_.device_number, kResetDeviceNumber);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kUnknown);
    EXPECT_FALSE(alpaca_request_.have_client_id);
    EXPECT_FALSE(alpaca_request_.have_client_transaction_id);
    EXPECT_EQ(GetNumExtraParameters(alpaca_request_), 0);

    if (TestHasFailed()) {
      break;
    }
  }
}

TEST_F(RequestDecoderTest, SmallestServerDescriptionRequest) {
  const std::string full_request(
      "GET /management/v1/description HTTP/1.1\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    if (TestHasFailed()) {
      break;
    }
    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
    EXPECT_THAT(buffer, IsEmpty());
    EXPECT_THAT(remainder, IsEmpty());
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::GET);
    EXPECT_EQ(alpaca_request_.api_group, EApiGroup::kManagement);
    EXPECT_EQ(alpaca_request_.api, EAlpacaApi::kManagementDescription);

    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kUnknown);
    EXPECT_EQ(alpaca_request_.device_number, kResetDeviceNumber);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kUnknown);
    EXPECT_FALSE(alpaca_request_.have_client_id);
    EXPECT_FALSE(alpaca_request_.have_client_transaction_id);
    EXPECT_EQ(GetNumExtraParameters(alpaca_request_), 0);

    if (TestHasFailed()) {
      break;
    }
  }
}

TEST_F(RequestDecoderTest, SmallestServerSetupRequest) {
  const std::string full_request(
      "GET /setup HTTP/1.1\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
    EXPECT_THAT(buffer, IsEmpty());
    EXPECT_THAT(remainder, IsEmpty());
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::GET);
    EXPECT_EQ(alpaca_request_.api_group, EApiGroup::kSetup);
    EXPECT_EQ(alpaca_request_.api, EAlpacaApi::kServerSetup);

    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kUnknown);
    EXPECT_EQ(alpaca_request_.device_number, kResetDeviceNumber);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kUnknown);
    EXPECT_FALSE(alpaca_request_.have_client_id);
    EXPECT_FALSE(alpaca_request_.have_client_transaction_id);
    EXPECT_EQ(GetNumExtraParameters(alpaca_request_), 0);

    if (TestHasFailed()) {
      break;
    }
  }
}

TEST_F(RequestDecoderTest, SmallestPutRequest) {
  const std::string full_request(
      "PUT /api/v1/observingconditions/0/refresh"
      "?ClientID=123&clienttransactionid=432 "
      "HTTP/1.1\r\n"
      "Content-Length: 0\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
    EXPECT_THAT(buffer, IsEmpty());
    EXPECT_THAT(remainder, IsEmpty());
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::PUT);
    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kObservingConditions);
    EXPECT_EQ(alpaca_request_.device_number, 0);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kRefresh);
    EXPECT_TRUE(alpaca_request_.have_client_id);
    EXPECT_TRUE(alpaca_request_.have_client_transaction_id);
    EXPECT_EQ(alpaca_request_.client_id, 123);
    EXPECT_EQ(alpaca_request_.client_transaction_id, 432);

    if (TestHasFailed()) {
      break;
    }
  }
}

TEST_F(RequestDecoderTest, SmallestAssetRequest) {
  const std::string full_request(
      "GET /asset/ HTTP/1.1\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    const auto expected_status = MaybeExpectAssetPathSegment("", true);

    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    VerifyAndClearListenerExpectations();
    EXPECT_EQ(status, expected_status);
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::GET);
    EXPECT_EQ(alpaca_request_.api_group, EApiGroup::kAsset);
    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kUnknown);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kUnknown);
    EXPECT_FALSE(alpaca_request_.have_client_id);
    EXPECT_FALSE(alpaca_request_.have_client_transaction_id);
    if (expected_status == EHttpStatusCode::kHttpOk) {
      EXPECT_THAT(buffer, IsEmpty());
      EXPECT_THAT(remainder, IsEmpty());
    } else {
      EXPECT_THAT(remainder, StartsWith(" HTTP"));
    }
    if (TestHasFailed()) {
      break;
    }
  }
}

TEST_F(RequestDecoderTest, AssetRequest) {
  const std::string full_request(
      "GET /asset/html/setup.html HTTP/1.1\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    InSequence s;
    MaybeExpectAssetPathSegment("html", false);
    const auto expected_status =
        MaybeExpectAssetPathSegment("setup.html", true);

    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    VerifyAndClearListenerExpectations();
    EXPECT_EQ(status, expected_status);
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::GET);
    EXPECT_EQ(alpaca_request_.api_group, EApiGroup::kAsset);
    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kUnknown);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kUnknown);
    EXPECT_FALSE(alpaca_request_.have_client_id);
    EXPECT_FALSE(alpaca_request_.have_client_transaction_id);
    if (expected_status == EHttpStatusCode::kHttpOk) {
      EXPECT_THAT(buffer, IsEmpty());
      EXPECT_THAT(remainder, IsEmpty());
    } else {
      EXPECT_THAT(remainder, StartsWith("html/setup.html"));
    }
    if (TestHasFailed()) {
      break;
    }
  }
}

TEST_F(RequestDecoderTest, RejectAssetRequest) {
  const std::string full_request(
      "GET /asset/html/setup.html HTTP/1.1\r\n"
      "\r\n");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    InSequence s;
    MaybeExpectAssetPathSegment("html", false);
    const auto expected_status = MaybeExpectAssetPathSegment(
        "setup.html", true, EHttpStatusCode::kHttpNotAcceptable);

    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    VerifyAndClearListenerExpectations();
    EXPECT_EQ(status, expected_status);
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::GET);
    EXPECT_EQ(alpaca_request_.api_group, EApiGroup::kAsset);
    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kUnknown);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kUnknown);
    EXPECT_FALSE(alpaca_request_.have_client_id);
    EXPECT_FALSE(alpaca_request_.have_client_transaction_id);
    if (expected_status == EHttpStatusCode::kHttpOk) {
      EXPECT_THAT(buffer, IsEmpty());
      EXPECT_THAT(remainder, IsEmpty());
    } else if (expected_status == EHttpStatusCode::kHttpNotAcceptable) {
      EXPECT_THAT(remainder, StartsWith(" HTTP/1.1"));
    } else {
      EXPECT_THAT(remainder, StartsWith("html/setup.html"));
    }
    if (TestHasFailed()) {
      break;
    }
  }
}

TEST_F(RequestDecoderTest, AllSupportedFeatures) {
  const std::string body = "a=1&raw=true&&ClienttransACTIONid=9";
  const std::string full_request = absl::StrCat(
      "PUT /api/v1/safetymonitor/9999/connected?ClientID=321&AbC=xYz "
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
    {
      InSequence s;
      MaybeExpectUnknownParameter("AbC", "xYz");
      MaybeExpectUnknownHeader("Host", "example.com");
      MaybeExpectUnknownHeader("Another-Header",
                               "Some Text, e.g. foo@example.com!");
      MaybeExpectUnknownHeader("accept", "application/json");
      MaybeExpectUnknownParameter("a", "1");
      MaybeExpectExtraParameter(EParameter::kRaw, "true");
    }

    auto result = DecodePartitionedRequest(decoder_, partition);

    VerifyAndClearListenerExpectations();

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
    EXPECT_THAT(buffer, IsEmpty());
    EXPECT_THAT(remainder, IsEmpty());
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::PUT);
    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kSafetyMonitor);
    EXPECT_EQ(alpaca_request_.device_number, 9999);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kConnected);
    EXPECT_TRUE(alpaca_request_.have_client_id);
    EXPECT_TRUE(alpaca_request_.have_client_transaction_id);
    EXPECT_EQ(alpaca_request_.client_id, 321);
    EXPECT_EQ(alpaca_request_.client_transaction_id, 9);
    EXPECT_FALSE(alpaca_request_.do_close);

#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
    // TODO(jamessynge): Implement this feature.
    // EXPECT_EQ(GetNumExtraParameters(alpaca_request_), 1);
    // EXPECT_TRUE(
    //     alpaca_request_.extra_parameters.contains(EParameter::kConnected));
    // EXPECT_EQ(alpaca_request_.extra_parameters.find(EParameter::kConnected),
    //           mcucore::StringView("abc"));
#else
    EXPECT_EQ(GetNumExtraParameters(alpaca_request_), 0);
#endif

    if (TestHasFailed()) {
      break;
    }
  }
}

TEST_F(RequestDecoderTest, RequestsWithClientId) {
  for (const auto& path : {
           "/setup/v1/observingconditions/987654/setup",
           "/api/v1/observingconditions/987654/connected",
       }) {
    for (const auto& param_name : {"clientid", "ClientID", "CLIENTID"}) {
      const auto full_request =
          absl::StrCat("GET ", path, "?", param_name, "=3456 HTTP/1.1\r\n\r\n");
      auto request = full_request;
      EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
                EHttpStatusCode::kHttpOk);
      EXPECT_THAT(request, IsEmpty());

      EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kObservingConditions);
      EXPECT_EQ(alpaca_request_.device_number, 987654);

      EXPECT_TRUE(alpaca_request_.have_client_id);
      EXPECT_EQ(alpaca_request_.client_id, 3456);

      EXPECT_FALSE(alpaca_request_.have_client_transaction_id);
      EXPECT_EQ(alpaca_request_.client_transaction_id,
                kResetClientTransactionId);
    }
  }
}

TEST_F(RequestDecoderTest, RequestWithoutClientId) {
  decoder_.Reset();
  EXPECT_FALSE(alpaca_request_.have_client_id);

  std::string full_request(
      "GET /setup HTTP/1.1\r\n"
      "\r\n");

  EXPECT_EQ(DecodeBuffer(decoder_, full_request, kDecodeBufferSize),
            EHttpStatusCode::kHttpOk);
  EXPECT_FALSE(alpaca_request_.have_client_id);
}

TEST_F(RequestDecoderTest, RequestsWithClientTransactionId) {
  for (const auto& path : {
           "/",
           "/setup",
           "/setup/v1/safetymonitor/7777/setup",
           "/api/v1/safetymonitor/7777/connected",
       }) {
    for (const auto& param_name : {"clienttransactionid", "ClientTransactionID",
                                   "CLIENTTRANSACTIONID"}) {
      const auto full_request =
          absl::StrCat("GET ", path, "?", param_name, "=0 HTTP/1.1\r\n\r\n");
      auto request = full_request;
      EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
                EHttpStatusCode::kHttpOk);
      EXPECT_THAT(request, IsEmpty());

      EXPECT_FALSE(alpaca_request_.have_client_id);
      EXPECT_EQ(alpaca_request_.client_id, kResetClientId);

      EXPECT_TRUE(alpaca_request_.have_client_transaction_id);
      EXPECT_EQ(alpaca_request_.client_transaction_id, 0);
    }
  }
}

TEST_F(RequestDecoderTest, ParamSeparatorsAtEndOfBody) {
  std::string body = "ClientId=876&&&&&&&&&";

  std::string request =
      absl::StrCat("PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",
                   "Content-Length: ", body.size(), "\r\n", "\r\n", body);
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpOk);
  EXPECT_TRUE(alpaca_request_.have_client_id);
  EXPECT_EQ(alpaca_request_.client_id, 876);
  EXPECT_FALSE(alpaca_request_.have_client_transaction_id);

  // Extra spaces at the end, not acceptable.
  body = "ClientId=654&&&&&&&&&   ";
  request = absl::StrCat("PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",
                         "Content-Length: ", body.size(), "\r\n", "\r\n", body);
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpBadRequest);
  EXPECT_TRUE(alpaca_request_.have_client_id);
  EXPECT_EQ(alpaca_request_.client_id, 654);
  EXPECT_FALSE(alpaca_request_.have_client_transaction_id);
}

TEST_F(RequestDecoderTest, RejectsOutOfRangeDeviceNumber) {
  std::string full_request(
      "GET /api/v1/safetymonitor/4294967300/issafe "
      "HTTP/1.1\r\n\r\n");

  alpaca_request_.client_id = kResetClientId;
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, full_request),
            EHttpStatusCode::kHttpBadRequest);
  EXPECT_EQ(alpaca_request_.client_id,
            kResetClientId);  // Hasn't been overwritten.
  // It isn't important how much of the request has been processed, however we
  // don't otherwise have a great way to confirm that the reason for the failure
  // was the device number, vs. the ASCOM method name.
  EXPECT_THAT(full_request, EndsWith("issafe HTTP/1.1\r\n\r\n"));
}

TEST_F(RequestDecoderTest, RejectsOutOfRangeClientId) {
  std::string full_request(
      "GET /api/v1/safetymonitor/0000004294967295/issafe?ClientId=4294967296 "
      "HTTP/1.1\r\n\r\n");
  alpaca_request_.client_id = kResetClientId;

  const auto expected_status = MaybeExpectExtraParameter(
      EParameter::kClientID, "4294967296",
      EHttpStatusCode::kHttpPayloadTooLarge, EHttpStatusCode::kHttpBadRequest);
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, full_request), expected_status);
  EXPECT_EQ(alpaca_request_.device_number, 4294967295UL);
  EXPECT_FALSE(alpaca_request_.have_client_id);
  EXPECT_EQ(alpaca_request_.client_id,
            kResetClientId);  // Hasn't been overwritten.
}

TEST_F(RequestDecoderTest, RejectsOutOfRangeClientTransactionId) {
  // ClientTransactionId fits in a uint32_t.
  std::string body = "ClientTransactionId=444444444&ClientId=1";
  std::string request =
      absl::StrCat("PUT /api/v1/safetymonitor/7/connected HTTP/1.1\r\n",
                   "Content-Length:", body.size(), "\r\n\r\n", body);

  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpOk);
  EXPECT_EQ(alpaca_request_.device_number, 7);
  EXPECT_TRUE(alpaca_request_.have_client_transaction_id);
  EXPECT_EQ(alpaca_request_.client_transaction_id, 444444444);
  EXPECT_TRUE(alpaca_request_.have_client_id);
  EXPECT_EQ(alpaca_request_.client_id, 1);

  // Append another digit, now too big to fit in a uint32_t. This will prevent
  // ClientId from ever being examined.
  body = "ClientTransactionId=4444444444&ClientId=1";
  request = absl::StrCat("PUT /api/v1/safetymonitor/7/connected HTTP/1.1\r\n",
                         "Content-Length:", body.size(), "\r\n\r\n", body);
  alpaca_request_.client_id = kResetClientId;
  alpaca_request_.client_transaction_id = kResetClientTransactionId;

  MaybeExpectExtraParameter(EParameter::kClientTransactionID, "4444444444");
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpBadRequest);
  EXPECT_THAT(request, StartsWith("&ClientId"));
  EXPECT_EQ(alpaca_request_.device_number, 7);
  EXPECT_FALSE(alpaca_request_.have_client_id);
  EXPECT_FALSE(alpaca_request_.have_client_transaction_id);
  // Confirm that neither client id hasn't been overwritten.
  EXPECT_EQ(alpaca_request_.client_id, kResetClientId);
  EXPECT_EQ(alpaca_request_.client_transaction_id, kResetClientTransactionId);
}

// We require the content length so that we will know when the end of the body
// has been reached; otherwise we'd need to support more complex encodings, such
// as with boundary markers for which we'd need storage.
TEST_F(RequestDecoderTest, MissingContentLength) {
  // Start with a missing Content-Length.
  std::string request =
      "PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "\r\n";
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpLengthRequired);
  EXPECT_EQ(alpaca_request_.device_number, 1);
  EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kIsSafe);
}

TEST_F(RequestDecoderTest, ContentLengthNotAnInteger) {
  std::string request =
      "PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "Content-Length: .0\r\n"
      "\r\n";

  const auto expected_status = MaybeExpectExtraHeader(
      EHttpHeader::kContentLength, ".0", EHttpStatusCode::kHttpNotAcceptable,
      EHttpStatusCode::kHttpBadRequest);

  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request), expected_status);
  EXPECT_EQ(alpaca_request_.device_number, 1);
  EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kIsSafe);
}

TEST_F(RequestDecoderTest, ContentLengthTooLargeToStore) {
  const auto kTooLong =
      absl::StrCat(RequestDecoderState::kMaxPayloadSize + 1LL);
  auto request =
      absl::StrCat("PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",  //
                   "content-LENGTH: ", kTooLong, "\r\n",               //
                   "\r\n");

  const auto expected_status =
      MaybeExpectExtraHeader(EHttpHeader::kContentLength, kTooLong,
                             EHttpStatusCode::kHttpUnsupportedMediaType,
                             EHttpStatusCode::kHttpPayloadTooLarge);

  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request), expected_status);
  EXPECT_EQ(alpaca_request_.device_number, 1);
  EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kIsSafe);
  EXPECT_EQ(request, "\r\n\r\n");
}

TEST_F(RequestDecoderTest, ContentLengthTooLargeToDecode) {
  // Provide a Content-Length that is too large to decode.
  const auto kTooLong = absl::StrCat(std::numeric_limits<uint64_t>::max());
  auto request =
      absl::StrCat("PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",  //
                   "content-LENGTH: ", kTooLong, "\r\n",               //
                   "\r\n");

  const auto expected_status = MaybeExpectExtraHeader(
      EHttpHeader::kContentLength, kTooLong,
      EHttpStatusCode::kHttpMethodNotAllowed, EHttpStatusCode::kHttpBadRequest);

  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request), expected_status);
  EXPECT_EQ(alpaca_request_.device_number, 1);
  EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kIsSafe);
  EXPECT_EQ(request, "\r\n\r\n");
}

TEST_F(RequestDecoderTest, DecodeMaxStringViewBody) {
  // A body of length 255 can be decoded.
  ASSERT_LE(255, RequestDecoderState::kMaxPayloadSize);
  // Need to make a body of that size which is valid.
  const std::string frag39 = "nineteen_characters=nineteen_characters";
  const auto frag239 =
      absl::StrJoin({frag39, frag39, frag39, frag39, frag39, frag39}, "&");
  EXPECT_EQ(frag239.size(), 239);
  const auto body = frag239 + "&a=0124567890123";
  EXPECT_EQ(body.size(), 255);
  const auto full_request =
      absl::StrCat("PUT /api/v1/observingconditions/1/action HTTP/1.1\r\n",
                   "CONTENT-LENGTH: 255\r\n", "\r\n", body);

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    InSequence s;
    MaybeExpectUnknownParameter("nineteen_characters", "nineteen_characters");
    MaybeExpectUnknownParameter("nineteen_characters", "nineteen_characters");
    MaybeExpectUnknownParameter("nineteen_characters", "nineteen_characters");
    MaybeExpectUnknownParameter("nineteen_characters", "nineteen_characters");
    MaybeExpectUnknownParameter("nineteen_characters", "nineteen_characters");
    MaybeExpectUnknownParameter("nineteen_characters", "nineteen_characters");
    MaybeExpectUnknownParameter("a", "0124567890123");

    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
    EXPECT_THAT(buffer, IsEmpty());
    EXPECT_THAT(remainder, IsEmpty());
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::PUT);
    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kObservingConditions);
    EXPECT_EQ(alpaca_request_.device_number, 1);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kAction);
    EXPECT_FALSE(alpaca_request_.have_client_id);
    EXPECT_FALSE(alpaca_request_.have_client_transaction_id);
    EXPECT_FALSE(alpaca_request_.do_close);
  }
}

TEST_F(RequestDecoderTest, DetectsPayloadTooLong) {
  std::string request =
      "PUT /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "Content-Length: 1\r\n"
      "\r\n"
      "12";
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpPayloadTooLarge);
}

TEST_F(RequestDecoderTest, DetectsParameterValueIsTooLong) {
  for (int max_size = 20; max_size <= kDecodeBufferSize; ++max_size) {
    std::string long_value = absl::StrCat(std::string(max_size, '0'), max_size);
    long_value.erase(0, long_value.size() - max_size);
    DCHECK_EQ(long_value.size(), max_size);
    const std::string ok_value = long_value.substr(1);

    std::string ok_request =
        absl::StrCat("GET /api/v1/safetymonitor/1/issafe?ClientId=", ok_value,
                     " HTTP/1.1\r\n\r\n");

    alpaca_request_.client_id = kResetClientId;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, ok_request, max_size),
              EHttpStatusCode::kHttpOk);
    EXPECT_EQ(alpaca_request_.client_id, max_size);
    EXPECT_THAT(ok_request, IsEmpty());

    std::string long_request =
        absl::StrCat("GET /api/v1/safetymonitor/1/issafe?ClientId=", long_value,
                     " HTTP/1.1\r\n\r\n");

    alpaca_request_.client_id = kResetClientId;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, long_request, max_size),
              EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge);
    EXPECT_EQ(alpaca_request_.client_id, kResetClientId);
    EXPECT_THAT(long_request, StartsWith(long_value));
  }
}

TEST_F(RequestDecoderTest, DetectsHeaderValueIsTooLong) {
  // For header names that are known, the decoder requires that the longest
  // value fits into the input buffer; that includes any trailing whitespace
  // because some values are allowed to have embedded whitespace, so the decoder
  // must find a '\r' in order to determine that the end of the value is in the
  // input buffer.

  std::string long_whitespace;
  while (long_whitespace.size() <= kDecodeBufferSize) {
    long_whitespace += "\t ";
  }

  for (int max_size = 1; max_size <= kDecodeBufferSize - 1; ++max_size) {
    const std::string ok_value(max_size, '0');
    std::string ok_request =
        absl::StrCat("GET /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",
                     "Content-Length:", long_whitespace, ok_value, "\r\n\r\n");

    alpaca_request_.client_id = kResetClientId;

    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, ok_request),
              EHttpStatusCode::kHttpOk);
    EXPECT_EQ(alpaca_request_.client_id, kResetClientId);
    EXPECT_THAT(ok_request, IsEmpty());

    std::string long_request =
        absl::StrCat("GET /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",
                     "Content-Length:", long_whitespace, ok_value,
                     long_whitespace, "\r\n\r\n");

    alpaca_request_.client_id = kResetClientId;

    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, long_request),
              EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge);
    EXPECT_EQ(alpaca_request_.client_id, kResetClientId);
    EXPECT_THAT(long_request, StartsWith(ok_value));
  }

  // If the value fills the decode buffer, then the decoder can't tell whether
  // it has the whole value, or if it is even longer.
  const std::string long_value(kDecodeBufferSize, '0');
  std::string long_request =
      absl::StrCat("GET /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",
                   "Content-Length:", long_whitespace, long_value, "\r\n\r\n");

  alpaca_request_.client_id = kResetClientId;

  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, long_request),
            EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge);
  EXPECT_EQ(alpaca_request_.client_id, kResetClientId);
  EXPECT_THAT(long_request, StartsWith(long_value));
}

TEST_F(RequestDecoderTest, WithoutListenerSkipsValueOfUnknownHeader) {
  // If the decoder encounters an unknown header, and has a listener with
  // support for receiving unknown headers, it will attempt to deliver the name
  // and value in separate calls to the listener. However, if the value is too
  // long, then the caller and decoder will not be able to store it in a single
  // buffer, at which point kHttpRequestHeaderFieldsTooLarge should be returned
  // as the status.
  // OTOH, if there is no listener setup to receive unknown headers, the value
  // will just be skipped.
  std::string request =
      absl::StrCat("GET /api/v1/safetymonitor/1/issafe HTTP/1.1\r\n",
                   "Foo-Bar-Baz:", std::string(kDecodeBufferSize + 1, ' '),
                   std::string(kDecodeBufferSize + 1, 'X'),
                   std::string(kDecodeBufferSize + 1, ' '), "\r\n\r\n");
  alpaca_request_.client_id = kResetClientId;

  MaybeExpectUnknownHeaderName("Foo-Bar-Baz");
  if (HasListener() && TAS_ENABLE_UNKNOWN_HEADER_DECODING != 0) {
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
              EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge);
    EXPECT_THAT(request, StartsWith("X"));
  } else {
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
              EHttpStatusCode::kHttpOk);
    EXPECT_THAT(request, IsEmpty());
  }

  EXPECT_EQ(alpaca_request_.client_id, kResetClientId);
}

TEST_F(RequestDecoderTest, RejectsUnsupportedHttpMethod) {
  const std::string request_after_method =
      "/api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "Content-Length: 0\r\n"
      "\r\n";

  for (std::string method :
       {"CONNECT", "DELETE", "OPTIONS", "PATCH", "POST", "TRACE"}) {
    const std::string full_request = method + " " + request_after_method;
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
              EHttpStatusCode::kHttpNotImplemented);
    EXPECT_THAT(request, EndsWith(request_after_method));
  }
}

TEST_F(RequestDecoderTest, WhitespaceBeforeHttpMethod) {
  const std::string request_after_method =
      "/api/v1/safetymonitor/1/issafe HTTP/1.1\r\n"
      "Content-Length: 0\r\n"
      "\r\n";

  for (const std::string prefix : {" ", "\r\n", "\t", "  ", "\n"}) {
    for (const std::string method : {"GET", "PUT", "HEAD", "CONNECT", "DELETE",
                                     "OPTIONS", "PATCH", "POST", "TRACE"}) {
      const std::string full_request =
          prefix + method + " " + request_after_method;
      auto request = full_request;
      EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
                EHttpStatusCode::kHttpBadRequest);
      EXPECT_THAT(request, EndsWith(method + " " + request_after_method));
    }
  }
}

TEST_F(RequestDecoderTest, RejectsUnsupportedAscomMethod) {
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
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
              EHttpStatusCode::kHttpBadRequest)
        << "\nfull_request: " << absl::CHexEscape(full_request);
    EXPECT_THAT(full_request, EndsWith(request));
    EXPECT_THAT(request, EndsWith(request_after_ascom_method));
  }
}

TEST_F(RequestDecoderTest, NotFoundPaths) {
  for (const auto& path : {
           "/api",
           "/api/",
           "/api/v1",
           "/api/v1/",
           "/api/v1/safetymonitor",
           "/api/v1/safetymonitor/",
           "/api/v1/safetymonitor/1",
           "/api/v1/safetymonitor/2/name/",
           "/api/v1/safetymonitor/3/name/something",
           "/api/v2",
           "/api/v2/",
           "/management",
           "/management/",
           "/management/imtheboss",
           "/management/apiversions/",
           "/management/v1",
           "/management/v1/",
           "/management/v1/description/",
           "/management/v1/other",
           "/setup/",
           "/setup/v1",
           "/setup/v1/",
           "/setup/v1/laserbeam",
           "/setup/v1/safetymonitor/1",
           "/setup/v1/safetymonitor/2/setup/",
           "/setup/v1/safetymonitor/3/setup/something",
           "/setup/v2",
           "/setup/v2/",
       }) {
    const auto full_request = absl::StrCat("GET ", path, " HTTP/1.1\r\n\r\n");
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
              EHttpStatusCode::kHttpBadRequest)
        << "\nfull_request: " << absl::CHexEscape(full_request);
    EXPECT_THAT(full_request, EndsWith(request));
  }
}

TEST_F(RequestDecoderTest, MethodNotAllowedPaths) {
  for (const auto& path : {
           "/management/",
           "/setup",
           "/setup/",
       }) {
    const auto full_request = absl::StrCat("PUT ", path, " HTTP/1.1\r\n\r\n");
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
              EHttpStatusCode::kHttpMethodNotAllowed)
        << "\nfull_request: " << absl::CHexEscape(full_request);
    EXPECT_THAT(full_request, EndsWith(request));
  }
}

TEST_F(RequestDecoderTest, RejectsInvalidPathStart) {
  const std::string request_after_path_start =
      "safetymonitor/1/issafe HTTP/1.1\r\n"
      "Content-Length: 0\r\n"
      "\r\n";

  for (std::string bogus_path_start : {
           // Cases missing ' ' after method:
           "*",  // "GET*safetymonitor"
           "/",  // "GET/safetymonitor"

           // Cases missing '/' after ' ':
           " ",   // "GET safetymonitor"
           " *",  // "GET *safetymonitor"
       }) {
    const std::string full_request =
        "GET" + bogus_path_start + request_after_path_start;
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
              EHttpStatusCode::kHttpBadRequest);
    EXPECT_THAT(full_request, EndsWith(request));
    EXPECT_THAT(request, EndsWith(request_after_path_start));
  }

  // Cases where the path after the leading slash is not valid.
  for (std::string bogus_path_start : {
           " //api/v1/",  // "GET //api/v1/safetymonitor" (extra "/")
           " /api//v1/",  // "GET /api//v1/safetymonitor" (extra "/")
           " /api/v2/",   // "GET /api/v2/safetymonitor" (wrong version)
           " /API/v1/",   // "GET /API/v1/safetymonitor" (wrong case)
       }) {
    const std::string full_request =
        "GET" + bogus_path_start + request_after_path_start;
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
              EHttpStatusCode::kHttpBadRequest)
        << "\nfull_request: " << absl::CHexEscape(full_request);
    EXPECT_THAT(full_request, EndsWith(request));
    EXPECT_THAT(request, EndsWith(request_after_path_start));
  }
}

TEST_F(RequestDecoderTest, RejectsUnknownOrMalformedDeviceType) {
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
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
              EHttpStatusCode::kHttpBadRequest);
    EXPECT_THAT(full_request, EndsWith(request));
    EXPECT_THAT(request, EndsWith(request_after_device_number));
  }
}

TEST_F(RequestDecoderTest, RejectsUnsupportedHttpVersion) {
  std::string request(
      "GET /api/v1/safetymonitor/0/name HTTP/1.0\r\n"
      "\r\n");
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpVersionNotSupported);
}

TEST_F(RequestDecoderTest, RejectsMissingParamName) {
  std::string request(
      "GET /api/v1/safetymonitor/0/name?=1 HTTP/1.1\r\n"
      "\r\n");
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpBadRequest);
}

TEST_F(RequestDecoderTest, RejectsInvalidParamNameValueSeparator) {
  std::string request(
      "GET /api/v1/safetymonitor/0/name?ClientId:1 HTTP/1.1\r\n"
      "\r\n");
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpBadRequest);
}

TEST_F(RequestDecoderTest, RejectsInvalidParamSeparator) {
  std::string request(
      "GET /api/v1/safetymonitor/0/name?ClientId=1] HTTP/1.1\r\n"
      "\r\n");
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpBadRequest);
}

TEST_F(RequestDecoderTest, MissingHeaderName) {
  std::string request(
      "GET /api/v1/safetymonitor/0/name HTTP/1.1\r\n"
      ": 123\r\n"
      "\r\n");

  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpBadRequest);
  EXPECT_THAT(request, HasSubstr(" 123"));
}

TEST_F(RequestDecoderTest, BadHeaderNameEnd) {
  std::string request(
      "GET /api/v1/safetymonitor/0/name HTTP/1.1\r\n"
      "Content-Length : ");

  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpBadRequest);
  EXPECT_EQ(request, " : ");
}

TEST_F(RequestDecoderTest, BadHeaderLineEnd) {
  const std::string full_request(
      "PUT /api/v1/safetymonitor/0/connected HTTP/1.1\r\n"
      "Content-Length: 10\n\r"
      "\r\n"
      "abc=123456");

  for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
    auto result = DecodePartitionedRequest(decoder_, partition);

    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    EXPECT_EQ(status, EHttpStatusCode::kHttpBadRequest);
    EXPECT_EQ(remainder, "\n\r\r\nabc=123456");
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::PUT);
    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kSafetyMonitor);
    EXPECT_EQ(alpaca_request_.device_number, 0);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kConnected);

    if (TestHasFailed()) {
      return;
    }
  }
}

TEST_F(RequestDecoderTest, ContentTypeUnsupported) {
  const std::string full_request(
      "PUT /api/v1/safetymonitor/0/connected HTTP/1.1\r\n"
      "Content-Length: 0\r\n"
      "Content-Type: application/json\r\n"
      "Accept: application/json\r\n"
      "\r\n");
  {
    // Confirm that the decoder overrides the status if the listener doesn't
    // return an error status by returning kContinueDecoding.
    MaybeExpectExtraHeader(EHttpHeader::kContentType, "application/json",
                           EHttpStatusCode::kContinueDecoding);

    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
              EHttpStatusCode::kHttpUnsupportedMediaType);
    VerifyAndClearListenerExpectations();
  }

  {
    // But the decoder will return an error status returned by the listener.
    const auto expected_status =
        MaybeExpectExtraHeader(EHttpHeader::kContentType, "application/json",
                               EHttpStatusCode::kHttpMethodNotAllowed,
                               EHttpStatusCode::kHttpUnsupportedMediaType);

    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request), expected_status);
    VerifyAndClearListenerExpectations();
  }
}

TEST_F(RequestDecoderTest, CallsOnExtraHeaderIfEnabled) {
  const std::string full_request(
      "GET /api/v1/safetymonitor/0/supportedactions HTTP/1.1\r\n"
      "Content-Length:0\r\n"
      "Date:  Star Wars Day  \r\n"
      "\r\n");
  {
    // First time return no error.
    MaybeExpectExtraHeader(EHttpHeader::kDate, "Star Wars Day");
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
              EHttpStatusCode::kHttpOk);
    VerifyAndClearListenerExpectations();
  }

  // Try again, but this time return an error code from the listener (if it
  // exists).
  if (HasListener()) {
    const auto expected_status =
        MaybeExpectExtraHeader(EHttpHeader::kDate, "Star Wars Day",
                               EHttpStatusCode::kHttpUnsupportedMediaType);
    auto request = full_request;
    EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request), expected_status);
    VerifyAndClearListenerExpectations();
  }
}

TEST_F(RequestDecoderTest, CallsOnExtraOrUnknownHeaderIfEnabled) {
  const std::string full_request(
      "GET /api/v1/safetymonitor/0/connected HTTP/1.1\r\n"
      "Date: today\r\n"
      "Accept-Encoding: deflate\r\n"
      "\r\n");

  // OK if the listener says "continue decoding".
  MaybeExpectExtraHeader(EHttpHeader::kDate, "today");
  MaybeExpectUnknownHeader("Accept-Encoding", "deflate");

  auto request = full_request;
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpOk);
}

TEST_F(RequestDecoderTest, StopsIfListenerRejectsExtraHeader) {
  const std::string full_request(
      "GET /api/v1/safetymonitor/0/connected HTTP/1.1\r\n"
      "Accept-Encoding: deflate\r\n"
      "Date: today\r\n"
      "\r\n");

  InSequence seq;
  MaybeExpectUnknownHeader("Accept-Encoding", "deflate");
  const auto expected_status = MaybeExpectExtraHeader(
      EHttpHeader::kDate, "today", EHttpStatusCode::kHttpPayloadTooLarge);

  auto request = full_request;
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request), expected_status);
}

TEST_F(RequestDecoderTest, ListenerRejectsUnknownHeaderName) {
  std::string request(
      "GET /api/v1/safetymonitor/0/connected HTTP/1.1\r\n"
      "Date: today\r\n"
      "Accept-Encoding: deflate\r\n"
      "\r\n");

  InSequence seq;
  MaybeExpectExtraHeader(EHttpHeader::kDate, "today");
  auto expected_status = MaybeExpectUnknownHeaderName(
      "Accept-Encoding", EHttpStatusCode::kHttpNotAcceptable,
      EHttpStatusCode::kHttpOk);

  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request), expected_status);
}

TEST_F(RequestDecoderTest, ListenerRejectsUnknownHeaderValue) {
  std::string request(
      "GET /api/v1/safetymonitor/0/connected HTTP/1.1\r\n"
      "Date: today\r\n"
      "Accept-Encoding: deflate\r\n"
      "\r\n");

  InSequence seq;
  MaybeExpectExtraHeader(EHttpHeader::kDate, "today");
  MaybeExpectUnknownHeaderName("Accept-Encoding");
  auto expected_status = MaybeExpectUnknownHeaderValue(
      "deflate", EHttpStatusCode::kHttpNotAcceptable, EHttpStatusCode::kHttpOk);

  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request), expected_status);
}

TEST_F(RequestDecoderTest, ListenerAcceptsUnknownHeader) {
  std::string request(
      "GET /api/v1/safetymonitor/0/connected HTTP/1.1\r\n"
      "Date: today\r\n"
      "Accept-Encoding: deflate\r\n"
      "\r\n");

  InSequence seq;
  MaybeExpectExtraHeader(EHttpHeader::kDate, "today");
  MaybeExpectUnknownHeaderName("Accept-Encoding");
  MaybeExpectUnknownHeaderValue("deflate");
  EXPECT_EQ(ResetAndDecodeFullBuffer(decoder_, request),
            EHttpStatusCode::kHttpOk);
}

TEST_F(RequestDecoderTest, SwitchDeviceRequests) {
  {
    const std::string full_request(
        "GET /api/v1/switch/9999/getswitchvalue"
        "?ClientID=123&clienttransactionid=432&Id=789 "
        "HTTP/1.1\r\n"
        "Content-Length: 0\r\n"
        "\r\n");
    for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
      auto result = DecodePartitionedRequest(decoder_, partition);

      const EHttpStatusCode status = std::get<0>(result);
      const std::string buffer = std::get<1>(result);
      const std::string remainder = std::get<2>(result);

      EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
      EXPECT_THAT(buffer, IsEmpty());
      EXPECT_THAT(remainder, IsEmpty());
      EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::GET);
      EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kSwitch);
      EXPECT_EQ(alpaca_request_.device_number, 9999);
      EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kGetSwitchValue);
      EXPECT_TRUE(alpaca_request_.have_client_id);
      EXPECT_TRUE(alpaca_request_.have_client_transaction_id);
      EXPECT_TRUE(alpaca_request_.have_id);
      EXPECT_FALSE(alpaca_request_.have_state);
      EXPECT_FALSE(alpaca_request_.have_value);
      EXPECT_EQ(alpaca_request_.client_id, 123);
      EXPECT_EQ(alpaca_request_.client_transaction_id, 432);
      EXPECT_EQ(alpaca_request_.id, 789);

      if (TestHasFailed()) {
        return;
      }
    }
  }
  {
    const std::string body =
        "state=false&id=9&clienttransactionid=8&clientid=7";

    const std::string full_request = absl::StrCat(
        "PUT /api/v1/switch/0/setswitch "
        "HTTP/1.1\r\n",
        "content-TYPE:application/x-www-form-urlencoded\r\n",
        "Content-Length:", body.size(), "\r\n",  // Last header line.
        "\r\n",  // End of headers, separator between message header and body.
        body);

    LOG(INFO) << "full_request:\n" << full_request << "\n";

    for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
      auto result = DecodePartitionedRequest(decoder_, partition);

      const EHttpStatusCode status = std::get<0>(result);
      const std::string buffer = std::get<1>(result);
      const std::string remainder = std::get<2>(result);

      EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
      EXPECT_THAT(buffer, IsEmpty());
      EXPECT_THAT(remainder, IsEmpty());
      EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::PUT);
      EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kSwitch);
      EXPECT_EQ(alpaca_request_.device_number, 0);
      EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kSetSwitch);
      EXPECT_TRUE(alpaca_request_.have_client_id);
      EXPECT_TRUE(alpaca_request_.have_client_transaction_id);
      EXPECT_TRUE(alpaca_request_.have_id);
      EXPECT_TRUE(alpaca_request_.have_state);
      EXPECT_FALSE(alpaca_request_.have_value);
      EXPECT_EQ(alpaca_request_.client_id, 7);
      EXPECT_EQ(alpaca_request_.client_transaction_id, 8);
      EXPECT_EQ(alpaca_request_.id, 9);
      EXPECT_EQ(alpaca_request_.state, false);

      if (TestHasFailed()) {
        return;
      }
    }
  }
  {
    const std::string body =
        "value=0.99999&id=0&clienttransactionid=9&clientid=7";

    const std::string full_request = absl::StrCat(
        "PUT /api/v1/switch/0/setswitchvalue "
        "HTTP/1.1\r\n",
        "content-TYPE:application/x-www-form-urlencoded\r\n",
        "Content-Length:", body.size(), "\r\n",  // Last header line.
        "\r\n",  // End of headers, separator between message header and body.
        body);

    LOG(INFO) << "full_request:\n" << full_request << "\n";

    for (auto partition : GenerateMultipleRequestPartitions(full_request)) {
      auto result = DecodePartitionedRequest(decoder_, partition);

      const EHttpStatusCode status = std::get<0>(result);
      const std::string buffer = std::get<1>(result);
      const std::string remainder = std::get<2>(result);

      EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
      EXPECT_THAT(buffer, IsEmpty());
      EXPECT_THAT(remainder, IsEmpty());
      EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::PUT);
      EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kSwitch);
      EXPECT_EQ(alpaca_request_.device_number, 0);
      EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kSetSwitchValue);
      EXPECT_TRUE(alpaca_request_.have_client_id);
      EXPECT_TRUE(alpaca_request_.have_client_transaction_id);
      EXPECT_TRUE(alpaca_request_.have_id);
      EXPECT_TRUE(alpaca_request_.have_value);
      EXPECT_FALSE(alpaca_request_.have_state);
      EXPECT_EQ(alpaca_request_.client_id, 7);
      EXPECT_EQ(alpaca_request_.client_transaction_id, 9);
      EXPECT_EQ(alpaca_request_.id, 0);
      EXPECT_EQ(alpaca_request_.value, 0.99999);

      if (TestHasFailed()) {
        return;
      }
    }
  }
}

// This is for helping debug the handling of partitioned requests, both by the
// decoder, and also by the test infrastructure. Ideally we'd have a way to
// divert the logs elsewhere for this test so they don't swamp the log file.
TEST_F(RequestDecoderTest, DISABLED_VerboseLogging) {
  absl::SetFlag(&FLAGS_v, 10);

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

    auto result = DecodePartitionedRequest(decoder_, partition);
    const EHttpStatusCode status = std::get<0>(result);
    const std::string buffer = std::get<1>(result);
    const std::string remainder = std::get<2>(result);

    EXPECT_EQ(status, EHttpStatusCode::kHttpOk);
    EXPECT_THAT(buffer, IsEmpty());
    EXPECT_THAT(remainder, IsEmpty());
    EXPECT_EQ(alpaca_request_.http_method, EHttpMethod::PUT);
    EXPECT_EQ(alpaca_request_.device_type, EDeviceType::kSafetyMonitor);
    EXPECT_EQ(alpaca_request_.device_number, 9999);
    EXPECT_EQ(alpaca_request_.device_method, EDeviceMethod::kConnected);
    EXPECT_TRUE(alpaca_request_.have_client_id);
    EXPECT_TRUE(alpaca_request_.have_client_transaction_id);
    EXPECT_EQ(alpaca_request_.client_id, 321);
    EXPECT_EQ(alpaca_request_.client_transaction_id, 9);

    if (TestHasFailed()) {
      return;
    }
  }
}

}  // namespace
}  // namespace test
}  // namespace alpaca
