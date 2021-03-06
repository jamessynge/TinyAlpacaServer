#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPE_ADAPTERS_OBSERVING_CONDITIONS_ADAPTER_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPE_ADAPTERS_OBSERVING_CONDITIONS_ADAPTER_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

#include "device_api_handler_base.h"
#include "platform.h"
#include "utils/status_or.h"

namespace alpaca {

class ObservingConditionsAdapter : public DeviceApiHandlerBase {
 public:
  explicit ObservingConditionsAdapter(const DeviceInfo& device_info);

  // Handle a GET 'request', write the HTTP response message to out.
  void HandleGetRequest(const AlpacaRequest& request, Print& out) override;

  // Handle a PUT 'request', write the HTTP response message to out.
  void HandlePutRequest(const AlpacaRequest& request, Print& out) override;

  virtual StatusOr<double> GetAveragePeriod() = 0;
  virtual StatusOr<double> GetCloudCover() = 0;
  virtual StatusOr<double> GetDewPoint() = 0;
  virtual StatusOr<double> GetHumidity() = 0;
  virtual StatusOr<double> GetPressure() = 0;
  virtual StatusOr<double> GetRainRate() = 0;
  virtual StatusOr<double> GetSkyBrightness() = 0;
  virtual StatusOr<double> GetSkyQuality() = 0;
  virtual StatusOr<double> GetSkyTemperature() = 0;
  virtual StatusOr<double> GetStarFullWidthHalfMax() = 0;
  virtual StatusOr<double> GetTemperature() = 0;
  virtual StatusOr<double> GetWindDirection() = 0;
  virtual StatusOr<double> GetWindGust() = 0;
  virtual StatusOr<double> GetWindSpeed() = 0;
  virtual StatusOr<double> GetTimeSinceLastUpdate() = 0;
  virtual StatusOr<Literal> GetSensorDescription(StringView sensor_name) = 0;

  virtual Status SetAveragePeriod(double value) = 0;
  virtual Status Refresh() = 0;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPE_ADAPTERS_OBSERVING_CONDITIONS_ADAPTER_H_
