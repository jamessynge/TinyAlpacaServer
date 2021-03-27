// TODO(jamessynge): Describe why this file exists/what it provides.

#include "device_type_adapters/observing_conditions_adapter.h"

#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "literals.h"
#include "utils/platform.h"

namespace alpaca {

ObservingConditionsAdapter::ObservingConditionsAdapter(
    const DeviceInfo& device_info)
    : DeviceApiHandlerBase(device_info) {}

// Handle a GET 'request', write the HTTP response message to out.
bool ObservingConditionsAdapter::HandleGetRequest(const AlpacaRequest& request,
                                                  Print& out) {
  switch (request.device_method) {
    case EDeviceMethod::kAveragePeriod:
      return WriteResponse::BoolResponse(request, GetConnected(), out);

    case EDeviceMethod::kCloudCover:
      return WriteResponse::StatusOrFloatResponse(request, GetCloudCover(),
                                                  out);
    case EDeviceMethod::kDewPoint:
      return WriteResponse::StatusOrFloatResponse(request, GetDewPoint(), out);
    case EDeviceMethod::kHumidity:
      return WriteResponse::StatusOrFloatResponse(request, GetHumidity(), out);
    case EDeviceMethod::kPressure:
      return WriteResponse::StatusOrFloatResponse(request, GetPressure(), out);
    case EDeviceMethod::kRainRate:
      return WriteResponse::StatusOrFloatResponse(request, GetRainRate(), out);
    case EDeviceMethod::kSensorDescription:
      return HandleGetSensorDescription(request, out);
    case EDeviceMethod::kSkyBrightness:
      return WriteResponse::StatusOrFloatResponse(request, GetSkyBrightness(),
                                                  out);
    case EDeviceMethod::kSkyQuality:
      return WriteResponse::StatusOrFloatResponse(request, GetSkyQuality(),
                                                  out);
    case EDeviceMethod::kSkyTemperature:
      return WriteResponse::StatusOrFloatResponse(request, GetSkyTemperature(),
                                                  out);
    case EDeviceMethod::kStarFullWidthHalfMax:
      return WriteResponse::StatusOrFloatResponse(
          request, GetStarFullWidthHalfMax(), out);
    case EDeviceMethod::kTemperature:
      return WriteResponse::StatusOrFloatResponse(request, GetTemperature(),
                                                  out);
    case EDeviceMethod::kWindDirection:
      return WriteResponse::StatusOrFloatResponse(request, GetWindDirection(),
                                                  out);
    case EDeviceMethod::kWindGust:
      return WriteResponse::StatusOrFloatResponse(request, GetWindGust(), out);
    case EDeviceMethod::kWindSpeed:
      return WriteResponse::StatusOrFloatResponse(request, GetWindSpeed(), out);
    case EDeviceMethod::kTimeSinceLastUpdate:
      return WriteResponse::StatusOrFloatResponse(
          request, GetTimeSinceLastUpdate(), out);
    default:
      return DeviceApiHandlerBase::HandleGetRequest(request, out);
  }
}

// Handle a PUT 'request', write the HTTP response message to out.
bool ObservingConditionsAdapter::HandlePutRequest(const AlpacaRequest& request,
                                                  Print& out) {
  return DeviceApiHandlerBase::HandlePutRequest(request, out);
}

StatusOr<float> ObservingConditionsAdapter::GetAveragePeriod() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetCloudCover() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetDewPoint() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetHumidity() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetPressure() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetRainRate() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetSkyBrightness() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetSkyQuality() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetSkyTemperature() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetStarFullWidthHalfMax() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetTemperature() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetWindDirection() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetWindGust() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetWindSpeed() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<float> ObservingConditionsAdapter::GetTimeSinceLastUpdate() {
  return ErrorCodes::ActionNotImplemented();
}

Status ObservingConditionsAdapter::SetAveragePeriod(float value) {
  return ErrorCodes::ActionNotImplemented();
}
Status ObservingConditionsAdapter::Refresh() {
  return ErrorCodes::ActionNotImplemented();
}

bool ObservingConditionsAdapter::HandleGetSensorDescription(
    const AlpacaRequest& request, Print& out) {
#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
  if (request.extra_parameters.contains(EParameter::kSensorName)) {
    StringView sensor_name =
        request.extra_parameters.find(EParameter::kSensorName);
    if (!sensor_name.empty()) {
      return HandleGetNamedSensorDescription(request, sensor_name, out);
    }
  }
  return WriteResponse::AscomErrorResponse(request,
                                           ErrorCodes::InvalidValue().code(),
                                           Literals::SensorNameMissing(), out);
#else   // !TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
  return WriteResponse::AscomErrorResponse(
      request, ErrorCodes::ActionNotImplemented().code(),
      Literals::HttpMethodNotImplemented(), out);
#endif  // TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
}

bool ObservingConditionsAdapter::HandleGetNamedSensorDescription(
    const AlpacaRequest& request, StringView sensor_name, Print& out) {
  return WriteResponse::StatusOrStringResponse(
      request, GetSensorDescription(sensor_name), out);
}

StatusOr<Literal> ObservingConditionsAdapter::GetSensorDescription(
    StringView sensor_name) {
  return ErrorCodes::ActionNotImplemented();
}

}  // namespace alpaca
