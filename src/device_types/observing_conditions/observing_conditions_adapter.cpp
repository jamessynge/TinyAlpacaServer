#include "device_types/observing_conditions/observing_conditions_adapter.h"

#include <McuCore.h>

#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "literals.h"

namespace alpaca {
namespace {
void AddSensorTableRow(
    mcucore::ProgmemStringView label, mcucore::ProgmemStringView html_class,
    mcucore::StatusOr<double> status_or_value, mcucore::ProgmemStringView units,
    mcucore::StatusOr<mcucore::ProgmemStringView> status_or_description,
    mcucore::OPrintStream& strm) {
  if (!status_or_value.ok()) {
    return;
  }
  strm << MCU_PSD("<tr class=") << html_class << MCU_PSD("><td class=ocl>")
       << label << MCU_PSD("</td><td class=ocv>") << status_or_value.value()
       << MCU_PSD("&nbsp;") << units << MCU_PSD("</td><td class=ocd>");
  if (status_or_description.ok()) {
    strm << status_or_description.value();
  }
  strm << MCU_PSD("</td></tr>\n");
}
}  // namespace

ObservingConditionsAdapter::ObservingConditionsAdapter(
    ServerContext& server_context, const DeviceDescription& device_description)
    : DeviceImplBase(server_context, device_description) {
  MCU_DCHECK_EQ(device_description.device_type,
                EDeviceType::kObservingConditions);
}

ObservingConditionsAdapter::~ObservingConditionsAdapter() {}

void ObservingConditionsAdapter::AddDeviceDetails(mcucore::OPrintStream& strm) {
  strm << MCU_PSD("<div class=ocp>\n<h4>Observing Conditions Properties</h4>\n")
       << MCU_PSD("<table>\n");

  // TODO(jamessynge): Consider whether to also render TimeSinceLastUpdate for
  // each sensor.

  AddSensorTableRow(MCU_PSV("Average Period"), MCU_PSV("averageperiod"),
                    GetAveragePeriod(), MCU_PSV("hrs"),
                    MCU_PSV("Period over which sensor readings are averaged."),
                    strm);

  AddSensorTableRow(MCU_PSV("Cloud Cover"), MCU_PSV("cloudcover"),
                    GetCloudCover(), MCU_PSV("%"),
                    GetSensorDescription(ESensorName::kCloudCover), strm);

  AddSensorTableRow(MCU_PSV("Dew Point"), MCU_PSV("dewpoint"), GetDewPoint(),
                    MCU_PSV("&#8451;"),
                    GetSensorDescription(ESensorName::kDewPoint), strm);

  AddSensorTableRow(MCU_PSV("Humidity"), MCU_PSV("humidity"), GetHumidity(),
                    MCU_PSV("%"), GetSensorDescription(ESensorName::kHumidity),
                    strm);

  AddSensorTableRow(MCU_PSV("Pressure"), MCU_PSV("pressure"), GetPressure(),
                    MCU_PSV("hPa"),
                    GetSensorDescription(ESensorName::kPressure), strm);

  AddSensorTableRow(MCU_PSV("Rain Rate"), MCU_PSV("rainrate"), GetRainRate(),
                    MCU_PSV("mm/hr"),
                    GetSensorDescription(ESensorName::kRainRate), strm);

  AddSensorTableRow(MCU_PSV("Sky Brightness"), MCU_PSV("skybrightness"),
                    GetSkyBrightness(), MCU_PSV("lx"),
                    GetSensorDescription(ESensorName::kSkyBrightness), strm);

  AddSensorTableRow(MCU_PSV("Sky Quality"), MCU_PSV("skyquality"),
                    GetSkyQuality(), MCU_PSV(""),
                    GetSensorDescription(ESensorName::kSkyQuality), strm);

  AddSensorTableRow(MCU_PSV("Sky Temperature"), MCU_PSV("skytemperature"),
                    GetSkyTemperature(), MCU_PSV("&#8451;"),
                    GetSensorDescription(ESensorName::kSkyTemperature), strm);

  AddSensorTableRow(MCU_PSV("Star FWHM"), MCU_PSV("starfwhm"), GetStarFWHM(),
                    MCU_PSV("&#8243;"),
                    GetSensorDescription(ESensorName::kStarFWHM), strm);

  AddSensorTableRow(MCU_PSV("Temperature"), MCU_PSV("temperature"),
                    GetTemperature(), MCU_PSV("&#8451;"),
                    GetSensorDescription(ESensorName::kTemperature), strm);

  AddSensorTableRow(MCU_PSV("Wind Direction"), MCU_PSV("winddirection"),
                    GetWindDirection(), MCU_PSV("&#176;"),
                    GetSensorDescription(ESensorName::kWindDirection), strm);

  AddSensorTableRow(MCU_PSV("Wind Gust"), MCU_PSV("windgust"), GetWindGust(),
                    MCU_PSV("m/s"),
                    GetSensorDescription(ESensorName::kWindGust), strm);

  AddSensorTableRow(MCU_PSV("Wind Speed"), MCU_PSV("windspeed"), GetWindSpeed(),
                    MCU_PSV("m/s"),
                    GetSensorDescription(ESensorName::kWindSpeed), strm);

  strm << MCU_PSD("</table>\n</div>\n");
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
            request, ProgmemStringViews::SensorName(), out);
      }
      return WriteResponse::StatusOrProgmemStringViewResponse(
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
            request, ProgmemStringViews::SensorName(), out);
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

