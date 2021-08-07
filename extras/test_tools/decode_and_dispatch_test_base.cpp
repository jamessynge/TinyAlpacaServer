#include "extras/test_tools/decode_and_dispatch_test_base.h"

#include <string>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "device_interface.h"
#include "extras/test_tools/http_request.h"
#include "extras/test_tools/http_response.h"
#include "extras/test_tools/json_decoder.h"
#include "extras/test_tools/test_tiny_alpaca_server.h"
#include "logging.h"
#include "util/task/status_macros.h"
#include "utils/array_view.h"

namespace alpaca {
namespace test {
namespace {

constexpr int kDeviceNumber = 87405;
constexpr int kClientId = 91240;

constexpr char kClientTransactionIDName[] = "ClientTransactionID";
constexpr char kServerTransactionIDName[] = "ServerTransactionID";
constexpr char kErrorNumberName[] = "ErrorNumber";
constexpr char kErrorMessageName[] = "ErrorMessage";

const ServerDescription kServerDescription  // NOLINT
    {
        .server_name = TAS_FLASHSTR("OurServer"),
        .manufacturer = TAS_FLASHSTR("Us"),
        .manufacturer_version = TAS_FLASHSTR("0.0.1"),
        .location = TAS_FLASHSTR("Right Here"),
    };

#define RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(json_value, json_type)             \
  if ((json_value.type()) == (json_type))                                      \
    ;                                                                          \
  else                                                                         \
    return absl::InvalidArgumentError(absl::StrCat(                            \
        "JSON value does not have the desired type (" #json_type "); value: ", \
        json_value.ToDebugString()))

}  // namespace

DecodeAndDispatchTestBase::DecodeAndDispatchTestBase() {}

void DecodeAndDispatchTestBase::AddDeviceInterface(
    DeviceInterface& device_interface) {
  TAS_CHECK(server_ == nullptr);
  device_interfaces_.push_back(&device_interface);
}

void DecodeAndDispatchTestBase::SetUp() {
  server_ = CreateServer();
  if (InitializeServerAutomatically()) {
    EXPECT_TRUE(server_->Initialize());
    EXPECT_FALSE(server_->connection_is_open());
  }
}

const ServerDescription& DecodeAndDispatchTestBase::GetServerDescription() {
  return kServerDescription;
}

std::unique_ptr<TestTinyAlpacaServer>
DecodeAndDispatchTestBase::CreateServer() {
  return std::make_unique<TestTinyAlpacaServer>(GetServerDescription(),
                                                GetDeviceInterfaces());
}

ArrayView<DeviceInterface*> DecodeAndDispatchTestBase::GetDeviceInterfaces() {
  return ArrayView<DeviceInterface*>(device_interfaces_.data(),
                                     device_interfaces_.size());
}

std::string_view DecodeAndDispatchTestBase::GetDeviceTypeName() {
  TAS_CHECK_EQ(device_interfaces_.size(), 1);
  const auto v = device_interfaces_[0]->device_type();
  switch (v) {
    case EDeviceType::kUnknown:
      break;
    case EDeviceType::kCamera:
      return "camera";
    case EDeviceType::kCoverCalibrator:
      return "covercalibrator";
    case EDeviceType::kDome:
      return "dome";
    case EDeviceType::kFilterWheel:
      return "filterwheel";
    case EDeviceType::kFocuser:
      return "focuser";
    case EDeviceType::kObservingConditions:
      return "observingconditions";
    case EDeviceType::kRotator:
      return "rotator";
    case EDeviceType::kSafetyMonitor:
      return "safetymonitor";
    case EDeviceType::kSwitch:
      return "switch";
    case EDeviceType::kTelescope:
      return "telescope";
  }
  TAS_CHECK(false) << "Unknown device type: " << v;
  return "";
}

uint32_t DecodeAndDispatchTestBase::GetDeviceNumber() {
  TAS_CHECK_EQ(device_interfaces_.size(), 1);
  return device_interfaces_[0]->device_number();
}

uint32_t DecodeAndDispatchTestBase::GetClientId() { return kClientId; }

uint32_t DecodeAndDispatchTestBase::GetNextClientTransactionId() {
  return ++last_client_transaction_id_;
}

void DecodeAndDispatchTestBase::AddCommonParts(HttpRequest& request) {
  request.SetParameter("ClientID", std::to_string(GetClientId()));
  request.SetParameter("ClientTransactionID",
                       std::to_string(GetNextClientTransactionId()));
  request.AddCommonParts();
}

HttpRequest DecodeAndDispatchTestBase::GenerateConfiguredDevicesRequest() {
  HttpRequest request("/management/v1/configureddevices");
  AddCommonParts(request);
  return request;
}

HttpRequest DecodeAndDispatchTestBase::GenerateDeviceSetupRequest() {
  HttpRequest request(absl::StrCat("/setup/v1/", GetDeviceTypeName(), "/",
                                   GetDeviceNumber(), "/setup"));
  AddCommonParts(request);
  return request;
}

HttpRequest DecodeAndDispatchTestBase::GenerateDeviceApiRequest(
    std::string_view ascom_method) {
  HttpRequest request(absl::StrCat("/api/v1/", GetDeviceTypeName(), "/",
                                   GetDeviceNumber(), "/", ascom_method));
  AddCommonParts(request);
  return request;
}

ConnectionResult DecodeAndDispatchTestBase::SendAndReceive(
    const std::string& input, bool half_close_when_drained) {
  server_->MaintainDevices();
  ConnectionResult final_result;
  EXPECT_EQ(server_->connection_is_open(), server_->connection_is_writeable());
  if (!server_->connection_is_open()) {
    final_result = server_->AnnounceConnect("");
    EXPECT_EQ(final_result.remaining_input, "");
    EXPECT_FALSE(final_result.connection_closed);
    if (final_result.connection_closed) {
      return final_result;
    }
  }
  server_->MaintainDevices();
  EXPECT_TRUE(server_->connection_is_writeable());
  auto result =
      server_->AnnounceCanRead(input,
                               /*repeat_until_stable=*/true,
                               /*peer_half_closed=*/half_close_when_drained);
  final_result.remaining_input = result.remaining_input;
  final_result.output += result.output;
  final_result.connection_closed = result.connection_closed;
  if (!final_result.connection_closed && half_close_when_drained) {
    server_->MaintainDevices();
    server_->AnnounceDisconnect();
  }
  return final_result;
}

absl::StatusOr<std::string> DecodeAndDispatchTestBase::RoundTripRequest(
    const std::string& request, bool half_close_when_drained) {
  auto result = SendAndReceive(request, half_close_when_drained);
  if (!result.remaining_input.empty()) {
    return absl::FailedPreconditionError(
        absl::StrCat("Not all of the request was read by the server: ",
                     result.remaining_input.substr(0, 30)));
  }
  return result.output;
}

absl::StatusOr<std::string> DecodeAndDispatchTestBase::RoundTripRequest(
    HttpRequest& request, bool half_close_when_drained) {
  return RoundTripRequest(request.ToString(), half_close_when_drained);
}

absl::StatusOr<std::string> DecodeAndDispatchTestBase::RoundTripSoleRequest(
    const std::string& request) {
  if (server_->connection_is_open()) {
    return absl::InternalError(
        "Connection is still open, test should close it first or call "
        "different RoundTrip method.");
  }

  auto result = server_->AnnounceConnect("");
  if (!result.remaining_input.empty() || result.connection_closed) {
    return absl::FailedPreconditionError(
        absl::StrCat("Unexpected response when opening a connection: ",
                     result.ToDebugString()));
  }
  std::string all_output = result.output;

  result = server_->AnnounceCanRead(request);
  if (result.connection_closed) {
    if (!result.remaining_input.empty()) {
      return absl::FailedPreconditionError(absl::StrCat(
          "Expected all input to be consumed: ", result.ToDebugString()));
    }
    return result.output;
  }
  all_output += result.output;

  result = server_->AnnounceHalfClosed();
  if (!result.connection_closed || !result.remaining_input.empty()) {
    return absl::FailedPreconditionError(absl::StrCat(
        "Expected all input to be consumed and the connection to be closed: ",
        result.ToDebugString()));
  }
  all_output += result.output;
  return all_output;
}

absl::StatusOr<std::string> DecodeAndDispatchTestBase::RoundTripSoleRequest(
    HttpRequest& request) {
  return RoundTripSoleRequest(request.ToString());
}

absl::StatusOr<HttpResponse> DecodeAndDispatchTestBase::ValidateResponseIsOk(
    const std::string& response_string) {
  ASSIGN_OR_RETURN(auto resp, HttpResponse::Make(response_string));
  if (resp.status_code != 200) {
    return absl::InvalidArgumentError(
        absl::StrCat("Expected status_code 200, not ", resp.status_code,
                     "; response:\n\n", response_string, "\n"));
  }
  if (resp.status_message != "OK") {
    return absl::InvalidArgumentError(
        absl::StrCat("Expected status_message OK, not ", resp.status_message,
                     "; response:\n\n", response_string, "\n"));
  }
  if (!resp.json_value.is_unset()) {
    // The body must be a JSON Object, and must have a ServerTransactionID
    // property.
    auto body_jv = resp.json_value;
    RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(body_jv, JsonValue::kObject);
    RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(
        body_jv.GetValue(kServerTransactionIDName), JsonValue::kInteger);
    // The other three Alpaca standard properties are optional.
    if (body_jv.HasKey(kClientTransactionIDName)) {
      RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(
          body_jv.GetValue(kClientTransactionIDName), JsonValue::kInteger);
    }
    if (body_jv.HasKey(kErrorNumberName)) {
      RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(body_jv.GetValue(kErrorNumberName),
                                          JsonValue::kInteger);
    }
    if (body_jv.HasKey(kErrorMessageName)) {
      RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(body_jv.GetValue(kErrorMessageName),
                                          JsonValue::kString);
    }
    // If there is an ErrorMessage,, there must be a non-zero ErrorNumber.
    if (body_jv.HasKey(kErrorMessageName) &&
        body_jv.GetValue(kErrorMessageName) != "") {
      auto error_number_jv = body_jv.GetValue(kErrorNumberName);
      if (body_jv.GetValue(kErrorNumberName) == 0) {
        return absl::InvalidArgumentError(absl::StrCat(
            "Has an ", kErrorMessageName, ", but not a non-zero ",
            kErrorNumberName, "; JSON response: ", body_jv.ToDebugString()));
      }
    }
    // The ServerTransactionID must be increasing.
    auto recv_txn_id = body_jv.GetValue(kServerTransactionIDName).as_integer();
    if (recv_txn_id < 0) {
      return absl::InvalidArgumentError(
          absl::StrCat("Expected ", kServerTransactionIDName,
                       " to be >= 0; value: ", recv_txn_id));
    }
    auto min_expected_txn_id = last_server_transaction_id_ + 1;
    last_server_transaction_id_ = recv_txn_id;
    if (min_expected_txn_id > recv_txn_id) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Expected ", kServerTransactionIDName, " >= ", min_expected_txn_id,
          "; received ", recv_txn_id));
    }
  }
  return resp;
}

