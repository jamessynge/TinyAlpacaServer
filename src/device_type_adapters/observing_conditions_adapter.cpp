// TODO(jamessynge): Describe why this file exists/what it provides.

#include "device_type_adapters/observing_conditions_adapter.h"

#include "alpaca_response.h"
#include "ascom_error_codes.h"
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
      return WriteBoolResponse(request, GetConnected(), out);

    case EDeviceMethod::kCloudCover:
      return WriteDoubleResponse(request, GetCloudCover(), out);
    case EDeviceMethod::kDewPoint:
      return WriteDoubleResponse(request, GetDewPoint(), out);
    case EDeviceMethod::kHumidity:
      return WriteDoubleResponse(request, GetHumidity(), out);
    case EDeviceMethod::kPressure:
      return WriteDoubleResponse(request, GetPressure(), out);
    case EDeviceMethod::kRainRate:
      return WriteDoubleResponse(request, GetRainRate(), out);
    case EDeviceMethod::kSkyBrightness:
      return WriteDoubleResponse(request, GetSkyBrightness(), out);
    case EDeviceMethod::kSkyQuality:
      return WriteDoubleResponse(request, GetSkyQuality(), out);
    case EDeviceMethod::kSkyTemperature:
      return WriteDoubleResponse(request, GetSkyTemperature(), out);
    case EDeviceMethod::kStarFullWidthHalfMax:
      return WriteDoubleResponse(request, GetStarFullWidthHalfMax(), out);
    case EDeviceMethod::kTemperature:
      return WriteDoubleResponse(request, GetTemperature(), out);
    case EDeviceMethod::kWindDirection:
      return WriteDoubleResponse(request, GetWindDirection(), out);
    case EDeviceMethod::kWindGust:
      return WriteDoubleResponse(request, GetWindGust(), out);
    case EDeviceMethod::kWindSpeed:
      return WriteDoubleResponse(request, GetWindSpeed(), out);
    case EDeviceMethod::kTimeSinceLastUpdate:
      return WriteDoubleResponse(request, GetTimeSinceLastUpdate(), out);
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
StatusOr<Literal> ObservingConditionsAdapter::GetSensorDescription(
    StringView sensor_name) {
  return ErrorCodes::ActionNotImplemented();
}

Status ObservingConditionsAdapter::SetAveragePeriod(double value) {
  return ErrorCodes::ActionNotImplemented();
}
Status ObservingConditionsAdapter::Refresh() {
  return ErrorCodes::ActionNotImplemented();
}

}  // namespace alpaca
