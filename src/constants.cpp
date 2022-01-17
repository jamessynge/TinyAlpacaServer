#include "constants.h"

// GENERATED FILE. See make_enum_to_string.py

#include "print_misc.h"
#include "progmem_string_data.h"

namespace alpaca {

size_t PrintValueTo(RequestDecoderStatus v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("RequestDecoderStatus"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpStatusCode v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("EHttpStatusCode"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpMethod v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("EHttpMethod"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EApiGroup v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("EApiGroup"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EAlpacaApi v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("EAlpacaApi"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EManagementMethod v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("EManagementMethod"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EDeviceType v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("EDeviceType"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EDeviceMethod v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("EDeviceMethod"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EParameter v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("EParameter"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(ESensorName v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("ESensorName"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpHeader v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("EHttpHeader"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EContentType v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("EContentType"),
                                          static_cast<uint32_t>(v), out);
}

const __FlashStringHelper* ToFlashStringHelper(RequestDecoderStatus v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case RequestDecoderStatus::kReset:
      return MCU_FLASHSTR("Reset");
    case RequestDecoderStatus::kDecoding:
      return MCU_FLASHSTR("Decoding");
    case RequestDecoderStatus::kDecoded:
      return MCU_FLASHSTR("Decoded");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == RequestDecoderStatus::kReset) {
    return MCU_FLASHSTR("Reset");
  }
  if (v == RequestDecoderStatus::kDecoding) {
    return MCU_FLASHSTR("Decoding");
  }
  if (v == RequestDecoderStatus::kDecoded) {
    return MCU_FLASHSTR("Decoded");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EHttpStatusCode v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case EHttpStatusCode::kContinueDecoding:
      return MCU_FLASHSTR("ContinueDecoding");
    case EHttpStatusCode::kNeedMoreInput:
      return MCU_FLASHSTR("NeedMoreInput");
    case EHttpStatusCode::kHttpOk:
      return MCU_FLASHSTR("HttpOk");
    case EHttpStatusCode::kHttpBadRequest:
      return MCU_FLASHSTR("HttpBadRequest");
    case EHttpStatusCode::kHttpMethodNotAllowed:
      return MCU_FLASHSTR("HttpMethodNotAllowed");
    case EHttpStatusCode::kHttpNotAcceptable:
      return MCU_FLASHSTR("HttpNotAcceptable");
    case EHttpStatusCode::kHttpLengthRequired:
      return MCU_FLASHSTR("HttpLengthRequired");
    case EHttpStatusCode::kHttpPayloadTooLarge:
      return MCU_FLASHSTR("HttpPayloadTooLarge");
    case EHttpStatusCode::kHttpUnsupportedMediaType:
      return MCU_FLASHSTR("HttpUnsupportedMediaType");
    case EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge:
      return MCU_FLASHSTR("HttpRequestHeaderFieldsTooLarge");
    case EHttpStatusCode::kHttpInternalServerError:
      return MCU_FLASHSTR("HttpInternalServerError");
    case EHttpStatusCode::kHttpMethodNotImplemented:
      return MCU_FLASHSTR("HttpMethodNotImplemented");
    case EHttpStatusCode::kHttpVersionNotSupported:
      return MCU_FLASHSTR("HttpVersionNotSupported");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == EHttpStatusCode::kContinueDecoding) {
    return MCU_FLASHSTR("ContinueDecoding");
  }
  if (v == EHttpStatusCode::kNeedMoreInput) {
    return MCU_FLASHSTR("NeedMoreInput");
  }
  if (v == EHttpStatusCode::kHttpOk) {
    return MCU_FLASHSTR("HttpOk");
  }
  if (v == EHttpStatusCode::kHttpBadRequest) {
    return MCU_FLASHSTR("HttpBadRequest");
  }
  if (v == EHttpStatusCode::kHttpMethodNotAllowed) {
    return MCU_FLASHSTR("HttpMethodNotAllowed");
  }
  if (v == EHttpStatusCode::kHttpNotAcceptable) {
    return MCU_FLASHSTR("HttpNotAcceptable");
  }
  if (v == EHttpStatusCode::kHttpLengthRequired) {
    return MCU_FLASHSTR("HttpLengthRequired");
  }
  if (v == EHttpStatusCode::kHttpPayloadTooLarge) {
    return MCU_FLASHSTR("HttpPayloadTooLarge");
  }
  if (v == EHttpStatusCode::kHttpUnsupportedMediaType) {
    return MCU_FLASHSTR("HttpUnsupportedMediaType");
  }
  if (v == EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge) {
    return MCU_FLASHSTR("HttpRequestHeaderFieldsTooLarge");
  }
  if (v == EHttpStatusCode::kHttpInternalServerError) {
    return MCU_FLASHSTR("HttpInternalServerError");
  }
  if (v == EHttpStatusCode::kHttpMethodNotImplemented) {
    return MCU_FLASHSTR("HttpMethodNotImplemented");
  }
  if (v == EHttpStatusCode::kHttpVersionNotSupported) {
    return MCU_FLASHSTR("HttpVersionNotSupported");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EHttpMethod v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case EHttpMethod::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case EHttpMethod::GET:
      return MCU_FLASHSTR("GET");
    case EHttpMethod::PUT:
      return MCU_FLASHSTR("PUT");
    case EHttpMethod::HEAD:
      return MCU_FLASHSTR("HEAD");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == EHttpMethod::kUnknown) {
    return MCU_FLASHSTR("Unknown");
  }
  if (v == EHttpMethod::GET) {
    return MCU_FLASHSTR("GET");
  }
  if (v == EHttpMethod::PUT) {
    return MCU_FLASHSTR("PUT");
  }
  if (v == EHttpMethod::HEAD) {
    return MCU_FLASHSTR("HEAD");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EApiGroup v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case EApiGroup::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case EApiGroup::kDevice:
      return MCU_FLASHSTR("Device");
    case EApiGroup::kManagement:
      return MCU_FLASHSTR("Management");
    case EApiGroup::kSetup:
      return MCU_FLASHSTR("Setup");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == EApiGroup::kUnknown) {
    return MCU_FLASHSTR("Unknown");
  }
  if (v == EApiGroup::kDevice) {
    return MCU_FLASHSTR("Device");
  }
  if (v == EApiGroup::kManagement) {
    return MCU_FLASHSTR("Management");
  }
  if (v == EApiGroup::kSetup) {
    return MCU_FLASHSTR("Setup");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EAlpacaApi v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case EAlpacaApi::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case EAlpacaApi::kDeviceApi:
      return MCU_FLASHSTR("DeviceApi");
    case EAlpacaApi::kDeviceSetup:
      return MCU_FLASHSTR("DeviceSetup");
    case EAlpacaApi::kManagementApiVersions:
      return MCU_FLASHSTR("ManagementApiVersions");
    case EAlpacaApi::kManagementDescription:
      return MCU_FLASHSTR("ManagementDescription");
    case EAlpacaApi::kManagementConfiguredDevices:
      return MCU_FLASHSTR("ManagementConfiguredDevices");
    case EAlpacaApi::kServerSetup:
      return MCU_FLASHSTR("ServerSetup");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == EAlpacaApi::kUnknown) {
    return MCU_FLASHSTR("Unknown");
  }
  if (v == EAlpacaApi::kDeviceApi) {
    return MCU_FLASHSTR("DeviceApi");
  }
  if (v == EAlpacaApi::kDeviceSetup) {
    return MCU_FLASHSTR("DeviceSetup");
  }
  if (v == EAlpacaApi::kManagementApiVersions) {
    return MCU_FLASHSTR("ManagementApiVersions");
  }
  if (v == EAlpacaApi::kManagementDescription) {
    return MCU_FLASHSTR("ManagementDescription");
  }
  if (v == EAlpacaApi::kManagementConfiguredDevices) {
    return MCU_FLASHSTR("ManagementConfiguredDevices");
  }
  if (v == EAlpacaApi::kServerSetup) {
    return MCU_FLASHSTR("ServerSetup");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EManagementMethod v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case EManagementMethod::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case EManagementMethod::kDescription:
      return MCU_FLASHSTR("Description");
    case EManagementMethod::kConfiguredDevices:
      return MCU_FLASHSTR("ConfiguredDevices");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == EManagementMethod::kUnknown) {
    return MCU_FLASHSTR("Unknown");
  }
  if (v == EManagementMethod::kDescription) {
    return MCU_FLASHSTR("Description");
  }
  if (v == EManagementMethod::kConfiguredDevices) {
    return MCU_FLASHSTR("ConfiguredDevices");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EDeviceType v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case EDeviceType::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case EDeviceType::kCamera:
      return MCU_FLASHSTR("Camera");
    case EDeviceType::kCoverCalibrator:
      return MCU_FLASHSTR("CoverCalibrator");
    case EDeviceType::kDome:
      return MCU_FLASHSTR("Dome");
    case EDeviceType::kFilterWheel:
      return MCU_FLASHSTR("FilterWheel");
    case EDeviceType::kFocuser:
      return MCU_FLASHSTR("Focuser");
    case EDeviceType::kObservingConditions:
      return MCU_FLASHSTR("ObservingConditions");
    case EDeviceType::kRotator:
      return MCU_FLASHSTR("Rotator");
    case EDeviceType::kSafetyMonitor:
      return MCU_FLASHSTR("SafetyMonitor");
    case EDeviceType::kSwitch:
      return MCU_FLASHSTR("Switch");
    case EDeviceType::kTelescope:
      return MCU_FLASHSTR("Telescope");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == EDeviceType::kUnknown) {
    return MCU_FLASHSTR("Unknown");
  }
  if (v == EDeviceType::kCamera) {
    return MCU_FLASHSTR("Camera");
  }
  if (v == EDeviceType::kCoverCalibrator) {
    return MCU_FLASHSTR("CoverCalibrator");
  }
  if (v == EDeviceType::kDome) {
    return MCU_FLASHSTR("Dome");
  }
  if (v == EDeviceType::kFilterWheel) {
    return MCU_FLASHSTR("FilterWheel");
  }
  if (v == EDeviceType::kFocuser) {
    return MCU_FLASHSTR("Focuser");
  }
  if (v == EDeviceType::kObservingConditions) {
    return MCU_FLASHSTR("ObservingConditions");
  }
  if (v == EDeviceType::kRotator) {
    return MCU_FLASHSTR("Rotator");
  }
  if (v == EDeviceType::kSafetyMonitor) {
    return MCU_FLASHSTR("SafetyMonitor");
  }
  if (v == EDeviceType::kSwitch) {
    return MCU_FLASHSTR("Switch");
  }
  if (v == EDeviceType::kTelescope) {
    return MCU_FLASHSTR("Telescope");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EDeviceMethod v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case EDeviceMethod::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case EDeviceMethod::kSetup:
      return MCU_FLASHSTR("Setup");
    case EDeviceMethod::kAction:
      return MCU_FLASHSTR("Action");
    case EDeviceMethod::kCommandBlind:
      return MCU_FLASHSTR("CommandBlind");
    case EDeviceMethod::kCommandBool:
      return MCU_FLASHSTR("CommandBool");
    case EDeviceMethod::kCommandString:
      return MCU_FLASHSTR("CommandString");
    case EDeviceMethod::kConnected:
      return MCU_FLASHSTR("Connected");
    case EDeviceMethod::kDescription:
      return MCU_FLASHSTR("Description");
    case EDeviceMethod::kDriverInfo:
      return MCU_FLASHSTR("DriverInfo");
    case EDeviceMethod::kDriverVersion:
      return MCU_FLASHSTR("DriverVersion");
    case EDeviceMethod::kInterfaceVersion:
      return MCU_FLASHSTR("InterfaceVersion");
    case EDeviceMethod::kName:
      return MCU_FLASHSTR("Name");
    case EDeviceMethod::kSupportedActions:
      return MCU_FLASHSTR("SupportedActions");
    case EDeviceMethod::kBrightness:
      return MCU_FLASHSTR("Brightness");
    case EDeviceMethod::kCalibratorState:
      return MCU_FLASHSTR("CalibratorState");
    case EDeviceMethod::kCoverState:
      return MCU_FLASHSTR("CoverState");
    case EDeviceMethod::kMaxBrightness:
      return MCU_FLASHSTR("MaxBrightness");
    case EDeviceMethod::kCalibratorOff:
      return MCU_FLASHSTR("CalibratorOff");
    case EDeviceMethod::kCalibratorOn:
      return MCU_FLASHSTR("CalibratorOn");
    case EDeviceMethod::kCloseCover:
      return MCU_FLASHSTR("CloseCover");
    case EDeviceMethod::kHaltCover:
      return MCU_FLASHSTR("HaltCover");
    case EDeviceMethod::kOpenCover:
      return MCU_FLASHSTR("OpenCover");
    case EDeviceMethod::kAveragePeriod:
      return MCU_FLASHSTR("AveragePeriod");
    case EDeviceMethod::kCloudCover:
      return MCU_FLASHSTR("CloudCover");
    case EDeviceMethod::kDewPoint:
      return MCU_FLASHSTR("DewPoint");
    case EDeviceMethod::kHumidity:
      return MCU_FLASHSTR("Humidity");
    case EDeviceMethod::kPressure:
      return MCU_FLASHSTR("Pressure");
    case EDeviceMethod::kRainRate:
      return MCU_FLASHSTR("RainRate");
    case EDeviceMethod::kRefresh:
      return MCU_FLASHSTR("Refresh");
    case EDeviceMethod::kSensorDescription:
      return MCU_FLASHSTR("SensorDescription");
    case EDeviceMethod::kSkyBrightness:
      return MCU_FLASHSTR("SkyBrightness");
    case EDeviceMethod::kSkyQuality:
      return MCU_FLASHSTR("SkyQuality");
    case EDeviceMethod::kSkyTemperature:
      return MCU_FLASHSTR("SkyTemperature");
    case EDeviceMethod::kStarFWHM:
      return MCU_FLASHSTR("StarFWHM");
    case EDeviceMethod::kTemperature:
      return MCU_FLASHSTR("Temperature");
    case EDeviceMethod::kTimeSinceLastUpdate:
      return MCU_FLASHSTR("TimeSinceLastUpdate");
    case EDeviceMethod::kWindDirection:
      return MCU_FLASHSTR("WindDirection");
    case EDeviceMethod::kWindGust:
      return MCU_FLASHSTR("WindGust");
    case EDeviceMethod::kWindSpeed:
      return MCU_FLASHSTR("WindSpeed");
    case EDeviceMethod::kIsSafe:
      return MCU_FLASHSTR("IsSafe");
    case EDeviceMethod::kMaxSwitch:
      return MCU_FLASHSTR("MaxSwitch");
    case EDeviceMethod::kCanWrite:
      return MCU_FLASHSTR("CanWrite");
    case EDeviceMethod::kGetSwitch:
      return MCU_FLASHSTR("GetSwitch");
    case EDeviceMethod::kGetSwitchDescription:
      return MCU_FLASHSTR("GetSwitchDescription");
    case EDeviceMethod::kGetSwitchName:
      return MCU_FLASHSTR("GetSwitchName");
    case EDeviceMethod::kGetSwitchValue:
      return MCU_FLASHSTR("GetSwitchValue");
    case EDeviceMethod::kMinSwitchValue:
      return MCU_FLASHSTR("MinSwitchValue");
    case EDeviceMethod::kMaxSwitchValue:
      return MCU_FLASHSTR("MaxSwitchValue");
    case EDeviceMethod::kSetSwitch:
      return MCU_FLASHSTR("SetSwitch");
    case EDeviceMethod::kSetSwitchName:
      return MCU_FLASHSTR("SetSwitchName");
    case EDeviceMethod::kSetSwitchValue:
      return MCU_FLASHSTR("SetSwitchValue");
    case EDeviceMethod::kSwitchStep:
      return MCU_FLASHSTR("SwitchStep");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == EDeviceMethod::kUnknown) {
    return MCU_FLASHSTR("Unknown");
  }
  if (v == EDeviceMethod::kSetup) {
    return MCU_FLASHSTR("Setup");
  }
  if (v == EDeviceMethod::kAction) {
    return MCU_FLASHSTR("Action");
  }
  if (v == EDeviceMethod::kCommandBlind) {
    return MCU_FLASHSTR("CommandBlind");
  }
  if (v == EDeviceMethod::kCommandBool) {
    return MCU_FLASHSTR("CommandBool");
  }
  if (v == EDeviceMethod::kCommandString) {
    return MCU_FLASHSTR("CommandString");
  }
  if (v == EDeviceMethod::kConnected) {
    return MCU_FLASHSTR("Connected");
  }
  if (v == EDeviceMethod::kDescription) {
    return MCU_FLASHSTR("Description");
  }
  if (v == EDeviceMethod::kDriverInfo) {
    return MCU_FLASHSTR("DriverInfo");
  }
  if (v == EDeviceMethod::kDriverVersion) {
    return MCU_FLASHSTR("DriverVersion");
  }
  if (v == EDeviceMethod::kInterfaceVersion) {
    return MCU_FLASHSTR("InterfaceVersion");
  }
  if (v == EDeviceMethod::kName) {
    return MCU_FLASHSTR("Name");
  }
  if (v == EDeviceMethod::kSupportedActions) {
    return MCU_FLASHSTR("SupportedActions");
  }
  if (v == EDeviceMethod::kBrightness) {
    return MCU_FLASHSTR("Brightness");
  }
  if (v == EDeviceMethod::kCalibratorState) {
    return MCU_FLASHSTR("CalibratorState");
  }
  if (v == EDeviceMethod::kCoverState) {
    return MCU_FLASHSTR("CoverState");
  }
  if (v == EDeviceMethod::kMaxBrightness) {
    return MCU_FLASHSTR("MaxBrightness");
  }
  if (v == EDeviceMethod::kCalibratorOff) {
    return MCU_FLASHSTR("CalibratorOff");
  }
  if (v == EDeviceMethod::kCalibratorOn) {
    return MCU_FLASHSTR("CalibratorOn");
  }
  if (v == EDeviceMethod::kCloseCover) {
    return MCU_FLASHSTR("CloseCover");
  }
  if (v == EDeviceMethod::kHaltCover) {
    return MCU_FLASHSTR("HaltCover");
  }
  if (v == EDeviceMethod::kOpenCover) {
    return MCU_FLASHSTR("OpenCover");
  }
  if (v == EDeviceMethod::kAveragePeriod) {
    return MCU_FLASHSTR("AveragePeriod");
  }
  if (v == EDeviceMethod::kCloudCover) {
    return MCU_FLASHSTR("CloudCover");
  }
  if (v == EDeviceMethod::kDewPoint) {
    return MCU_FLASHSTR("DewPoint");
  }
  if (v == EDeviceMethod::kHumidity) {
    return MCU_FLASHSTR("Humidity");
  }
  if (v == EDeviceMethod::kPressure) {
    return MCU_FLASHSTR("Pressure");
  }
  if (v == EDeviceMethod::kRainRate) {
    return MCU_FLASHSTR("RainRate");
  }
  if (v == EDeviceMethod::kRefresh) {
    return MCU_FLASHSTR("Refresh");
  }
  if (v == EDeviceMethod::kSensorDescription) {
    return MCU_FLASHSTR("SensorDescription");
  }
  if (v == EDeviceMethod::kSkyBrightness) {
    return MCU_FLASHSTR("SkyBrightness");
  }
  if (v == EDeviceMethod::kSkyQuality) {
    return MCU_FLASHSTR("SkyQuality");
  }
  if (v == EDeviceMethod::kSkyTemperature) {
    return MCU_FLASHSTR("SkyTemperature");
  }
  if (v == EDeviceMethod::kStarFWHM) {
    return MCU_FLASHSTR("StarFWHM");
  }
  if (v == EDeviceMethod::kTemperature) {
    return MCU_FLASHSTR("Temperature");
  }
  if (v == EDeviceMethod::kTimeSinceLastUpdate) {
    return MCU_FLASHSTR("TimeSinceLastUpdate");
  }
  if (v == EDeviceMethod::kWindDirection) {
    return MCU_FLASHSTR("WindDirection");
  }
  if (v == EDeviceMethod::kWindGust) {
    return MCU_FLASHSTR("WindGust");
  }
  if (v == EDeviceMethod::kWindSpeed) {
    return MCU_FLASHSTR("WindSpeed");
  }
  if (v == EDeviceMethod::kIsSafe) {
    return MCU_FLASHSTR("IsSafe");
  }
  if (v == EDeviceMethod::kMaxSwitch) {
    return MCU_FLASHSTR("MaxSwitch");
  }
  if (v == EDeviceMethod::kCanWrite) {
    return MCU_FLASHSTR("CanWrite");
  }
  if (v == EDeviceMethod::kGetSwitch) {
    return MCU_FLASHSTR("GetSwitch");
  }
  if (v == EDeviceMethod::kGetSwitchDescription) {
    return MCU_FLASHSTR("GetSwitchDescription");
  }
  if (v == EDeviceMethod::kGetSwitchName) {
    return MCU_FLASHSTR("GetSwitchName");
  }
  if (v == EDeviceMethod::kGetSwitchValue) {
    return MCU_FLASHSTR("GetSwitchValue");
  }
  if (v == EDeviceMethod::kMinSwitchValue) {
    return MCU_FLASHSTR("MinSwitchValue");
  }
  if (v == EDeviceMethod::kMaxSwitchValue) {
    return MCU_FLASHSTR("MaxSwitchValue");
  }
  if (v == EDeviceMethod::kSetSwitch) {
    return MCU_FLASHSTR("SetSwitch");
  }
  if (v == EDeviceMethod::kSetSwitchName) {
    return MCU_FLASHSTR("SetSwitchName");
  }
  if (v == EDeviceMethod::kSetSwitchValue) {
    return MCU_FLASHSTR("SetSwitchValue");
  }
  if (v == EDeviceMethod::kSwitchStep) {
    return MCU_FLASHSTR("SwitchStep");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EParameter v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case EParameter::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case EParameter::kAction:
      return MCU_FLASHSTR("Action");
    case EParameter::kClientID:
      return MCU_FLASHSTR("ClientID");
    case EParameter::kClientTransactionID:
      return MCU_FLASHSTR("ClientTransactionID");
    case EParameter::kCommand:
      return MCU_FLASHSTR("Command");
    case EParameter::kConnected:
      return MCU_FLASHSTR("Connected");
    case EParameter::kParameters:
      return MCU_FLASHSTR("Parameters");
    case EParameter::kRaw:
      return MCU_FLASHSTR("Raw");
    case EParameter::kBrightness:
      return MCU_FLASHSTR("Brightness");
    case EParameter::kAveragePeriod:
      return MCU_FLASHSTR("AveragePeriod");
    case EParameter::kSensorName:
      return MCU_FLASHSTR("SensorName");
    case EParameter::kId:
      return MCU_FLASHSTR("Id");
    case EParameter::kName:
      return MCU_FLASHSTR("Name");
    case EParameter::kState:
      return MCU_FLASHSTR("State");
    case EParameter::kValue:
      return MCU_FLASHSTR("Value");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == EParameter::kUnknown) {
    return MCU_FLASHSTR("Unknown");
  }
  if (v == EParameter::kAction) {
    return MCU_FLASHSTR("Action");
  }
  if (v == EParameter::kClientID) {
    return MCU_FLASHSTR("ClientID");
  }
  if (v == EParameter::kClientTransactionID) {
    return MCU_FLASHSTR("ClientTransactionID");
  }
  if (v == EParameter::kCommand) {
    return MCU_FLASHSTR("Command");
  }
  if (v == EParameter::kConnected) {
    return MCU_FLASHSTR("Connected");
  }
  if (v == EParameter::kParameters) {
    return MCU_FLASHSTR("Parameters");
  }
  if (v == EParameter::kRaw) {
    return MCU_FLASHSTR("Raw");
  }
  if (v == EParameter::kBrightness) {
    return MCU_FLASHSTR("Brightness");
  }
  if (v == EParameter::kAveragePeriod) {
    return MCU_FLASHSTR("AveragePeriod");
  }
  if (v == EParameter::kSensorName) {
    return MCU_FLASHSTR("SensorName");
  }
  if (v == EParameter::kId) {
    return MCU_FLASHSTR("Id");
  }
  if (v == EParameter::kName) {
    return MCU_FLASHSTR("Name");
  }
  if (v == EParameter::kState) {
    return MCU_FLASHSTR("State");
  }
  if (v == EParameter::kValue) {
    return MCU_FLASHSTR("Value");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(ESensorName v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case ESensorName::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case ESensorName::kCloudCover:
      return MCU_FLASHSTR("CloudCover");
    case ESensorName::kDewPoint:
      return MCU_FLASHSTR("DewPoint");
    case ESensorName::kHumidity:
      return MCU_FLASHSTR("Humidity");
    case ESensorName::kPressure:
      return MCU_FLASHSTR("Pressure");
    case ESensorName::kRainRate:
      return MCU_FLASHSTR("RainRate");
    case ESensorName::kSkyBrightness:
      return MCU_FLASHSTR("SkyBrightness");
    case ESensorName::kSkyQuality:
      return MCU_FLASHSTR("SkyQuality");
    case ESensorName::kSkyTemperature:
      return MCU_FLASHSTR("SkyTemperature");
    case ESensorName::kStarFWHM:
      return MCU_FLASHSTR("StarFWHM");
    case ESensorName::kTemperature:
      return MCU_FLASHSTR("Temperature");
    case ESensorName::kWindDirection:
      return MCU_FLASHSTR("WindDirection");
    case ESensorName::kWindGust:
      return MCU_FLASHSTR("WindGust");
    case ESensorName::kWindSpeed:
      return MCU_FLASHSTR("WindSpeed");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == ESensorName::kUnknown) {
    return MCU_FLASHSTR("Unknown");
  }
  if (v == ESensorName::kCloudCover) {
    return MCU_FLASHSTR("CloudCover");
  }
  if (v == ESensorName::kDewPoint) {
    return MCU_FLASHSTR("DewPoint");
  }
  if (v == ESensorName::kHumidity) {
    return MCU_FLASHSTR("Humidity");
  }
  if (v == ESensorName::kPressure) {
    return MCU_FLASHSTR("Pressure");
  }
  if (v == ESensorName::kRainRate) {
    return MCU_FLASHSTR("RainRate");
  }
  if (v == ESensorName::kSkyBrightness) {
    return MCU_FLASHSTR("SkyBrightness");
  }
  if (v == ESensorName::kSkyQuality) {
    return MCU_FLASHSTR("SkyQuality");
  }
  if (v == ESensorName::kSkyTemperature) {
    return MCU_FLASHSTR("SkyTemperature");
  }
  if (v == ESensorName::kStarFWHM) {
    return MCU_FLASHSTR("StarFWHM");
  }
  if (v == ESensorName::kTemperature) {
    return MCU_FLASHSTR("Temperature");
  }
  if (v == ESensorName::kWindDirection) {
    return MCU_FLASHSTR("WindDirection");
  }
  if (v == ESensorName::kWindGust) {
    return MCU_FLASHSTR("WindGust");
  }
  if (v == ESensorName::kWindSpeed) {
    return MCU_FLASHSTR("WindSpeed");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EHttpHeader v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case EHttpHeader::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case EHttpHeader::kConnection:
      return MCU_FLASHSTR("Connection");
    case EHttpHeader::kContentLength:
      return MCU_FLASHSTR("Content-Length");
    case EHttpHeader::kContentType:
      return MCU_FLASHSTR("Content-Type");
    case EHttpHeader::kDate:
      return MCU_FLASHSTR("Date");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == EHttpHeader::kUnknown) {
    return MCU_FLASHSTR("Unknown");
  }
  if (v == EHttpHeader::kConnection) {
    return MCU_FLASHSTR("Connection");
  }
  if (v == EHttpHeader::kContentLength) {
    return MCU_FLASHSTR("Content-Length");
  }
  if (v == EHttpHeader::kContentType) {
    return MCU_FLASHSTR("Content-Type");
  }
  if (v == EHttpHeader::kDate) {
    return MCU_FLASHSTR("Date");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EContentType v) {
#ifdef TO_FLASH_STRING_HELPER_USE_SWITCH
  switch (v) {
    case EContentType::kApplicationJson:
      return MCU_FLASHSTR("application/json");
    case EContentType::kTextPlain:
      return MCU_FLASHSTR("text/plain");
    case EContentType::kTextHtml:
      return MCU_FLASHSTR("text/html");
  }
#else   // !TO_FLASH_STRING_HELPER_USE_SWITCH
  if (v == EContentType::kApplicationJson) {
    return MCU_FLASHSTR("application/json");
  }
  if (v == EContentType::kTextPlain) {
    return MCU_FLASHSTR("text/plain");
  }
  if (v == EContentType::kTextHtml) {
    return MCU_FLASHSTR("text/html");
  }
#endif  // TO_FLASH_STRING_HELPER_USE_SWITCH
  return nullptr;
}

#if MCU_HOST_TARGET
// Support for debug logging of enums.

std::ostream& operator<<(std::ostream& os, RequestDecoderStatus v) {
  switch (v) {
    case RequestDecoderStatus::kReset:
      return os << "Reset";
    case RequestDecoderStatus::kDecoding:
      return os << "Decoding";
    case RequestDecoderStatus::kDecoded:
      return os << "Decoded";
  }
  return os << "Unknown RequestDecoderStatus, value="
            << static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& os, EHttpStatusCode v) {
  switch (v) {
    case EHttpStatusCode::kContinueDecoding:
      return os << "ContinueDecoding";
    case EHttpStatusCode::kNeedMoreInput:
      return os << "NeedMoreInput";
    case EHttpStatusCode::kHttpOk:
      return os << "HttpOk";
    case EHttpStatusCode::kHttpBadRequest:
      return os << "HttpBadRequest";
    case EHttpStatusCode::kHttpMethodNotAllowed:
      return os << "HttpMethodNotAllowed";
    case EHttpStatusCode::kHttpNotAcceptable:
      return os << "HttpNotAcceptable";
    case EHttpStatusCode::kHttpLengthRequired:
      return os << "HttpLengthRequired";
    case EHttpStatusCode::kHttpPayloadTooLarge:
      return os << "HttpPayloadTooLarge";
    case EHttpStatusCode::kHttpUnsupportedMediaType:
      return os << "HttpUnsupportedMediaType";
    case EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge:
      return os << "HttpRequestHeaderFieldsTooLarge";
    case EHttpStatusCode::kHttpInternalServerError:
      return os << "HttpInternalServerError";
    case EHttpStatusCode::kHttpMethodNotImplemented:
      return os << "HttpMethodNotImplemented";
    case EHttpStatusCode::kHttpVersionNotSupported:
      return os << "HttpVersionNotSupported";
  }
  return os << "Unknown EHttpStatusCode, value=" << static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& os, EHttpMethod v) {
  switch (v) {
    case EHttpMethod::kUnknown:
      return os << "Unknown";
    case EHttpMethod::GET:
      return os << "GET";
    case EHttpMethod::PUT:
      return os << "PUT";
    case EHttpMethod::HEAD:
      return os << "HEAD";
  }
  return os << "Unknown EHttpMethod, value=" << static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& os, EApiGroup v) {
  switch (v) {
    case EApiGroup::kUnknown:
      return os << "Unknown";
    case EApiGroup::kDevice:
      return os << "Device";
    case EApiGroup::kManagement:
      return os << "Management";
    case EApiGroup::kSetup:
      return os << "Setup";
  }
  return os << "Unknown EApiGroup, value=" << static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& os, EAlpacaApi v) {
  switch (v) {
    case EAlpacaApi::kUnknown:
      return os << "Unknown";
    case EAlpacaApi::kDeviceApi:
      return os << "DeviceApi";
    case EAlpacaApi::kDeviceSetup:
      return os << "DeviceSetup";
    case EAlpacaApi::kManagementApiVersions:
      return os << "ManagementApiVersions";
    case EAlpacaApi::kManagementDescription:
      return os << "ManagementDescription";
    case EAlpacaApi::kManagementConfiguredDevices:
      return os << "ManagementConfiguredDevices";
    case EAlpacaApi::kServerSetup:
      return os << "ServerSetup";
  }
  return os << "Unknown EAlpacaApi, value=" << static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& os, EManagementMethod v) {
  switch (v) {
    case EManagementMethod::kUnknown:
      return os << "Unknown";
    case EManagementMethod::kDescription:
      return os << "Description";
    case EManagementMethod::kConfiguredDevices:
      return os << "ConfiguredDevices";
  }
  return os << "Unknown EManagementMethod, value=" << static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& os, EDeviceType v) {
  switch (v) {
    case EDeviceType::kUnknown:
      return os << "Unknown";
    case EDeviceType::kCamera:
      return os << "Camera";
    case EDeviceType::kCoverCalibrator:
      return os << "CoverCalibrator";
    case EDeviceType::kDome:
      return os << "Dome";
    case EDeviceType::kFilterWheel:
      return os << "FilterWheel";
    case EDeviceType::kFocuser:
      return os << "Focuser";
    case EDeviceType::kObservingConditions:
      return os << "ObservingConditions";
    case EDeviceType::kRotator:
      return os << "Rotator";
    case EDeviceType::kSafetyMonitor:
      return os << "SafetyMonitor";
    case EDeviceType::kSwitch:
      return os << "Switch";
    case EDeviceType::kTelescope:
      return os << "Telescope";
  }
  return os << "Unknown EDeviceType, value=" << static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& os, EDeviceMethod v) {
  switch (v) {
    case EDeviceMethod::kUnknown:
      return os << "Unknown";
    case EDeviceMethod::kSetup:
      return os << "Setup";
    case EDeviceMethod::kAction:
      return os << "Action";
    case EDeviceMethod::kCommandBlind:
      return os << "CommandBlind";
    case EDeviceMethod::kCommandBool:
      return os << "CommandBool";
    case EDeviceMethod::kCommandString:
      return os << "CommandString";
    case EDeviceMethod::kConnected:
      return os << "Connected";
    case EDeviceMethod::kDescription:
      return os << "Description";
    case EDeviceMethod::kDriverInfo:
      return os << "DriverInfo";
    case EDeviceMethod::kDriverVersion:
      return os << "DriverVersion";
    case EDeviceMethod::kInterfaceVersion:
      return os << "InterfaceVersion";
    case EDeviceMethod::kName:
      return os << "Name";
    case EDeviceMethod::kSupportedActions:
      return os << "SupportedActions";
    case EDeviceMethod::kBrightness:
      return os << "Brightness";
    case EDeviceMethod::kCalibratorState:
      return os << "CalibratorState";
    case EDeviceMethod::kCoverState:
      return os << "CoverState";
    case EDeviceMethod::kMaxBrightness:
      return os << "MaxBrightness";
    case EDeviceMethod::kCalibratorOff:
      return os << "CalibratorOff";
    case EDeviceMethod::kCalibratorOn:
      return os << "CalibratorOn";
    case EDeviceMethod::kCloseCover:
      return os << "CloseCover";
    case EDeviceMethod::kHaltCover:
      return os << "HaltCover";
    case EDeviceMethod::kOpenCover:
      return os << "OpenCover";
    case EDeviceMethod::kAveragePeriod:
      return os << "AveragePeriod";
    case EDeviceMethod::kCloudCover:
      return os << "CloudCover";
    case EDeviceMethod::kDewPoint:
      return os << "DewPoint";
    case EDeviceMethod::kHumidity:
      return os << "Humidity";
    case EDeviceMethod::kPressure:
      return os << "Pressure";
    case EDeviceMethod::kRainRate:
      return os << "RainRate";
    case EDeviceMethod::kRefresh:
      return os << "Refresh";
    case EDeviceMethod::kSensorDescription:
      return os << "SensorDescription";
    case EDeviceMethod::kSkyBrightness:
      return os << "SkyBrightness";
    case EDeviceMethod::kSkyQuality:
      return os << "SkyQuality";
    case EDeviceMethod::kSkyTemperature:
      return os << "SkyTemperature";
    case EDeviceMethod::kStarFWHM:
      return os << "StarFWHM";
    case EDeviceMethod::kTemperature:
      return os << "Temperature";
    case EDeviceMethod::kTimeSinceLastUpdate:
      return os << "TimeSinceLastUpdate";
    case EDeviceMethod::kWindDirection:
      return os << "WindDirection";
    case EDeviceMethod::kWindGust:
      return os << "WindGust";
    case EDeviceMethod::kWindSpeed:
      return os << "WindSpeed";
    case EDeviceMethod::kIsSafe:
      return os << "IsSafe";
    case EDeviceMethod::kMaxSwitch:
      return os << "MaxSwitch";
    case EDeviceMethod::kCanWrite:
      return os << "CanWrite";
    case EDeviceMethod::kGetSwitch:
      return os << "GetSwitch";
    case EDeviceMethod::kGetSwitchDescription:
      return os << "GetSwitchDescription";
    case EDeviceMethod::kGetSwitchName:
      return os << "GetSwitchName";
    case EDeviceMethod::kGetSwitchValue:
      return os << "GetSwitchValue";
    case EDeviceMethod::kMinSwitchValue:
      return os << "MinSwitchValue";
    case EDeviceMethod::kMaxSwitchValue:
      return os << "MaxSwitchValue";
    case EDeviceMethod::kSetSwitch:
      return os << "SetSwitch";
    case EDeviceMethod::kSetSwitchName:
      return os << "SetSwitchName";
    case EDeviceMethod::kSetSwitchValue:
      return os << "SetSwitchValue";
    case EDeviceMethod::kSwitchStep:
      return os << "SwitchStep";
  }
  return os << "Unknown EDeviceMethod, value=" << static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& os, EParameter v) {
  switch (v) {
    case EParameter::kUnknown:
      return os << "Unknown";
    case EParameter::kAction:
      return os << "Action";
    case EParameter::kClientID:
      return os << "ClientID";
    case EParameter::kClientTransactionID:
      return os << "ClientTransactionID";
    case EParameter::kCommand:
      return os << "Command";
    case EParameter::kConnected:
      return os << "Connected";
    case EParameter::kParameters:
      return os << "Parameters";
    case EParameter::kRaw:
      return os << "Raw";
    case EParameter::kBrightness:
      return os << "Brightness";
    case EParameter::kAveragePeriod:
      return os << "AveragePeriod";
    case EParameter::kSensorName:
      return os << "SensorName";
    case EParameter::kId:
      return os << "Id";
    case EParameter::kName:
      return os << "Name";
    case EParameter::kState:
      return os << "State";
    case EParameter::kValue:
      return os << "Value";
  }
  return os << "Unknown EParameter, value=" << static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& os, ESensorName v) {
  switch (v) {
    case ESensorName::kUnknown:
      return os << "Unknown";
    case ESensorName::kCloudCover:
      return os << "CloudCover";
    case ESensorName::kDewPoint:
      return os << "DewPoint";
    case ESensorName::kHumidity:
      return os << "Humidity";
    case ESensorName::kPressure:
      return os << "Pressure";
    case ESensorName::kRainRate:
      return os << "RainRate";
    case ESensorName::kSkyBrightness:
      return os << "SkyBrightness";
    case ESensorName::kSkyQuality:
      return os << "SkyQuality";
    case ESensorName::kSkyTemperature:
      return os << "SkyTemperature";
    case ESensorName::kStarFWHM:
      return os << "StarFWHM";
    case ESensorName::kTemperature:
      return os << "Temperature";
    case ESensorName::kWindDirection:
      return os << "WindDirection";
    case ESensorName::kWindGust:
      return os << "WindGust";
    case ESensorName::kWindSpeed:
      return os << "WindSpeed";
  }
  return os << "Unknown ESensorName, value=" << static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& os, EHttpHeader v) {
  switch (v) {
    case EHttpHeader::kUnknown:
      return os << "Unknown";
    case EHttpHeader::kConnection:
      return os << "Connection";
    case EHttpHeader::kContentLength:
      return os << "Content-Length";
    case EHttpHeader::kContentType:
      return os << "Content-Type";
    case EHttpHeader::kDate:
      return os << "Date";
  }
  return os << "Unknown EHttpHeader, value=" << static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& os, EContentType v) {
  switch (v) {
    case EContentType::kApplicationJson:
      return os << "application/json";
    case EContentType::kTextPlain:
      return os << "text/plain";
    case EContentType::kTextHtml:
      return os << "text/html";
  }
  return os << "Unknown EContentType, value=" << static_cast<int64_t>(v);
}
#endif  // MCU_HOST_TARGET

}  // namespace alpaca
