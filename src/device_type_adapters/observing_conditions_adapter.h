#ifndef TINY_ALPACA_SERVER_SRC_DEVICE_TYPE_ADAPTERS_OBSERVING_CONDITIONS_ADAPTER_H_
#define TINY_ALPACA_SERVER_SRC_DEVICE_TYPE_ADAPTERS_OBSERVING_CONDITIONS_ADAPTER_H_

// Implements much of the common logic for all Observing Conditions devices,
// providing a simple API for sub-classes to implement (e.g. GetHumidity).
//
// Author: james.synge@gmail.com

#include "device_api_handler_base.h"
#include "utils/platform.h"
#include "utils/status_or.h"

namespace alpaca {

class ObservingConditionsAdapter : public DeviceApiHandlerBase {
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
  // DeviceApiHandlerBase. Returns true to indicate that the response was
  // written without error, otherwise false, in which case the connection to the
  // client will be closed.
  bool HandlePutRequest(const AlpacaRequest& request, Print& out) override;

  // Accessors for various sensor values. The default implementations return an
  // unimplemented error.
  virtual StatusOr<float> GetAveragePeriod();
  virtual StatusOr<float> GetCloudCover();
  virtual StatusOr<float> GetDewPoint();
  virtual StatusOr<float> GetHumidity();
  virtual StatusOr<float> GetPressure();
  virtual StatusOr<float> GetRainRate();
  virtual StatusOr<float> GetSkyBrightness();
  virtual StatusOr<float> GetSkyQuality();
  virtual StatusOr<float> GetSkyTemperature();
  virtual StatusOr<float> GetStarFullWidthHalfMax();
  virtual StatusOr<float> GetTemperature();
  virtual StatusOr<float> GetWindDirection();
  virtual StatusOr<float> GetWindGust();
  virtual StatusOr<float> GetWindSpeed();
  virtual StatusOr<float> GetTimeSinceLastUpdate();

  // Records the period over which the caller wants sensor data to be averaged.
  // Returns OK if the implementation can perform averaging, else an error.
  virtual Status SetAveragePeriod(float value);

  // Refreshes sensor values from hardware.
  virtual Status Refresh();

  // Handles a GET sensordescription 'request', writes the HTTP response message
  // to out. Generates an error if there is no sensor name, else delegates to
  // HandleGetNamedSensorDescription.
  virtual bool HandleGetSensorDescription(const AlpacaRequest& request,
                                          Print& out);

  // Writes a response with the description of 'sensor_name' to 'out', or an
  // error if it is not a known or supported device. The default implementation
  // calls GetSensorDescription to get the description, though subclasses can
  // override if they want to construct a description that can't be represented
  // as an Literal instance.
  virtual bool HandleGetNamedSensorDescription(const AlpacaRequest& request,
                                               StringView sensor_name,
                                               Print& out);

  // Returns the description of the named sensor, or an error if not known.
  // The default implementation returns an unimplemented error.
  virtual StatusOr<Literal> GetSensorDescription(StringView sensor_name);
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_DEVICE_TYPE_ADAPTERS_OBSERVING_CONDITIONS_ADAPTER_H_
