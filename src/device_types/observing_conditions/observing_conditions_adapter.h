#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_OBSERVING_CONDITIONS_OBSERVING_CONDITIONS_ADAPTER_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_OBSERVING_CONDITIONS_OBSERVING_CONDITIONS_ADAPTER_H_

// Implements much of the common logic for all Observing Conditions devices,
// providing a simple API for sub-classes to implement (e.g. GetHumidity).
//
// Author: james.synge@gmail.com

#include "constants.h"
#include "device_types/device_impl_base.h"
#include "utils/platform.h"
#include "utils/status.h"
#include "utils/status_or.h"

namespace alpaca {

class ObservingConditionsAdapter : public DeviceImplBase {
 public:
  explicit ObservingConditionsAdapter(const DeviceInfo& device_info);
  ~ObservingConditionsAdapter() override {}

  // Handles GET 'request', writes the HTTP response message to 'out'. Returns
  // true to indicate that the response was written without error, otherwise
  // false, in which case the connection to the client will be closed.
  //
  // For device specific methods (e.g. "/humidity"), gets the value to be
  // returned to the client by calling the appropriate GetXyz() method below
  // (e.g. GetHumidity). For other methods (e.g. "/driverinfo"), delegates to
  // the base classes' HandleGetRequest method.
  bool HandleGetRequest(const AlpacaRequest& request, Print& out) override;

  //////////////////////////////////////////////////////////////////////////////
  // Accessors for various sensor values. The default implementations return an
  // unimplemented error.

  // Returns the number of hours over which all sensor values will be averaged.
  virtual StatusOr<double> GetAveragePeriod();

  // Returns the percentage of the sky obscured by cloud.
  virtual StatusOr<double> GetCloudCover();

  // Returns the atmospheric dew point at the observatory reported in °C.
  virtual StatusOr<double> GetDewPoint();

  // Returns the atmospheric humidity (%) at the observatory.
  virtual StatusOr<double> GetHumidity();

  // Returns the atmospheric pressure in hectoPascals at the observatory's
  // altitude
  // - NOT reduced to sea level.
  virtual StatusOr<double> GetPressure();

  // Returns the rain rate (mm/hour) at the observatory. For more info, see:
  // https://ascom-standards.org/Help/Developer/html/P_ASCOM_DeviceInterface_IObservingConditions_RainRate.htm
  virtual StatusOr<double> GetRainRate();

  // Returns the description of the named sensor, or an error if not known.
  // The default implementation returns an unimplemented error.
  virtual StatusOr<Literal> GetSensorDescription(ESensorName sensor_name);

  // Returns the sky brightness at the observatory (Lux).
  virtual StatusOr<double> GetSkyBrightness();

  // Returns the sky quality at the observatory (magnitudes per square arc
  // second).
  virtual StatusOr<double> GetSkyQuality();

  // Returns the sky temperature(°C) at the observatory.
  virtual StatusOr<double> GetSkyTemperature();

  // Returns the seeing at the observatory measured as star full width half
  // maximum (FWHM) in arc secs.
  virtual StatusOr<double> GetStarFWHM();

  // Returns the temperature(°C) at the observatory.
  virtual StatusOr<double> GetTemperature();

  // Returns the time (hours) since the sensor specified in the SensorName
  // parameter was last updated.
  virtual StatusOr<double> GetTimeSinceLastUpdate(ESensorName sensor_name);

  // Returns the wind direction. The returned value must be between 0.0 and
  // 360.0, interpreted according to the metereological standard, where a
  // special value of 0.0 is returned when the wind speed is 0.0. Wind direction
  // is measured clockwise from north, through east, where East=90.0,
  // South=180.0, West=270.0 and North=360.0.
  virtual StatusOr<double> GetWindDirection();

  // Returns the peak 3 second wind gust(m/s) at the observatory over the last 2
  // minutes. Note that this doesn't follow the averaging of other sensors.
  virtual StatusOr<double> GetWindGust();

  // Returns the wind speed(m/s) at the observatory.
  virtual StatusOr<double> GetWindSpeed();

  //////////////////////////////////////////////////////////////////////////////

  // Handles PUT 'request', writes the HTTP response message to 'out'. Returns
  // true to indicate that the response was written without error, otherwise
  // false, in which case the connection to the client will be closed.
  //
  // Delegates to the appropriate device and request specific method (e.g.
  // "/averageperiod" PUT requests are passed to HandlePutAveragePeriod). The
  // delegatee is responsible for further validating the request (e.g. that the
  // "AveragePeriod" parameter is provided for a "/averageperiod" PUT request,
  // and has a valid value). Requests that are not specific to this device type
  // are delegated to the base class, DeviceImplBase.
  bool HandlePutRequest(const AlpacaRequest& request, Print& out) override;

  //////////////////////////////////////////////////////////////////////////////
  // Handlers for mutating requests (e.g. "/refresh"). These return true to
  // indicate that the response was written without error, otherwise false, in
  // which case the connection to the client will be closed.

  virtual bool HandlePutAveragePeriod(const AlpacaRequest& request, Print& out);
  virtual bool HandlePutRefresh(const AlpacaRequest& request, Print& out);

  //////////////////////////////////////////////////////////////////////////////
  // Handlers for the core of the above methods.

  // Records the period (hours) over which the caller wants sensor data to be
  // averaged. Returns OK if the implementation can perform averaging over the
  // period, else an error. The default implementation returns an error if the
  // value is not 0 (i.e. no averaging is supported by default).
  virtual Status SetAveragePeriod(double hours);

  // Returns the maximum supported average period, which is used by
  // HandlePutAveragePeriod to validate the hours parameter prior to passing it
  // to SetAveragePeriod. The default implementation returns 0. The returned
  // value must not be less than zero.
  virtual double MaxAveragePeriod() const;

  // Refreshes sensor values from hardware.
  virtual Status Refresh();

  // If the result is OK, the write a DoubleResponse, else write the specified
  // sensor error.
  static bool WriteDoubleOrSensorErrorResponse(const AlpacaRequest& request,
                                               ESensorName sensor_name,
                                               StatusOr<double> result,
                                               Print& out);

  // Write a Not Implemented error with the name of the sensor.
  static bool WriteSensorNotImpementedResponse(const AlpacaRequest& request,
                                               ESensorName sensor_name,
                                               Print& out);
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPES_OBSERVING_CONDITIONS_OBSERVING_CONDITIONS_ADAPTER_H_
