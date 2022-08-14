#include "alpaca_devices.h"

#include <McuCore.h>
#include <McuNet.h>

#include "alpaca_response.h"
#include "configured_devices_response.h"
#include "constants.h"
#include "device_description.h"
#include "literals.h"

namespace alpaca {

AlpacaDevices::AlpacaDevices(ServerContext& server_context,
                             mcucore::ArrayView<DeviceInterface*> devices)
    : server_context_(server_context), devices_(devices) {}

// Before initializing the devices, we want to make sure they're valid:
// * None of the pointers are nullptr.
// * No two devices have the same UUID or EepromDomain.
// * No two devices of the same type have the same device_number.
// * Devices are numbered starting from zero.
void AlpacaDevices::ValidateDevices() {
  // We're going to need the EepromTlv for looking up UUIDs.
  auto& tlv = server_context_.eeprom_tlv();

  for (int i = 0; i < devices_.size(); ++i) {
    MCU_CHECK_NE(devices_[i], nullptr)
        << MCU_PSD("DeviceInterface pointer [") << i << MCU_PSD("] is null!");
  }

  for (int i = 0; i < devices_.size(); ++i) {
    DeviceInterface* const device1 = devices_[i];
    const DeviceDescription& description1 = device1->device_description();
    MCU_CHECK_OK_AND_ASSIGN(auto device1_uuid,
                            description1.GetOrCreateUniqueId(tlv));
    for (int j = i + 1; j < devices_.size(); ++j) {
      DeviceInterface* const device2 = devices_[j];
      const DeviceDescription& description2 = device2->device_description();
      MCU_CHECK_NE(device1, device2)
          << MCU_PSD("Device appears twice in the list of devices:")
          << MCU_PSD(" device_type=") << description1.device_type
          << MCU_PSD(", device_number=") << description1.device_number
          << MCU_PSD(", name=") << mcucore::HexEscaped(description1.name);
      MCU_CHECK_NE(description1.domain, description2.domain)
          << MCU_PSD("Devices [") << i << MCU_PSD("] and [") << j
          << MCU_PSD("] have the same domain");
      MCU_CHECK_OK_AND_ASSIGN(auto device2_uuid,
                              description2.GetOrCreateUniqueId(tlv));
      MCU_CHECK_NE(device1_uuid, device2_uuid)
          << MCU_PSD("Devices [") << i << MCU_PSD("] and [") << j
          << MCU_PSD("] have the same UUID: ") << device1_uuid;
      if (description1.device_type != description2.device_type) {
        break;
      }
      MCU_CHECK_NE(description1.device_number, description2.device_number)
          << MCU_PSD("Devices [") << i << MCU_PSD("] and [") << j
          << MCU_PSD("] have the same type and number");
    }
  }

  // Verify that device_numbers, within each device_type, are ascending and
  // start at zero.
  for (int i = 0; i < devices_.size(); ++i) {
    DeviceInterface* const device = devices_[i];
    const DeviceDescription& description = device->device_description();
    if (description.device_number != 0) {
      // There should be a device of the same type whose number is immediately
      // below this one.
      MCU_CHECK_NE(
          FindDevice(description.device_type, description.device_number - 1),
          nullptr)
          << MCU_PSD_128(
                 "Devices of each type must be numbered starting from 0, "
                 "without any gap; ")
          << description.device_type << '#' << description.device_number
          << MCU_PSD(" (devices_[") << i
          << MCU_PSD("]) doesn't have a predecessor");
    }
  }

  for (DeviceInterface* device : devices_) {
    device->ValidateConfiguration();
  }
}

void AlpacaDevices::ResetHardware() {
  for (DeviceInterface* device : devices_) {
    device->ResetHardware();
  }
}

void AlpacaDevices::InitializeDevices() {
  for (DeviceInterface* device : devices_) {
    device->InitializeDevice();
  }
}

void AlpacaDevices::MaintainDevices() {
  // Give devices a chance to perform work.
  for (DeviceInterface* device : devices_) {
    device->MaintainDevice();
  }
}

bool AlpacaDevices::HandleManagementConfiguredDevices(AlpacaRequest& request,
                                                      Print& out) {
  MCU_VLOG(3) << MCU_PSD("AlpacaDevices::HandleManagementConfiguredDevices");
  MCU_DCHECK_EQ(request.api_group, EApiGroup::kManagement);
  MCU_DCHECK_EQ(request.api, EAlpacaApi::kManagementConfiguredDevices);
  ConfiguredDevicesResponse response(request, devices_);
  return WriteResponse::OkJsonResponse(request, response, out);
}

bool AlpacaDevices::DispatchDeviceRequest(AlpacaRequest& request, Print& out) {
  MCU_DCHECK(request.api == EAlpacaApi::kDeviceApi ||
             request.api == EAlpacaApi::kDeviceSetup);

  DeviceInterface* device =
      FindDevice(request.device_type, request.device_number);
  if (device != nullptr) {
    const auto result = DispatchDeviceRequest(request, *device, out);
    if (!result) {
      MCU_VLOG(3) << MCU_PSD("DispatchDeviceRequest: ") << MCU_PSD("result=")
                  << result;
    }
    return result;
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

void AlpacaDevices::AddToHomePageHtml(const AlpacaRequest& request,
                                      EHtmlPageSection section,
                                      mcucore::OPrintStream& strm) {
  for (DeviceInterface* device : devices_) {
    device->AddToHomePageHtml(request, section, strm);
  }
}

bool AlpacaDevices::DispatchDeviceRequest(AlpacaRequest& request,
                                          DeviceInterface& device, Print& out) {
  MCU_VLOG(3) << MCU_PSD("AlpacaDevices::DispatchDeviceRequest: ")
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
      mcucore::PrintableCat(MCU_PSV("request.api: "),
                            ToFlashStringHelper(request.api)),
      out);
  // COV_NF_END
}

// Returns the specified device, or nullptr if not found.
DeviceInterface* AlpacaDevices::FindDevice(EDeviceType device_type,
                                           uint32_t device_number) {
  for (DeviceInterface* device : devices_) {
    if (device_type == device->device_description().device_type &&
        device_number == device->device_description().device_number) {
      return device;
    }
  }
  return nullptr;
}

}  // namespace alpaca