mcucore::StatusOr<double> ObservingConditionsAdapter::GetAveragePeriod() {
  if (MaxAveragePeriod() == 0) {
    return 0;
  } else {
    MCU_DCHECK(false) << MCU_FLASHSTR_128(
        "GetAveragePeriod should be overridden given that MaxAveragePeriod is "
        "not 0, but it has not been.");
    return ErrorCodes::NotImplemented();
  }
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetCloudCover() {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetDewPoint() {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetHumidity() {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetPressure() {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetRainRate() {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<mcucore::ProgmemStringView>
ObservingConditionsAdapter::GetSensorDescription(ESensorName sensor_name) {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetSkyBrightness() {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetSkyQuality() {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetSkyTemperature() {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetStarFWHM() {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetTemperature() {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetTimeSinceLastUpdate(
    ESensorName sensor_name) {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetWindDirection() {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetWindGust() {
  return ErrorCodes::NotImplemented();
}
mcucore::StatusOr<double> ObservingConditionsAdapter::GetWindSpeed() {
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
        request, ProgmemStringViews::AveragePeriod(), out);
  }
  if (request.average_period < 0 ||
      MaxAveragePeriod() < request.average_period) {
    return WriteResponse::AscomParameterInvalidErrorResponse(
        request, ProgmemStringViews::AveragePeriod(), out);
  }
  return WriteResponse::StatusResponse(
      request, SetAveragePeriod(request.average_period), out);
}

double ObservingConditionsAdapter::MaxAveragePeriod() const { return 0; }

mcucore::Status ObservingConditionsAdapter::SetAveragePeriod(double hours) {
  if (MaxAveragePeriod() == 0) {
    MCU_DCHECK_EQ(hours, 0);
    return mcucore::OkStatus();
  } else {
    MCU_DCHECK(false) << MCU_FLASHSTR_128(
        "SetAveragePeriod must be overridden if MaxAveragePeriod is not 0.");
    return ErrorCodes::NotImplemented();
  }
}

bool ObservingConditionsAdapter::HandlePutRefresh(const AlpacaRequest& request,
                                                  Print& out) {
  return WriteResponse::StatusResponse(request, Refresh(), out);
}

mcucore::Status ObservingConditionsAdapter::Refresh() {
  return ErrorCodes::NotImplemented();
}

bool ObservingConditionsAdapter::WriteDoubleOrSensorErrorResponse(
    const AlpacaRequest& request, ESensorName sensor_name,
    mcucore::StatusOr<double> result, Print& out) {
  if (result.ok()) {
    return WriteResponse::DoubleResponse(request, result.value(), out);
  } else if (static_cast<int>(result.status().code()) ==
             ErrorCodes::kNotImplemented) {
    return WriteSensorNotImpementedResponse(request, sensor_name, out);
  } else {
    return WriteResponse::StatusResponse(request, result.status(), out);
  }
}

bool ObservingConditionsAdapter::WriteSensorNotImpementedResponse(
    const AlpacaRequest& request, ESensorName sensor_name, Print& out) {
  auto error_message =
      mcucore::PrintableCat(MCU_FLASHSTR("Sensor not implemented: "),
                            ToFlashStringHelper(sensor_name));
  return WriteResponse::AscomErrorResponse(request, ErrorCodes::kNotImplemented,
                                           error_message, out);
}

}  // namespace alpaca
