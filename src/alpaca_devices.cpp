#include "alpaca_devices.h"

#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "constants.h"
#include "json_response.h"
#include "literals.h"
#include "utils/any_printable.h"
#include "utils/array_view.h"
#include "utils/hex_escape.h"
#include "utils/json_encoder.h"
#include "utils/logging.h"
#include "utils/platform_ethernet.h"
#include "utils/string_view.h"

namespace alpaca {

AlpacaDevices::AlpacaDevices(ArrayView<DeviceInterface*> devices)
    : devices_(devices) {}

bool AlpacaDevices::Initialize() {
  bool result = true;
  // Before initializing the devices, we make sure they're valid:
  // * None of the pointers are nullptr.
  // * No two devices of the same type have the same device_number or the
  //   same config_id).
  for (int i = 0; i < devices_.size(); ++i) {
    DeviceInterface* const device = devices_[i];
    if (device == nullptr) {
      TAS_DCHECK_NE(device, nullptr)
          << "DeviceInterface pointer [" << i << "] is null!";
      result = false;  // TAS_DCHECK may be disabled.
    }
  }
  if (!result) {
    return false;
  }
  for (int i = 0; i < devices_.size(); ++i) {
    DeviceInterface* const device1 = devices_[i];
    for (int j = i + 1; j < devices_.size(); ++j) {
      DeviceInterface* const device2 = devices_[j];
      if (device1 == device2) {
        TAS_DCHECK_NE(device1, device2)
            << "Device appears twice in the list of devices: "
            << "type=" << device1->device_type()
            << ", number=" << device1->device_number()
            << ", name=" << HexEscaped(device1->device_info().name);
        result = false;  // TAS_DCHECK may be disabled.
        break;
      }
      if (device1->device_info().unique_id ==
          device2->device_info().unique_id) {
        TAS_DCHECK(false) << "Devices [" << i << "] and [" << j
                          << "] have the same unique_id";
        result = false;  // TAS_DCHECK may be disabled.
      }
      if (device1->device_type() != device2->device_type()) {
        break;
      }
      if (device1->device_number() == device2->device_number()) {
        TAS_DCHECK(false) << "Devices [" << i << "] and [" << j
                          << "] have the same type and number";
        result = false;  // TAS_DCHECK may be disabled.
      }
#if 0
      if (device1->device_info().config_id ==
          device2->device_info().config_id) {
        // Someday we'll generate a UUID from the device type and config_id,
        // so they need to be distinct.
        TAS_DCHECK(false) << "Devices [" << i << "] and [" << j
                          << "] have the same type and config_id";
        result = false;  // TAS_DCHECK may be disabled.
      }
#endif
    }
  }
  if (!result) {
    return false;
  }
  for (DeviceInterface* device : devices_) {
    device->Initialize();
  }
  return true;
}

void AlpacaDevices::MaintainDevices() {
  // Give devices a chance to perform work.
  for (DeviceInterface* device : devices_) {
    device->Update();
  }
}

bool AlpacaDevices::HandleManagementConfiguredDevices(AlpacaRequest& request,
                                                      Print& out) {
  TAS_VLOG(3) << TASLIT("AlpacaDevices::HandleManagementConfiguredDevices");
  TAS_DCHECK_EQ(request.api_group, EApiGroup::kManagement);
  TAS_DCHECK_EQ(request.api, EAlpacaApi::kManagementConfiguredDevices);
  ConfiguredDevicesResponse response(request, devices_);
  return WriteResponse::OkJsonResponse(request, response, out);
}

bool AlpacaDevices::DispatchDeviceRequest(AlpacaRequest& request, Print& out) {
  TAS_VLOG(3) << TASLIT("AlpacaDevices::DispatchDeviceRequest: device_type=")
              << request.device_type << TASLIT(", device_number=")
              << request.device_number;
  TAS_DCHECK(request.api == EAlpacaApi::kDeviceApi ||
             request.api == EAlpacaApi::kDeviceSetup);

  for (DeviceInterface* device : devices_) {
    if (request.device_type == device->device_type() &&
        request.device_number == device->device_number()) {
      return DispatchDeviceRequest(request, *device, out);
    }
  }

  TAS_VLOG(3) << TASLIT("Found no Device API Handler");
  // https://ascom-standards.org/Developer/ASCOM%20Alpaca%20API%20Reference.pdf
  // says we should return Bad Request rather than Not Found or another such
  // error for an understandable Alpaca path which is for a non-existent device.
  return WriteResponse::HttpErrorResponse(EHttpStatusCode::kHttpBadRequest,
                                          TASLIT("Unknown device"), out);
}

bool AlpacaDevices::DispatchDeviceRequest(AlpacaRequest& request,
                                          DeviceInterface& device, Print& out) {
  if (request.api == EAlpacaApi::kDeviceApi) {
    TAS_VLOG(3) << TASLIT(
                       "AlpacaDevices::DispatchDeviceRequest: device_method=")
                << request.device_method;
    return device.HandleDeviceApiRequest(request, out);
  } else if (request.api == EAlpacaApi::kDeviceSetup) {
    return device.HandleDeviceSetupRequest(request, out);
  }
  // COV_NF_START
  return WriteResponse::HttpErrorResponse(
      EHttpStatusCode::kHttpInternalServerError,
      Literals::HttpMethodNotImplemented(), out);
  // COV_NF_END
}

}  // namespace alpaca
