#include "device_types/observing_conditions/observing_conditions_adapter.h"

#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "literals.h"
#include "utils/logging.h"
#include "utils/status.h"

namespace alpaca {

ObservingConditionsAdapter::ObservingConditionsAdapter(
    const DeviceInfo& device_info)
    : DeviceImplBase(device_info) {
  TAS_DCHECK_EQ(device_info.device_type, EDeviceType::kObservingConditions);
}

// Handle a GET 'request', write the HTTP response message to out.
bool ObservingConditionsAdapter::HandleGetRequest(const AlpacaRequest& request,
                                                  Print& out) {
  switch (request.device_method) {
    case EDeviceMethod::kAveragePeriod:
      return WriteResponse::StatusOrDoubleResponse(request, GetAveragePeriod(),
                                                   out);

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
      // Requires a sensor name.
      if (request.sensor_name == ESensorName::kUnknown) {
        return WriteResponse::AscomParameterMissingErrorResponse(
            request, Literals::SensorName(), out);
      }
      return WriteResponse::StatusOrLiteralResponse(
          request, GetSensorDescription(request.sensor_name), out);

    case EDeviceMethod::kSkyBrightness:
      return WriteResponse::StatusOrDoubleResponse(request, GetSkyBrightness(),
                                                   out);

    case EDeviceMethod::kSkyQuality:
      return WriteResponse::StatusOrDoubleResponse(request, GetSkyQuality(),
                                                   out);

    case EDeviceMethod::kSkyTemperature:
      return WriteResponse::StatusOrDoubleResponse(request, GetSkyTemperature(),
                                                   out);

    case EDeviceMethod::kStarFWHM:
      return WriteResponse::StatusOrDoubleResponse(request, GetStarFWHM(), out);

    case EDeviceMethod::kTemperature:
      return WriteResponse::StatusOrDoubleResponse(request, GetTemperature(),
                                                   out);

    case EDeviceMethod::kTimeSinceLastUpdate:
      // Requires a sensor name.
      if (request.sensor_name == ESensorName::kUnknown) {
        return WriteResponse::AscomParameterMissingErrorResponse(
            request, Literals::SensorName(), out);
      }
      return WriteResponse::StatusOrDoubleResponse(
          request, GetTimeSinceLastUpdate(request.sensor_name), out);

    case EDeviceMethod::kWindDirection:
      return WriteResponse::StatusOrDoubleResponse(request, GetWindDirection(),
                                                   out);

    case EDeviceMethod::kWindGust:
      return WriteResponse::StatusOrDoubleResponse(request, GetWindGust(), out);

    case EDeviceMethod::kWindSpeed:
      return WriteResponse::StatusOrDoubleResponse(request, GetWindSpeed(),
                                                   out);

    default:
      return DeviceImplBase::HandleGetRequest(request, out);
  }
}

StatusOr<double> ObservingConditionsAdapter::GetAveragePeriod() {
  if (MaxAveragePeriod() == 0) {
    return 0;
  } else {
    TAS_DCHECK(false) << TAS_FLASHSTR(
        "GetAveragePeriod must be overridden if MaxAveragePeriod is not 0.");
    return ErrorCodes::ActionNotImplemented();
  }
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
StatusOr<Literal> ObservingConditionsAdapter::GetSensorDescription(
    ESensorName sensor_name) {
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
StatusOr<double> ObservingConditionsAdapter::GetStarFWHM() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetTemperature() {
  return ErrorCodes::ActionNotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetTimeSinceLastUpdate(
    ESensorName sensor_name) {
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

////////////////////////////////////////////////////////////////////////////////

// Handle a PUT 'request', write the HTTP response message to out.
bool ObservingConditionsAdapter::HandlePutRequest(const AlpacaRequest& request,
                                                  Print& out) {
  switch (request.device_method) {
    case EDeviceMethod::kAveragePeriod:
      return HandlePutAveragePeriod(request, out);

    case EDeviceMethod::kRefresh:
      return HandlePutRefresh(request, out);

    default:
      return DeviceImplBase::HandlePutRequest(request, out);
  }
}

bool ObservingConditionsAdapter::HandlePutAveragePeriod(
    const AlpacaRequest& request, Print& out) {
  // Requires the AveragePeriod parameter.
  if (!request.have_average_period) {
    return WriteResponse::AscomParameterMissingErrorResponse(
        request, Literals::AveragePeriod(), out);
  }
  if (request.average_period < 0 ||
      MaxAveragePeriod() < request.average_period) {
    return WriteResponse::AscomParameterInvalidErrorResponse(
        request, Literals::AveragePeriod(), out);
  }
  return WriteResponse::StatusResponse(
      request, SetAveragePeriod(request.average_period), out);
}

double ObservingConditionsAdapter::MaxAveragePeriod() const { return 0; }

Status ObservingConditionsAdapter::SetAveragePeriod(double hours) {
  if (MaxAveragePeriod() == 0) {
    TAS_DCHECK_EQ(hours, 0);
    return OkStatus();
  } else {
    TAS_DCHECK(false) << TAS_FLASHSTR(
        "SetAveragePeriod must be overridden if MaxAveragePeriod is not 0.");
    return ErrorCodes::ActionNotImplemented();
  }
}

bool ObservingConditionsAdapter::HandlePutRefresh(const AlpacaRequest& request,
                                                  Print& out) {
  return WriteResponse::StatusResponse(request, Refresh(), out);
}

Status ObservingConditionsAdapter::Refresh() {
  return ErrorCodes::ActionNotImplemented();
}

}  // namespace alpaca
