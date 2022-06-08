#include "alpaca_devices.h"

#include <McuCore.h>
#include <McuNet.h>

#include "alpaca_response.h"
#include "constants.h"
#include "literals.h"

namespace alpaca {

AlpacaDevices::AlpacaDevices(mcucore::ArrayView<DeviceInterface*> devices)
    : devices_(devices) {}

bool AlpacaDevices::Initialize() {
  MCU_CHECK_OK_AND_ASSIGN(auto tlv, mcucore::EepromTlv::GetIfValid());
  // Before initializing the devices, we make sure they're valid:
  // * None of the pointers are nullptr.
  // * No two devices have the same UUID or EepromDomain.
  // * No two devices of the same type have the same device_number.
  for (int i = 0; i < devices_.size(); ++i) {
    MCU_CHECK_NE(devices_[i], nullptr)
        << MCU_FLASHSTR("DeviceInterface pointer [") << i
        << MCU_FLASHSTR("] is null!");
  }
  for (int i = 0; i < devices_.size(); ++i) {
    DeviceInterface* const device1 = devices_[i];
    MCU_CHECK_OK_AND_ASSIGN(auto device1_uuid,
                            device1->device_info().GetOrCreateUniqueId(tlv));
    for (int j = i + 1; j < devices_.size(); ++j) {
      DeviceInterface* const device2 = devices_[j];
      MCU_CHECK_NE(device1, device2)
          << MCU_FLASHSTR("Device appears twice in the list of devices:")
          << MCU_FLASHSTR(" device_type=") << device1->device_type()
          << MCU_FLASHSTR(", device_number=") << device1->device_number()
          << MCU_FLASHSTR(", name=")
          << mcucore::HexEscaped(device1->device_info().name);
      MCU_CHECK_NE(device1->device_info().domain, device2->device_info().domain)
          << MCU_FLASHSTR("Devices [") << i << MCU_FLASHSTR("] and [") << j
          << MCU_FLASHSTR("] have the same domain");
      MCU_CHECK_OK_AND_ASSIGN(auto device2_uuid,
                              device2->device_info().GetOrCreateUniqueId(tlv));
      MCU_CHECK_NE(device1_uuid, device2_uuid)
          << MCU_FLASHSTR("Devices [") << i << MCU_FLASHSTR("] and [") << j
          << MCU_FLASHSTR("] have the same UUID: ") << device1_uuid;

      if (device1->device_type() != device2->device_type()) {
        break;
      }

      MCU_CHECK_NE(device1->device_number(), device2->device_number())
          << MCU_FLASHSTR("Devices [") << i << MCU_FLASHSTR("] and [") << j
          << MCU_FLASHSTR("] have the same type and number");
    }
  }
  // TODO(jamessynge): Verify that device_numbers, within each device_type, are
  // ascending and start at zero.
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
  MCU_VLOG(3) << MCU_FLASHSTR(
      "AlpacaDevices::HandleManagementConfiguredDevices");
  MCU_DCHECK_EQ(request.api_group, EApiGroup::kManagement);
  MCU_DCHECK_EQ(request.api, EAlpacaApi::kManagementConfiguredDevices);
  ConfiguredDevicesResponse response(request, devices_);
  return WriteResponse::OkJsonResponse(request, response, out);
}

bool AlpacaDevices::DispatchDeviceRequest(AlpacaRequest& request, Print& out) {
  MCU_DCHECK(request.api == EAlpacaApi::kDeviceApi ||
             request.api == EAlpacaApi::kDeviceSetup);

  for (DeviceInterface* device : devices_) {
    if (request.device_type == device->device_type() &&
        request.device_number == device->device_number()) {
      const auto result = DispatchDeviceRequest(request, *device, out);
      if (!result) {
        MCU_VLOG(3) << MCU_FLASHSTR("DispatchDeviceRequest: ")
                    << MCU_FLASHSTR("result=") << result;
      }
      return result;
    }
  }

  const auto s1 = MCU_PSV("Not found: api=");
  const auto s2 = MCU_PSV(", type=");
  const auto s3 = MCU_PSV(", number=");

  MCU_VLOG(2) << s1 << request.api << s2 << request.device_type << s3
              << request.device_number;

  // https://ascom-standards.org/Developer/ASCOM%20Alpaca%20API%20Reference.pdf
  // says we should return Bad Request rather than Not Found or another such
  // error for an understandable Alpaca path which is for a non-existent device.
  return WriteResponse::HttpErrorResponse(
      EHttpStatusCode::kHttpBadRequest,
      mcucore::PrintableCat(s1, request.api, s2, request.device_type, s3,
                            request.device_number),
      out);
}

bool AlpacaDevices::DispatchDeviceRequest(AlpacaRequest& request,
                                          DeviceInterface& device, Print& out) {
  MCU_VLOG(3) << MCU_FLASHSTR("AlpacaDevices::DispatchDeviceRequest: ")
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
      mcucore::PrintableCat(MCU_FLASHSTR("request.api: "),
                            ToFlashStringHelper(request.api)),
      out);
  // COV_NF_END
}

}  // namespace alpaca
