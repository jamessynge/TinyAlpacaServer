#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPE_ADAPTERS_OBSERVING_CONDITIONS_ADAPTER_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPE_ADAPTERS_OBSERVING_CONDITIONS_ADAPTER_H_

// Implements much of the common logic for all Observing Conditions devices,
// providing a simple API for sub-classes to implement (e.g. GetHumidity).
//
// Author: james.synge@gmail.com

#include "device_impl_base.h"
#include "utils/platform.h"
#include "utils/status_or.h"

namespace alpaca {

class ObservingConditionsAdapter : public DeviceImplBase {
 public:
  explicit ObservingConditionsAdapter(const DeviceInfo& device_info);

  // Handles GET 'request', writes the HTTP response message to 'out'.
  // Delegates getting the values to the GetXyx() methods below.  Returns true
  // to indicate that the response was written without error, otherwise false,
  // in which case the connection to the client will be closed.
  bool HandleGetRequest(const AlpacaRequest& request, Print& out) override;

  // Handles PUT 'request', writes the HTTP response message to 'out'. Delegates
  // a PUT averageperiod request to SetAveragePeriod and a PUT referesh request
  // to Refresh; any other ASCOM method is delegated to the base class,
  // DeviceImplBase. Returns true to indicate that the response was
  // written without error, otherwise false, in which case the connection to the
  // client will be closed.
  bool HandlePutRequest(const AlpacaRequest& request, Print& out) override;

  //////////////////////////////////////////////////////////////////////////////
  // Accessors for various sensor values. The default implementations return an
  // unimplemented error.

  // Returns the number of hours over which all sensor values will be averaged.
  virtual StatusOr<float> GetAveragePeriod();

  // Returns the percentage of the sky obscured by cloud.
  virtual StatusOr<float> GetCloudCover();

  // Returns the atmospheric dew point at the observatory reported in °C.
  virtual StatusOr<float> GetDewPoint();

  // Returns the atmospheric humidity (%) at the observatory.
  virtual StatusOr<float> GetHumidity();

  // Returns the atmospheric pressure in hectoPascals at the observatory's
  // altitude
  // - NOT reduced to sea level.
  virtual StatusOr<float> GetPressure();

  // Returns the rain rate (mm/hour) at the observatory.
  virtual StatusOr<float> GetRainRate();

  // Returns the description of the named sensor, or an error if not known.
  // The default implementation returns an unimplemented error.
  virtual StatusOr<Literal> GetSensorDescription(ESensorName sensor_name);

  // Returns the sky brightness at the observatory (Lux).
  virtual StatusOr<float> GetSkyBrightness();

  // Returns the sky quality at the observatory (magnitudes per square arc
  // second).
  virtual StatusOr<float> GetSkyQuality();

  // Returns the sky temperature(°C) at the observatory.
  virtual StatusOr<float> GetSkyTemperature();

  // Returns the seeing at the observatory measured as star full width half
  // maximum (FWHM) in arc secs.
  virtual StatusOr<float> GetStarFullWidthHalfMax();

  // Returns the temperature(°C) at the observatory.
  virtual StatusOr<float> GetTemperature();

  // Returns the time (hours) since the sensor specified in the SensorName
  // parameter was last updated.
  virtual StatusOr<float> GetTimeSinceLastUpdate(ESensorName sensor_name);

  // Returns the wind direction. The returned value must be between 0.0 and
  // 360.0, interpreted according to the metereological standard, where a
  // special value of 0.0 is returned when the wind speed is 0.0. Wind direction
  // is measured clockwise from north, through east, where East=90.0,
  // South=180.0, West=270.0 and North=360.0.
  virtual StatusOr<float> GetWindDirection();

  // Returns the peak 3 second wind gust(m/s) at the observatory over the last 2
  // minutes. Note that this doesn't follow the averaging of other sensors.
  virtual StatusOr<float> GetWindGust();

  // Returns the wind speed(m/s) at the observatory.
  virtual StatusOr<float> GetWindSpeed();

  //////////////////////////////////////////////////////////////////////////////

  // Records the period (hours) over which the caller wants sensor data to be
  // averaged. Returns OK if the implementation can perform averaging over the
  // period, else an error.
  virtual Status SetAveragePeriod(float hours);

  // Refreshes sensor values from hardware.
  virtual Status Refresh();

 protected:
  double average_period() const { return average_period_; }

 private:
  // The period (hours) over which to average some of the sensor data (i.e.
  // temperature but not wind gust).
  double average_period_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPE_ADAPTERS_OBSERVING_CONDITIONS_ADAPTER_H_