absl::StatusOr<HttpResponse>
DecodeAndDispatchTestBase::ValidateJsonResponseIsOk(
    const HttpRequest& request, const std::string& response_string) {
  ASSIGN_OR_RETURN(auto resp, ValidateResponseIsOk(response_string));
  RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(resp.json_value, JsonValue::kObject);
  auto body_jv = resp.json_value;

  // Not OK if there is a non-zero error number.
  if (body_jv.HasKey(kErrorNumberName) &&
      body_jv.GetValue(kErrorNumberName).as_integer() != 0) {
    return absl::InvalidArgumentError(
        absl::StrCat("Expected no ASCOM error, got:\n", kErrorNumberName, ":  ",
                     body_jv.GetValue(kErrorNumberName).as_integer(), "\n",
                     kErrorMessageName, ": ",
                     (body_jv.HasKey(kErrorMessageName)
                          ? body_jv.GetValue(kErrorMessageName).as_string()
                          : "")));
  }
  if (request.HasParameter(kClientTransactionIDName)) {
    ASSIGN_OR_RETURN(auto sent_txn_id,
                     request.GetParameter(kClientTransactionIDName));
    auto recv_txn_id = body_jv.GetValue(kClientTransactionIDName);
    RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(recv_txn_id, JsonValue::kInteger);
    if (sent_txn_id != std::to_string(recv_txn_id.as_integer())) {
      return absl::InvalidArgumentError(
          absl::StrCat("Sent ", kClientTransactionIDName, "=", sent_txn_id,
                       ", response contained ", recv_txn_id.ToDebugString()));
    }
  }
  return resp;
}

