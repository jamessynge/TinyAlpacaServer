#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPE_ADAPTERS_OBSERVING_CONDITIONS_ADAPTER_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPE_ADAPTERS_OBSERVING_CONDITIONS_ADAPTER_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

#include "device_api_handler_base.h"
#include "utils/platform.h"
#include "utils/status_or.h"

namespace alpaca {

class ObservingConditionsAdapter : public DeviceApiHandlerBase {
 public:
  explicit ObservingConditionsAdapter(const DeviceInfo& device_info);

  // Handle a GET 'request', write the HTTP response message to out.
  bool HandleGetRequest(const AlpacaRequest& request, Print& out) override;

  // Handle a PUT 'request', write the HTTP response message to out.
  bool HandlePutRequest(const AlpacaRequest& request, Print& out) override;

  virtual StatusOr<double> GetAveragePeriod();
  virtual StatusOr<double> GetCloudCover();
  virtual StatusOr<double> GetDewPoint();
  virtual StatusOr<double> GetHumidity();
  virtual StatusOr<double> GetPressure();
  virtual StatusOr<double> GetRainRate();
  virtual StatusOr<double> GetSkyBrightness();
  virtual StatusOr<double> GetSkyQuality();
  virtual StatusOr<double> GetSkyTemperature();
  virtual StatusOr<double> GetStarFullWidthHalfMax();
  virtual StatusOr<double> GetTemperature();
  virtual StatusOr<double> GetWindDirection();
  virtual StatusOr<double> GetWindGust();
  virtual StatusOr<double> GetWindSpeed();
  virtual StatusOr<double> GetTimeSinceLastUpdate();
  virtual StatusOr<Literal> GetSensorDescription(StringView sensor_name);

  virtual Status SetAveragePeriod(double value);
  virtual Status Refresh();
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPE_ADAPTERS_OBSERVING_CONDITIONS_ADAPTER_H_
