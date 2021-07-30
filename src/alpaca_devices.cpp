#include "alpaca_devices.h"

#include "alpaca_response.h"
#include "constants.h"
#include "literals.h"
#include "utils/hex_escape.h"
#include "utils/inline_literal.h"
#include "utils/json_encoder.h"
#include "utils/logging.h"
#include "utils/platform_ethernet.h"

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
          << TAS_FLASHSTR("DeviceInterface pointer [") << i
          << TAS_FLASHSTR("] is null!");
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
            << TAS_FLASHSTR("Device appears twice in the list of devices:")
            << TAS_FLASHSTR(" device_type=") << device1->device_type()
            << TAS_FLASHSTR(", device_number=") << device1->device_number()
            << TAS_FLASHSTR(", name=")
            << HexEscaped(device1->device_info().name);
        result = false;  // TAS_DCHECK may be disabled.
        break;
      }
      if (device1->device_info().unique_id ==
          device2->device_info().unique_id) {
        TAS_DCHECK(false) << TAS_FLASHSTR("Devices [") << i
                          << TAS_FLASHSTR("] and [") << j
                          << TAS_FLASHSTR("] have the same unique_id");
        result = false;  // TAS_DCHECK may be disabled.
      }
      if (device1->device_type() != device2->device_type()) {
        break;
      }
      if (device1->device_number() == device2->device_number()) {
        TAS_DCHECK(false) << TAS_FLASHSTR("Devices [") << i
                          << TAS_FLASHSTR("] and [") << j
                          << TAS_FLASHSTR("] have the same type and number");
        result = false;  // TAS_DCHECK may be disabled.
      }
#if 0
      if (device1->device_info().config_id ==
          device2->device_info().config_id) {
        // Someday we'll generate a UUID from the device type and config_id,
        // so they need to be distinct.
        TAS_DCHECK(false) << TAS_FLASHSTR("Devices [") << i
                          << TAS_FLASHSTR("] and [") << j
                          << TAS_FLASHSTR("] have the same type and config_id");
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
    device->MaintainDevice();
  }
}

bool AlpacaDevices::HandleManagementConfiguredDevices(AlpacaRequest& request,
                                                      Print& out) {
  TAS_VLOG(3) << TAS_FLASHSTR(
      "AlpacaDevices::HandleManagementConfiguredDevices");
  TAS_DCHECK_EQ(request.api_group, EApiGroup::kManagement);
  TAS_DCHECK_EQ(request.api, EAlpacaApi::kManagementConfiguredDevices);
  ConfiguredDevicesResponse response(request, devices_);
  return WriteResponse::OkJsonResponse(request, response, out);
}

bool AlpacaDevices::DispatchDeviceRequest(AlpacaRequest& request, Print& out) {
  TAS_DCHECK(request.api == EAlpacaApi::kDeviceApi ||
             request.api == EAlpacaApi::kDeviceSetup);

  for (DeviceInterface* device : devices_) {
    if (request.device_type == device->device_type() &&
        request.device_number == device->device_number()) {
      const auto result = DispatchDeviceRequest(request, *device, out);
      if (!result) {
        TAS_VLOG(3) << TAS_FLASHSTR("DispatchDeviceRequest: ")
                    << TAS_FLASHSTR("result=") << result;
      }
      return result;
    }
  }

  TAS_VLOG(2) << TAS_FLASHSTR("AlpacaDevices::")
              << TAS_FLASHSTR("DispatchDeviceRequest: ")
              << TAS_FLASHSTR("Found no Device API Handler for api=")
              << request.api << TAS_FLASHSTR(" device_type=")
              << request.device_type << TAS_FLASHSTR(", device_number=")
              << request.device_number;

  // https://ascom-standards.org/Developer/ASCOM%20Alpaca%20API%20Reference.pdf
  // says we should return Bad Request rather than Not Found or another such
  // error for an understandable Alpaca path which is for a non-existent device.
  return WriteResponse::HttpErrorResponse(EHttpStatusCode::kHttpBadRequest,
                                          TAS_FLASHSTR("Unknown device"), out);
}

bool AlpacaDevices::DispatchDeviceRequest(AlpacaRequest& request,
                                          DeviceInterface& device, Print& out) {
  TAS_VLOG(3) << TAS_FLASHSTR("AlpacaDevices::DispatchDeviceRequest: ")
              << request.device_type << '/' << request.device_number << '/'
              << request.device_method;
  if (request.api == EAlpacaApi::kDeviceApi) {
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
