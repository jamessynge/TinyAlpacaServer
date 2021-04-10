#include "device_type_adapters/observing_conditions_adapter.h"

#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "literals.h"

namespace alpaca {

ObservingConditionsAdapter::ObservingConditionsAdapter(
    const DeviceInfo& device_info)
    : DeviceImplBase(device_info) {}

// Handle a GET 'request', write the HTTP response message to out.
bool ObservingConditionsAdapter::HandleGetRequest(const AlpacaRequest& request,
                                                  Print& out) {
  switch (request.device_method) {
    case EDeviceMethod::kAveragePeriod:
      return WriteResponse::StatusOrFloatResponse(request, GetAveragePeriod(),
                                                  out);
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
      // Requires a sensor name.
      if (request.sensor_name == ESensorName::kUnknown) {
        return WriteResponse::AscomErrorResponse(
            request, ErrorCodes::InvalidValue().code(),
            Literals::SensorNameMissing(), out);
      }
      return WriteResponse::StatusOrLiteralResponse(
          request, GetSensorDescription(request.sensor_name), out);
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
    case EDeviceMethod::kTimeSinceLastUpdate:
      // Requires a sensor name.
      if (request.sensor_name == ESensorName::kUnknown) {
        return WriteResponse::AscomErrorResponse(
            request, ErrorCodes::InvalidValue().code(),
            Literals::SensorNameMissing(), out);
      }
      return WriteResponse::StatusOrFloatResponse(
          request, GetTimeSinceLastUpdate(request.sensor_name), out);
    case EDeviceMethod::kWindDirection:
      return WriteResponse::StatusOrFloatResponse(request, GetWindDirection(),
                                                  out);
    case EDeviceMethod::kWindGust:
      return WriteResponse::StatusOrFloatResponse(request, GetWindGust(), out);
    case EDeviceMethod::kWindSpeed:
      return WriteResponse::StatusOrFloatResponse(request, GetWindSpeed(), out);
    default:
      return DeviceImplBase::HandleGetRequest(request, out);
  }
}

// Handle a PUT 'request', write the HTTP response message to out.
bool ObservingConditionsAdapter::HandlePutRequest(const AlpacaRequest& request,
                                                  Print& out) {
  return DeviceImplBase::HandlePutRequest(request, out);
}

StatusOr<float> ObservingConditionsAdapter::GetAveragePeriod() {
  return average_period_;
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
StatusOr<Literal> ObservingConditionsAdapter::GetSensorDescription(
    ESensorName sensor_name) {
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
StatusOr<float> ObservingConditionsAdapter::GetTimeSinceLastUpdate(
    ESensorName sensor_name) {
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

Status ObservingConditionsAdapter::SetAveragePeriod(float hours) {
  return ErrorCodes::ActionNotImplemented();
}

Status ObservingConditionsAdapter::Refresh() {
  return ErrorCodes::ActionNotImplemented();
}

}  // namespace alpaca
