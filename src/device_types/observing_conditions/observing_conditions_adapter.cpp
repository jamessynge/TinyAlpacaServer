#include "device_types/observing_conditions/observing_conditions_adapter.h"

#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "device_types/cover_calibrator/cover_calibrator_constants.h"
#include "literals.h"
#include "utils/inline_literal.h"
#include "utils/logging.h"
#include "utils/printable_cat.h"
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
      return WriteDoubleOrSensorErrorResponse(request, ESensorName::kCloudCover,
                                              GetCloudCover(), out);

    case EDeviceMethod::kDewPoint:
      return WriteDoubleOrSensorErrorResponse(request, ESensorName::kDewPoint,
                                              GetDewPoint(), out);

    case EDeviceMethod::kHumidity:
      return WriteDoubleOrSensorErrorResponse(request, ESensorName::kHumidity,
                                              GetHumidity(), out);

    case EDeviceMethod::kPressure:
      return WriteDoubleOrSensorErrorResponse(request, ESensorName::kPressure,
                                              GetPressure(), out);

    case EDeviceMethod::kRainRate:
      return WriteDoubleOrSensorErrorResponse(request, ESensorName::kRainRate,
                                              GetRainRate(), out);

    case EDeviceMethod::kSensorDescription:
      // Requires a sensor name.
      if (request.sensor_name == ESensorName::kUnknown) {
        return WriteResponse::AscomParameterMissingErrorResponse(
            request, Literals::SensorName(), out);
      }
      return WriteResponse::StatusOrLiteralResponse(
          request, GetSensorDescription(request.sensor_name), out);

    case EDeviceMethod::kSkyBrightness:
      return WriteDoubleOrSensorErrorResponse(
          request, ESensorName::kSkyBrightness, GetSkyBrightness(), out);

    case EDeviceMethod::kSkyQuality:
      return WriteDoubleOrSensorErrorResponse(request, ESensorName::kSkyQuality,
                                              GetSkyQuality(), out);

    case EDeviceMethod::kSkyTemperature:
      return WriteDoubleOrSensorErrorResponse(
          request, ESensorName::kSkyTemperature, GetSkyTemperature(), out);

    case EDeviceMethod::kStarFWHM:
      return WriteDoubleOrSensorErrorResponse(request, ESensorName::kStarFWHM,
                                              GetStarFWHM(), out);

    case EDeviceMethod::kTemperature:
      return WriteDoubleOrSensorErrorResponse(
          request, ESensorName::kTemperature, GetTemperature(), out);

    case EDeviceMethod::kTimeSinceLastUpdate:
      // Requires a sensor name.
      //
      // TODO(jamessynge): Make sure that sensor name can be the empty string.
      // The spec says:
      //
      //    If an empty string is supplied as the PropertyName, the driver must
      //    return the time since the most recent update of any sensor.
      //
      // This could be accomodated in the adapter by adding a
      // GetTimeSinceLastUpdate() method that takes no argument,
      if (request.sensor_name == ESensorName::kUnknown) {
        return WriteResponse::AscomParameterMissingErrorResponse(
            request, Literals::SensorName(), out);
      }
      return WriteDoubleOrSensorErrorResponse(
          request, request.sensor_name,
          GetTimeSinceLastUpdate(request.sensor_name), out);

    case EDeviceMethod::kWindDirection:
      return WriteDoubleOrSensorErrorResponse(
          request, ESensorName::kWindDirection, GetWindDirection(), out);

    case EDeviceMethod::kWindGust:
      return WriteDoubleOrSensorErrorResponse(request, ESensorName::kWindGust,
                                              GetWindGust(), out);

    case EDeviceMethod::kWindSpeed:
      return WriteDoubleOrSensorErrorResponse(request, ESensorName::kWindSpeed,
                                              GetWindSpeed(), out);

    default:
      return DeviceImplBase::HandleGetRequest(request, out);
  }
}

StatusOr<double> ObservingConditionsAdapter::GetAveragePeriod() {
  if (MaxAveragePeriod() == 0) {
    return 0;
  } else {
    TAS_DCHECK(false) << TAS_FLASHSTR_128(
        "GetAveragePeriod should be overridden given that MaxAveragePeriod is "
        "not 0, but it has not been.");
    return ErrorCodes::NotImplemented();
  }
}
StatusOr<double> ObservingConditionsAdapter::GetCloudCover() {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetDewPoint() {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetHumidity() {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetPressure() {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetRainRate() {
  return ErrorCodes::NotImplemented();
}
StatusOr<Literal> ObservingConditionsAdapter::GetSensorDescription(
    ESensorName sensor_name) {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetSkyBrightness() {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetSkyQuality() {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetSkyTemperature() {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetStarFWHM() {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetTemperature() {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetTimeSinceLastUpdate(
    ESensorName sensor_name) {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetWindDirection() {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetWindGust() {
  return ErrorCodes::NotImplemented();
}
StatusOr<double> ObservingConditionsAdapter::GetWindSpeed() {
  return ErrorCodes::NotImplemented();
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
    TAS_DCHECK(false) << TAS_FLASHSTR_128(
        "SetAveragePeriod must be overridden if MaxAveragePeriod is not 0.");
    return ErrorCodes::NotImplemented();
  }
}

bool ObservingConditionsAdapter::HandlePutRefresh(const AlpacaRequest& request,
                                                  Print& out) {
  return WriteResponse::StatusResponse(request, Refresh(), out);
}

Status ObservingConditionsAdapter::Refresh() {
  return ErrorCodes::NotImplemented();
}

bool ObservingConditionsAdapter::WriteDoubleOrSensorErrorResponse(
    const AlpacaRequest& request, ESensorName sensor_name,
    StatusOr<double> result, Print& out) {
  if (result.ok()) {
    return WriteResponse::DoubleResponse(request, result.value(), out);
  } else if (result.status().code() == ErrorCodes::kNotImplemented) {
    return WriteSensorNotImpementedResponse(request, sensor_name, out);
  } else {
    return WriteResponse::StatusResponse(request, result.status(), out);
  }
}

bool ObservingConditionsAdapter::WriteSensorNotImpementedResponse(
    const AlpacaRequest& request, ESensorName sensor_name, Print& out) {
  auto error_message = PrintableCat(TAS_FLASHSTR("Sensor not implemented: "),
                                    ToFlashStringHelper(sensor_name));
  return WriteResponse::AscomErrorResponse(request, ErrorCodes::kNotImplemented,
                                           error_message, out);
}

}  // namespace alpaca