absl::StatusOr<HttpResponse>
DecodeAndDispatchTestBase::ValidateJsonResponseHasError(
    const HttpRequest& request, const std::string& response_string,
    int expected_error_number) {
  ASSIGN_OR_RETURN(auto resp, ValidateResponseIsOk(response_string));
  RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(resp.json_value, JsonValue::kObject);
  auto body_jv = resp.json_value;
  if (body_jv.GetValue(kErrorNumberName) != expected_error_number) {
    return absl::InvalidArgumentError(
        absl::StrCat("Expected ", kErrorNumberName, "=", expected_error_number,
                     "; JSON response:\n", body_jv.ToDebugString()));
  }
  return resp;
}

absl::StatusOr<HttpResponse>
DecodeAndDispatchTestBase::ValidateValuelessResponse(
    const HttpRequest& request, const std::string& response_string) {
  ASSIGN_OR_RETURN(auto resp,
                   ValidateJsonResponseIsOk(request, response_string));
  if (resp.json_value.HasKey("Value")) {
    return absl::InvalidArgumentError(
        "Expected the JSON body object to NOT have a Value property");
  }
  return resp;
}

absl::StatusOr<JsonValue> DecodeAndDispatchTestBase::ValidateValueResponse(
    const HttpRequest& request, const std::string& response_string) {
  ASSIGN_OR_RETURN(auto resp,
                   ValidateJsonResponseIsOk(request, response_string));
  if (!resp.json_value.HasKey("Value")) {
    return absl::InvalidArgumentError(
        "Expected the JSON body object to have a Value property");
  }
  return resp.json_value.GetValue("Value");
}

absl::StatusOr<JsonValue> DecodeAndDispatchTestBase::ValidateArrayValueResponse(
    const HttpRequest& request, const std::string& response_string) {
  ASSIGN_OR_RETURN(auto value_jv,
                   ValidateValueResponse(request, response_string));
  RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(value_jv, JsonValue::kArray);
  return value_jv;
}

absl::StatusOr<std::vector<int64_t>>
DecodeAndDispatchTestBase::ValidateIntArrayResponse(
    const HttpRequest& request, const std::string& response_string) {
  ASSIGN_OR_RETURN(auto array_jv,
                   ValidateArrayValueResponse(request, response_string));
  std::vector<int64_t> result;
  for (const auto& elem_jv : array_jv.as_array()) {
    RETURN_ERROR_IF_JSON_VALUE_NOT_TYPE(elem_jv, JsonValue::kInteger);
    result.push_back((elem_jv.as_integer()));
  }
  return result;
}

}  // namespace test
}  // namespace alpaca
