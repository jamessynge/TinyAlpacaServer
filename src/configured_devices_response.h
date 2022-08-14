#ifndef TINY_ALPACA_SERVER_SRC_CONFIGURED_DEVICES_RESPONSE_H_
#define TINY_ALPACA_SERVER_SRC_CONFIGURED_DEVICES_RESPONSE_H_

// Generates the JSON body for "/management/v1/configureddevices" requests.

#include <McuCore.h>

#include "alpaca_request.h"
#include "device_interface.h"
#include "json_response.h"

namespace alpaca {

class ConfiguredDevicesResponse : public JsonMethodResponse {
 public:
  ConfiguredDevicesResponse(const AlpacaRequest& request,
                            mcucore::ArrayView<DeviceInterface*> devices)
      : JsonMethodResponse(request), devices_(devices) {}

  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const override;

 private:
  mcucore::ArrayView<DeviceInterface*> devices_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_CONFIGURED_DEVICES_RESPONSE_H_
