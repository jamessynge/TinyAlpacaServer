#include "extras/test_tools/decode_and_dispatch_test_base.h"

#include <McuCore.h>

#include <memory>
#include <string>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "device_interface.h"
#include "extras/test_tools/test_tiny_alpaca_server.h"
#include "mcucore/extras/test_tools/http_request.h"
#include "mcucore/extras/test_tools/json_decoder.h"
#include "util/task/status_macros.h"

namespace alpaca {
namespace test {
namespace {

using ::mcucore::test::HttpRequest;
using ::mcucore::test::JsonValue;

constexpr int kDeviceNumber = 87405;
constexpr int kClientId = 91240;

constexpr char kClientTransactionIDName[] = "ClientTransactionID";
constexpr char kServerTransactionIDName[] = "ServerTransactionID";
constexpr char kErrorNumberName[] = "ErrorNumber";
constexpr char kErrorMessageName[] = "ErrorMessage";

const ServerDescription kServerDescription  // NOLINT
    {
        .server_name = MCU_FLASHSTR("OurServer"),
        .manufacturer = MCU_FLASHSTR("Us"),
        .manufacturer_version = MCU_FLASHSTR("0.0.1"),
        .location = MCU_FLASHSTR("Right Here"),
    };

}  // namespace

DecodeAndDispatchTestBase::DecodeAndDispatchTestBase()
    : mock_platform_network_lifetime_(
          std::make_unique<mcunet::test::MockPlatformNetwork>()) {}

void DecodeAndDispatchTestBase::AddDeviceInterface(
    DeviceInterface& device_interface) {
  MCU_CHECK(server_ == nullptr);
  device_interfaces_.push_back(&device_interface);
}

void DecodeAndDispatchTestBase::SetUp() {
  PrepareEeprom();
  server_ = CreateServer();
  if (InitializeServerAutomatically()) {
    EXPECT_TRUE(server_->Initialize());
    EXPECT_FALSE(server_->connection_is_open());
  }
}

void DecodeAndDispatchTestBase::PrepareEeprom() {
  mcucore::EepromTlv::ClearAndInitializeEeprom();
}

const ServerDescription& DecodeAndDispatchTestBase::GetServerDescription() {
  return kServerDescription;
}

std::unique_ptr<TestTinyAlpacaServer>
DecodeAndDispatchTestBase::CreateServer() {
  return std::make_unique<TestTinyAlpacaServer>(GetServerDescription(),
                                                GetDeviceInterfaces());
}

mcucore::ArrayView<DeviceInterface*>
DecodeAndDispatchTestBase::GetDeviceInterfaces() {
  return mcucore::ArrayView<DeviceInterface*>(device_interfaces_.data(),
                                              device_interfaces_.size());
}

std::string_view DecodeAndDispatchTestBase::GetDeviceTypeName() {
  MCU_CHECK_EQ(device_interfaces_.size(), 1);
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
  MCU_CHECK(false) << "Unknown device type: " << v;
  return "";
}

uint32_t DecodeAndDispatchTestBase::GetDeviceNumber() {
  MCU_CHECK_EQ(device_interfaces_.size(), 1);
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
  response_validator_.SetClientTransactionIdFromRequest(request);
  return RoundTripRequest(request.ToString(), half_close_when_drained);
}

absl::StatusOr<JsonValue>
DecodeAndDispatchTestBase::RoundTripRequestWithValueResponse(
    HttpRequest& request, bool half_close_when_drained) {
  ASSIGN_OR_RETURN(auto response_message,
                   RoundTripRequest(request, half_close_when_drained));
  return response_validator_.ValidateValueResponse(response_message);
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
  response_validator_.SetClientTransactionIdFromRequest(request);
  return RoundTripSoleRequest(request.ToString());
}

absl::StatusOr<JsonValue>
DecodeAndDispatchTestBase::RoundTripSoleRequestWithValueResponse(
    HttpRequest& request) {
  ASSIGN_OR_RETURN(auto response_message, RoundTripSoleRequest(request));
  return response_validator_.ValidateValueResponse(response_message);
}

}  // namespace test
}  // namespace alpaca
