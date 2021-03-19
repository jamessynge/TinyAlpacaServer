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
      return WriteResponse::StatusOrDoubleResponse(request, GetCloudCover(),
                                                   out);
    case EDeviceMethod::kDewPoint:
      return WriteResponse::StatusOrDoubleResponse(request, GetDewPoint(), out);
    case EDeviceMethod::kHumidity:
      return WriteResponse::StatusOrDoubleResponse(request, GetHumidity(), out);
    case EDeviceMethod::kPressure:
      return WriteResponse::StatusOrDoubleResponse(request, GetPressure(), out);
    case EDeviceMethod::kRainRate:
      return WriteResponse::StatusOrDoubleResponse(request, GetRainRate(), out);
    case EDeviceMethod::kSensorDescription:
      return HandleGetSensorDescription(request, out);
    case EDeviceMethod::kSkyBrightness:
      return WriteResponse::StatusOrDoubleResponse(request, GetSkyBrightness(),
                                                   out);
    case EDeviceMethod::kSkyQuality:
      return WriteResponse::StatusOrDoubleResponse(request, GetSkyQuality(),
                                                   out);
    case EDeviceMethod::kSkyTemperature:
      return WriteResponse::StatusOrDoubleResponse(request, GetSkyTemperature(),
                                                   out);
    case EDeviceMethod::kStarFullWidthHalfMax:
      return WriteResponse::StatusOrDoubleResponse(
          request, GetStarFullWidthHalfMax(), out);
    case EDeviceMethod::kTemperature:
      return WriteResponse::StatusOrDoubleResponse(request, GetTemperature(),
                                                   out);
    case EDeviceMethod::kWindDirection:
      return WriteResponse::StatusOrDoubleResponse(request, GetWindDirection(),
                                                   out);
    case EDeviceMethod::kWindGust:
      return WriteResponse::StatusOrDoubleResponse(request, GetWindGust(), out);
    case EDeviceMethod::kWindSpeed:
      return WriteResponse::StatusOrDoubleResponse(request, GetWindSpeed(),
                                                   out);
    case EDeviceMethod::kTimeSinceLastUpdate:
      return WriteResponse::StatusOrDoubleResponse(
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

StatusOr<double> ObservingConditionsAdapter::GetAveragePeriod() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetCloudCover() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetDewPoint() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetHumidity() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetPressure() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetRainRate() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetSkyBrightness() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetSkyQuality() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetSkyTemperature() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetStarFullWidthHalfMax() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetTemperature() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetWindDirection() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetWindGust() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetWindSpeed() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetTimeSinceLastUpdate() {
  return ErrorCodes::ActionNotImplemented();
}

Status ObservingConditionsAdapter::SetAveragePeriod(double value) {
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
