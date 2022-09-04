#include "constants.h"

// (Mostly) GENERATED FILE. See make_enum_to_string.py

#if MCU_HOST_TARGET
#include <ostream>  // pragma: keep standard include
#endif

// BEGIN_SOURCE_GENERATED_BY_MAKE_ENUM_TO_STRING

#include <McuCore.h>

namespace alpaca {
namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(RequestDecoderStatus v) {
  switch (v) {
    case RequestDecoderStatus::kReset:
      return MCU_FLASHSTR("Reset");
    case RequestDecoderStatus::kDecoding:
      return MCU_FLASHSTR("Decoding");
    case RequestDecoderStatus::kDecoded:
      return MCU_FLASHSTR("Decoded");
  }
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(RequestDecoderStatus v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  if (v == RequestDecoderStatus::kReset) {
    return MCU_FLASHSTR("Reset");
  }
  if (v == RequestDecoderStatus::kDecoding) {
    return MCU_FLASHSTR("Decoding");
  }
  if (v == RequestDecoderStatus::kDecoded) {
    return MCU_FLASHSTR("Decoded");
  }
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(RequestDecoderStatus::kReset ==
                static_cast<RequestDecoderStatus>(0));
  static_assert(RequestDecoderStatus::kDecoding ==
                static_cast<RequestDecoderStatus>(1));
  static_assert(RequestDecoderStatus::kDecoded ==
                static_cast<RequestDecoderStatus>(2));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("Reset"),     // 0: kReset
      MCU_PSD("Decoding"),  // 1: kDecoding
      MCU_PSD("Decoded"),   // 2: kDecoded
  );
  return mcucore::LookupFlashStringForDenseEnum<uint_fast8_t>(
      flash_string_table, RequestDecoderStatus::kReset,
      RequestDecoderStatus::kDecoded, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(EHttpStatusCode v) {
  switch (v) {
    case EHttpStatusCode::kContinueDecoding:
      return MCU_FLASHSTR("ContinueDecoding");
    case EHttpStatusCode::kNeedMoreInput:
      return MCU_FLASHSTR("NeedMoreInput");
    case EHttpStatusCode::kHttpOk:
      return MCU_FLASHSTR("OK");
    case EHttpStatusCode::kHttpBadRequest:
      return MCU_FLASHSTR("Bad Request");
    case EHttpStatusCode::kHttpNotFound:
      return MCU_FLASHSTR("Not Found");
    case EHttpStatusCode::kHttpMethodNotAllowed:
      return MCU_FLASHSTR("Method Not Allowed");
    case EHttpStatusCode::kHttpNotAcceptable:
      return MCU_FLASHSTR("Not Acceptable");
    case EHttpStatusCode::kHttpLengthRequired:
      return MCU_FLASHSTR("Length Required");
    case EHttpStatusCode::kHttpPayloadTooLarge:
      return MCU_FLASHSTR("Payload Too Large");
    case EHttpStatusCode::kHttpUnsupportedMediaType:
      return MCU_FLASHSTR("Unsupported Media Type");
    case EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge:
      return MCU_FLASHSTR("Request Header Fields Too Large");
    case EHttpStatusCode::kHttpInternalServerError:
      return MCU_FLASHSTR("Internal Server Error");
    case EHttpStatusCode::kHttpNotImplemented:
      return MCU_FLASHSTR("Not Implemented");
    case EHttpStatusCode::kHttpVersionNotSupported:
      return MCU_FLASHSTR("HTTP Version Not Supported");
  }
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(EHttpStatusCode v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else   // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
  if (v == EHttpStatusCode::kContinueDecoding) {
    return MCU_FLASHSTR("ContinueDecoding");
  }
  if (v == EHttpStatusCode::kNeedMoreInput) {
    return MCU_FLASHSTR("NeedMoreInput");
  }
  if (v == EHttpStatusCode::kHttpOk) {
    return MCU_FLASHSTR("OK");
  }
  if (v == EHttpStatusCode::kHttpBadRequest) {
    return MCU_FLASHSTR("Bad Request");
  }
  if (v == EHttpStatusCode::kHttpNotFound) {
    return MCU_FLASHSTR("Not Found");
  }
  if (v == EHttpStatusCode::kHttpMethodNotAllowed) {
    return MCU_FLASHSTR("Method Not Allowed");
  }
  if (v == EHttpStatusCode::kHttpNotAcceptable) {
    return MCU_FLASHSTR("Not Acceptable");
  }
  if (v == EHttpStatusCode::kHttpLengthRequired) {
    return MCU_FLASHSTR("Length Required");
  }
  if (v == EHttpStatusCode::kHttpPayloadTooLarge) {
    return MCU_FLASHSTR("Payload Too Large");
  }
  if (v == EHttpStatusCode::kHttpUnsupportedMediaType) {
    return MCU_FLASHSTR("Unsupported Media Type");
  }
  if (v == EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge) {
    return MCU_FLASHSTR("Request Header Fields Too Large");
  }
  if (v == EHttpStatusCode::kHttpInternalServerError) {
    return MCU_FLASHSTR("Internal Server Error");
  }
  if (v == EHttpStatusCode::kHttpNotImplemented) {
    return MCU_FLASHSTR("Not Implemented");
  }
  if (v == EHttpStatusCode::kHttpVersionNotSupported) {
    return MCU_FLASHSTR("HTTP Version Not Supported");
  }
  return nullptr;
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(EHttpMethod v) {
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
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(EHttpMethod v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
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
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(EHttpMethod::kUnknown == static_cast<EHttpMethod>(0));
  static_assert(EHttpMethod::GET == static_cast<EHttpMethod>(1));
  static_assert(EHttpMethod::PUT == static_cast<EHttpMethod>(2));
  static_assert(EHttpMethod::HEAD == static_cast<EHttpMethod>(3));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("Unknown"),  // 0: kUnknown
      MCU_PSD("GET"),      // 1: GET
      MCU_PSD("PUT"),      // 2: PUT
      MCU_PSD("HEAD"),     // 3: HEAD
  );
  return mcucore::LookupFlashStringForDenseEnum<uint_fast8_t>(
      flash_string_table, EHttpMethod::kUnknown, EHttpMethod::HEAD, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(EApiGroup v) {
  switch (v) {
    case EApiGroup::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case EApiGroup::kDevice:
      return MCU_FLASHSTR("Device");
    case EApiGroup::kManagement:
      return MCU_FLASHSTR("Management");
    case EApiGroup::kSetup:
      return MCU_FLASHSTR("Setup");
    case EApiGroup::kAsset:
      return MCU_FLASHSTR("Asset");
    case EApiGroup::kServerStatus:
      return MCU_FLASHSTR("ServerStatus");
  }
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(EApiGroup v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
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
  if (v == EApiGroup::kAsset) {
    return MCU_FLASHSTR("Asset");
  }
  if (v == EApiGroup::kServerStatus) {
    return MCU_FLASHSTR("ServerStatus");
  }
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(EApiGroup::kUnknown == static_cast<EApiGroup>(0));
  static_assert(EApiGroup::kDevice == static_cast<EApiGroup>(1));
  static_assert(EApiGroup::kManagement == static_cast<EApiGroup>(2));
  static_assert(EApiGroup::kSetup == static_cast<EApiGroup>(3));
  static_assert(EApiGroup::kAsset == static_cast<EApiGroup>(4));
  static_assert(EApiGroup::kServerStatus == static_cast<EApiGroup>(5));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("Unknown"),       // 0: kUnknown
      MCU_PSD("Device"),        // 1: kDevice
      MCU_PSD("Management"),    // 2: kManagement
      MCU_PSD("Setup"),         // 3: kSetup
      MCU_PSD("Asset"),         // 4: kAsset
      MCU_PSD("ServerStatus"),  // 5: kServerStatus
  );
  return mcucore::LookupFlashStringForDenseEnum<uint_fast8_t>(
      flash_string_table, EApiGroup::kUnknown, EApiGroup::kServerStatus, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(EAlpacaApi v) {
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
    case EAlpacaApi::kAsset:
      return MCU_FLASHSTR("Asset");
    case EAlpacaApi::kServerSetup:
      return MCU_FLASHSTR("ServerSetup");
    case EAlpacaApi::kServerStatus:
      return MCU_FLASHSTR("ServerStatus");
  }
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(EAlpacaApi v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
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
  if (v == EAlpacaApi::kAsset) {
    return MCU_FLASHSTR("Asset");
  }
  if (v == EAlpacaApi::kServerSetup) {
    return MCU_FLASHSTR("ServerSetup");
  }
  if (v == EAlpacaApi::kServerStatus) {
    return MCU_FLASHSTR("ServerStatus");
  }
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(EAlpacaApi::kUnknown == static_cast<EAlpacaApi>(0));
  static_assert(EAlpacaApi::kDeviceApi == static_cast<EAlpacaApi>(1));
  static_assert(EAlpacaApi::kDeviceSetup == static_cast<EAlpacaApi>(2));
  static_assert(EAlpacaApi::kManagementApiVersions ==
                static_cast<EAlpacaApi>(3));
  static_assert(EAlpacaApi::kManagementDescription ==
                static_cast<EAlpacaApi>(4));
  static_assert(EAlpacaApi::kManagementConfiguredDevices ==
                static_cast<EAlpacaApi>(5));
  static_assert(EAlpacaApi::kAsset == static_cast<EAlpacaApi>(6));
  static_assert(EAlpacaApi::kServerSetup == static_cast<EAlpacaApi>(7));
  static_assert(EAlpacaApi::kServerStatus == static_cast<EAlpacaApi>(8));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("Unknown"),                // 0: kUnknown
      MCU_PSD("DeviceApi"),              // 1: kDeviceApi
      MCU_PSD("DeviceSetup"),            // 2: kDeviceSetup
      MCU_PSD("ManagementApiVersions"),  // 3: kManagementApiVersions
      MCU_PSD("ManagementDescription"),  // 4: kManagementDescription
      MCU_PSD(
          "ManagementConfiguredDevices"),  // 5: kManagementConfiguredDevices
      MCU_PSD("Asset"),                    // 6: kAsset
      MCU_PSD("ServerSetup"),              // 7: kServerSetup
      MCU_PSD("ServerStatus"),             // 8: kServerStatus
  );
  return mcucore::LookupFlashStringForDenseEnum<uint_fast8_t>(
      flash_string_table, EAlpacaApi::kUnknown, EAlpacaApi::kServerStatus, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(EManagementMethod v) {
  switch (v) {
    case EManagementMethod::kUnknown:
      return MCU_FLASHSTR("Unknown");
    case EManagementMethod::kDescription:
      return MCU_FLASHSTR("Description");
    case EManagementMethod::kConfiguredDevices:
      return MCU_FLASHSTR("ConfiguredDevices");
  }
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(EManagementMethod v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  if (v == EManagementMethod::kUnknown) {
    return MCU_FLASHSTR("Unknown");
  }
  if (v == EManagementMethod::kDescription) {
    return MCU_FLASHSTR("Description");
  }
  if (v == EManagementMethod::kConfiguredDevices) {
    return MCU_FLASHSTR("ConfiguredDevices");
  }
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(EManagementMethod::kUnknown ==
                static_cast<EManagementMethod>(0));
  static_assert(EManagementMethod::kDescription ==
                static_cast<EManagementMethod>(1));
  static_assert(EManagementMethod::kConfiguredDevices ==
                static_cast<EManagementMethod>(2));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("Unknown"),            // 0: kUnknown
      MCU_PSD("Description"),        // 1: kDescription
      MCU_PSD("ConfiguredDevices"),  // 2: kConfiguredDevices
  );
  return mcucore::LookupFlashStringForDenseEnum<uint_fast8_t>(
      flash_string_table, EManagementMethod::kUnknown,
      EManagementMethod::kConfiguredDevices, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(EDeviceType v) {
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
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(EDeviceType v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
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
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(EDeviceType::kUnknown == static_cast<EDeviceType>(0));
  static_assert(EDeviceType::kCamera == static_cast<EDeviceType>(1));
  static_assert(EDeviceType::kCoverCalibrator == static_cast<EDeviceType>(2));
  static_assert(EDeviceType::kDome == static_cast<EDeviceType>(3));
  static_assert(EDeviceType::kFilterWheel == static_cast<EDeviceType>(4));
  static_assert(EDeviceType::kFocuser == static_cast<EDeviceType>(5));
  static_assert(EDeviceType::kObservingConditions ==
                static_cast<EDeviceType>(6));
  static_assert(EDeviceType::kRotator == static_cast<EDeviceType>(7));
  static_assert(EDeviceType::kSafetyMonitor == static_cast<EDeviceType>(8));
  static_assert(EDeviceType::kSwitch == static_cast<EDeviceType>(9));
  static_assert(EDeviceType::kTelescope == static_cast<EDeviceType>(10));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("Unknown"),              // 0: kUnknown
      MCU_PSD("Camera"),               // 1: kCamera
      MCU_PSD("CoverCalibrator"),      // 2: kCoverCalibrator
      MCU_PSD("Dome"),                 // 3: kDome
      MCU_PSD("FilterWheel"),          // 4: kFilterWheel
      MCU_PSD("Focuser"),              // 5: kFocuser
      MCU_PSD("ObservingConditions"),  // 6: kObservingConditions
      MCU_PSD("Rotator"),              // 7: kRotator
      MCU_PSD("SafetyMonitor"),        // 8: kSafetyMonitor
      MCU_PSD("Switch"),               // 9: kSwitch
      MCU_PSD("Telescope"),            // 10: kTelescope
  );
  return mcucore::LookupFlashStringForDenseEnum<uint_fast8_t>(
      flash_string_table, EDeviceType::kUnknown, EDeviceType::kTelescope, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(EDeviceMethod v) {
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
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(EDeviceMethod v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
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
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(EDeviceMethod::kUnknown == static_cast<EDeviceMethod>(0));
  static_assert(EDeviceMethod::kSetup == static_cast<EDeviceMethod>(1));
  static_assert(EDeviceMethod::kAction == static_cast<EDeviceMethod>(2));
  static_assert(EDeviceMethod::kCommandBlind == static_cast<EDeviceMethod>(3));
  static_assert(EDeviceMethod::kCommandBool == static_cast<EDeviceMethod>(4));
  static_assert(EDeviceMethod::kCommandString == static_cast<EDeviceMethod>(5));
  static_assert(EDeviceMethod::kConnected == static_cast<EDeviceMethod>(6));
  static_assert(EDeviceMethod::kDescription == static_cast<EDeviceMethod>(7));
  static_assert(EDeviceMethod::kDriverInfo == static_cast<EDeviceMethod>(8));
  static_assert(EDeviceMethod::kDriverVersion == static_cast<EDeviceMethod>(9));
  static_assert(EDeviceMethod::kInterfaceVersion ==
                static_cast<EDeviceMethod>(10));
  static_assert(EDeviceMethod::kName == static_cast<EDeviceMethod>(11));
  static_assert(EDeviceMethod::kSupportedActions ==
                static_cast<EDeviceMethod>(12));
  static_assert(EDeviceMethod::kBrightness == static_cast<EDeviceMethod>(13));
  static_assert(EDeviceMethod::kCalibratorState ==
                static_cast<EDeviceMethod>(14));
  static_assert(EDeviceMethod::kCoverState == static_cast<EDeviceMethod>(15));
  static_assert(EDeviceMethod::kMaxBrightness ==
                static_cast<EDeviceMethod>(16));
  static_assert(EDeviceMethod::kCalibratorOff ==
                static_cast<EDeviceMethod>(17));
  static_assert(EDeviceMethod::kCalibratorOn == static_cast<EDeviceMethod>(18));
  static_assert(EDeviceMethod::kCloseCover == static_cast<EDeviceMethod>(19));
  static_assert(EDeviceMethod::kHaltCover == static_cast<EDeviceMethod>(20));
  static_assert(EDeviceMethod::kOpenCover == static_cast<EDeviceMethod>(21));
  static_assert(EDeviceMethod::kAveragePeriod ==
                static_cast<EDeviceMethod>(22));
  static_assert(EDeviceMethod::kCloudCover == static_cast<EDeviceMethod>(23));
  static_assert(EDeviceMethod::kDewPoint == static_cast<EDeviceMethod>(24));
  static_assert(EDeviceMethod::kHumidity == static_cast<EDeviceMethod>(25));
  static_assert(EDeviceMethod::kPressure == static_cast<EDeviceMethod>(26));
  static_assert(EDeviceMethod::kRainRate == static_cast<EDeviceMethod>(27));
  static_assert(EDeviceMethod::kRefresh == static_cast<EDeviceMethod>(28));
  static_assert(EDeviceMethod::kSensorDescription ==
                static_cast<EDeviceMethod>(29));
  static_assert(EDeviceMethod::kSkyBrightness ==
                static_cast<EDeviceMethod>(30));
  static_assert(EDeviceMethod::kSkyQuality == static_cast<EDeviceMethod>(31));
  static_assert(EDeviceMethod::kSkyTemperature ==
                static_cast<EDeviceMethod>(32));
  static_assert(EDeviceMethod::kStarFWHM == static_cast<EDeviceMethod>(33));
  static_assert(EDeviceMethod::kTemperature == static_cast<EDeviceMethod>(34));
  static_assert(EDeviceMethod::kTimeSinceLastUpdate ==
                static_cast<EDeviceMethod>(35));
  static_assert(EDeviceMethod::kWindDirection ==
                static_cast<EDeviceMethod>(36));
  static_assert(EDeviceMethod::kWindGust == static_cast<EDeviceMethod>(37));
  static_assert(EDeviceMethod::kWindSpeed == static_cast<EDeviceMethod>(38));
  static_assert(EDeviceMethod::kIsSafe == static_cast<EDeviceMethod>(39));
  static_assert(EDeviceMethod::kMaxSwitch == static_cast<EDeviceMethod>(40));
  static_assert(EDeviceMethod::kCanWrite == static_cast<EDeviceMethod>(41));
  static_assert(EDeviceMethod::kGetSwitch == static_cast<EDeviceMethod>(42));
  static_assert(EDeviceMethod::kGetSwitchDescription ==
                static_cast<EDeviceMethod>(43));
  static_assert(EDeviceMethod::kGetSwitchName ==
                static_cast<EDeviceMethod>(44));
  static_assert(EDeviceMethod::kGetSwitchValue ==
                static_cast<EDeviceMethod>(45));
  static_assert(EDeviceMethod::kMinSwitchValue ==
                static_cast<EDeviceMethod>(46));
  static_assert(EDeviceMethod::kMaxSwitchValue ==
                static_cast<EDeviceMethod>(47));
  static_assert(EDeviceMethod::kSetSwitch == static_cast<EDeviceMethod>(48));
  static_assert(EDeviceMethod::kSetSwitchName ==
                static_cast<EDeviceMethod>(49));
  static_assert(EDeviceMethod::kSetSwitchValue ==
                static_cast<EDeviceMethod>(50));
  static_assert(EDeviceMethod::kSwitchStep == static_cast<EDeviceMethod>(51));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("Unknown"),               // 0: kUnknown
      MCU_PSD("Setup"),                 // 1: kSetup
      MCU_PSD("Action"),                // 2: kAction
      MCU_PSD("CommandBlind"),          // 3: kCommandBlind
      MCU_PSD("CommandBool"),           // 4: kCommandBool
      MCU_PSD("CommandString"),         // 5: kCommandString
      MCU_PSD("Connected"),             // 6: kConnected
      MCU_PSD("Description"),           // 7: kDescription
      MCU_PSD("DriverInfo"),            // 8: kDriverInfo
      MCU_PSD("DriverVersion"),         // 9: kDriverVersion
      MCU_PSD("InterfaceVersion"),      // 10: kInterfaceVersion
      MCU_PSD("Name"),                  // 11: kName
      MCU_PSD("SupportedActions"),      // 12: kSupportedActions
      MCU_PSD("Brightness"),            // 13: kBrightness
      MCU_PSD("CalibratorState"),       // 14: kCalibratorState
      MCU_PSD("CoverState"),            // 15: kCoverState
      MCU_PSD("MaxBrightness"),         // 16: kMaxBrightness
      MCU_PSD("CalibratorOff"),         // 17: kCalibratorOff
      MCU_PSD("CalibratorOn"),          // 18: kCalibratorOn
      MCU_PSD("CloseCover"),            // 19: kCloseCover
      MCU_PSD("HaltCover"),             // 20: kHaltCover
      MCU_PSD("OpenCover"),             // 21: kOpenCover
      MCU_PSD("AveragePeriod"),         // 22: kAveragePeriod
      MCU_PSD("CloudCover"),            // 23: kCloudCover
      MCU_PSD("DewPoint"),              // 24: kDewPoint
      MCU_PSD("Humidity"),              // 25: kHumidity
      MCU_PSD("Pressure"),              // 26: kPressure
      MCU_PSD("RainRate"),              // 27: kRainRate
      MCU_PSD("Refresh"),               // 28: kRefresh
      MCU_PSD("SensorDescription"),     // 29: kSensorDescription
      MCU_PSD("SkyBrightness"),         // 30: kSkyBrightness
      MCU_PSD("SkyQuality"),            // 31: kSkyQuality
      MCU_PSD("SkyTemperature"),        // 32: kSkyTemperature
      MCU_PSD("StarFWHM"),              // 33: kStarFWHM
      MCU_PSD("Temperature"),           // 34: kTemperature
      MCU_PSD("TimeSinceLastUpdate"),   // 35: kTimeSinceLastUpdate
      MCU_PSD("WindDirection"),         // 36: kWindDirection
      MCU_PSD("WindGust"),              // 37: kWindGust
      MCU_PSD("WindSpeed"),             // 38: kWindSpeed
      MCU_PSD("IsSafe"),                // 39: kIsSafe
      MCU_PSD("MaxSwitch"),             // 40: kMaxSwitch
      MCU_PSD("CanWrite"),              // 41: kCanWrite
      MCU_PSD("GetSwitch"),             // 42: kGetSwitch
      MCU_PSD("GetSwitchDescription"),  // 43: kGetSwitchDescription
      MCU_PSD("GetSwitchName"),         // 44: kGetSwitchName
      MCU_PSD("GetSwitchValue"),        // 45: kGetSwitchValue
      MCU_PSD("MinSwitchValue"),        // 46: kMinSwitchValue
      MCU_PSD("MaxSwitchValue"),        // 47: kMaxSwitchValue
      MCU_PSD("SetSwitch"),             // 48: kSetSwitch
      MCU_PSD("SetSwitchName"),         // 49: kSetSwitchName
      MCU_PSD("SetSwitchValue"),        // 50: kSetSwitchValue
      MCU_PSD("SwitchStep"),            // 51: kSwitchStep
  );
  return mcucore::LookupFlashStringForDenseEnum<uint_fast8_t>(
      flash_string_table, EDeviceMethod::kUnknown, EDeviceMethod::kSwitchStep,
      v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(EParameter v) {
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
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(EParameter v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
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
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(EParameter::kUnknown == static_cast<EParameter>(0));
  static_assert(EParameter::kAction == static_cast<EParameter>(1));
  static_assert(EParameter::kClientID == static_cast<EParameter>(2));
  static_assert(EParameter::kClientTransactionID == static_cast<EParameter>(3));
  static_assert(EParameter::kCommand == static_cast<EParameter>(4));
  static_assert(EParameter::kConnected == static_cast<EParameter>(5));
  static_assert(EParameter::kParameters == static_cast<EParameter>(6));
  static_assert(EParameter::kRaw == static_cast<EParameter>(7));
  static_assert(EParameter::kBrightness == static_cast<EParameter>(8));
  static_assert(EParameter::kAveragePeriod == static_cast<EParameter>(9));
  static_assert(EParameter::kSensorName == static_cast<EParameter>(10));
  static_assert(EParameter::kId == static_cast<EParameter>(11));
  static_assert(EParameter::kName == static_cast<EParameter>(12));
  static_assert(EParameter::kState == static_cast<EParameter>(13));
  static_assert(EParameter::kValue == static_cast<EParameter>(14));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("Unknown"),              // 0: kUnknown
      MCU_PSD("Action"),               // 1: kAction
      MCU_PSD("ClientID"),             // 2: kClientID
      MCU_PSD("ClientTransactionID"),  // 3: kClientTransactionID
      MCU_PSD("Command"),              // 4: kCommand
      MCU_PSD("Connected"),            // 5: kConnected
      MCU_PSD("Parameters"),           // 6: kParameters
      MCU_PSD("Raw"),                  // 7: kRaw
      MCU_PSD("Brightness"),           // 8: kBrightness
      MCU_PSD("AveragePeriod"),        // 9: kAveragePeriod
      MCU_PSD("SensorName"),           // 10: kSensorName
      MCU_PSD("Id"),                   // 11: kId
      MCU_PSD("Name"),                 // 12: kName
      MCU_PSD("State"),                // 13: kState
      MCU_PSD("Value"),                // 14: kValue
  );
  return mcucore::LookupFlashStringForDenseEnum<uint_fast8_t>(
      flash_string_table, EParameter::kUnknown, EParameter::kValue, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(ESensorName v) {
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
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(ESensorName v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
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
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(ESensorName::kUnknown == static_cast<ESensorName>(0));
  static_assert(ESensorName::kCloudCover == static_cast<ESensorName>(1));
  static_assert(ESensorName::kDewPoint == static_cast<ESensorName>(2));
  static_assert(ESensorName::kHumidity == static_cast<ESensorName>(3));
  static_assert(ESensorName::kPressure == static_cast<ESensorName>(4));
  static_assert(ESensorName::kRainRate == static_cast<ESensorName>(5));
  static_assert(ESensorName::kSkyBrightness == static_cast<ESensorName>(6));
  static_assert(ESensorName::kSkyQuality == static_cast<ESensorName>(7));
  static_assert(ESensorName::kSkyTemperature == static_cast<ESensorName>(8));
  static_assert(ESensorName::kStarFWHM == static_cast<ESensorName>(9));
  static_assert(ESensorName::kTemperature == static_cast<ESensorName>(10));
  static_assert(ESensorName::kWindDirection == static_cast<ESensorName>(11));
  static_assert(ESensorName::kWindGust == static_cast<ESensorName>(12));
  static_assert(ESensorName::kWindSpeed == static_cast<ESensorName>(13));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("Unknown"),         // 0: kUnknown
      MCU_PSD("CloudCover"),      // 1: kCloudCover
      MCU_PSD("DewPoint"),        // 2: kDewPoint
      MCU_PSD("Humidity"),        // 3: kHumidity
      MCU_PSD("Pressure"),        // 4: kPressure
      MCU_PSD("RainRate"),        // 5: kRainRate
      MCU_PSD("SkyBrightness"),   // 6: kSkyBrightness
      MCU_PSD("SkyQuality"),      // 7: kSkyQuality
      MCU_PSD("SkyTemperature"),  // 8: kSkyTemperature
      MCU_PSD("StarFWHM"),        // 9: kStarFWHM
      MCU_PSD("Temperature"),     // 10: kTemperature
      MCU_PSD("WindDirection"),   // 11: kWindDirection
      MCU_PSD("WindGust"),        // 12: kWindGust
      MCU_PSD("WindSpeed"),       // 13: kWindSpeed
  );
  return mcucore::LookupFlashStringForDenseEnum<uint_fast8_t>(
      flash_string_table, ESensorName::kUnknown, ESensorName::kWindSpeed, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(EHttpHeader v) {
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
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(EHttpHeader v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
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
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(EHttpHeader::kUnknown == static_cast<EHttpHeader>(0));
  static_assert(EHttpHeader::kConnection == static_cast<EHttpHeader>(1));
  static_assert(EHttpHeader::kContentLength == static_cast<EHttpHeader>(2));
  static_assert(EHttpHeader::kContentType == static_cast<EHttpHeader>(3));
  static_assert(EHttpHeader::kDate == static_cast<EHttpHeader>(4));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("Unknown"),         // 0: kUnknown
      MCU_PSD("Connection"),      // 1: kConnection
      MCU_PSD("Content-Length"),  // 2: kContentLength
      MCU_PSD("Content-Type"),    // 3: kContentType
      MCU_PSD("Date"),            // 4: kDate
  );
  return mcucore::LookupFlashStringForDenseEnum<uint_fast8_t>(
      flash_string_table, EHttpHeader::kUnknown, EHttpHeader::kDate, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(EContentType v) {
  switch (v) {
    case EContentType::kApplicationJson:
      return MCU_FLASHSTR("application/json");
    case EContentType::kTextPlain:
      return MCU_FLASHSTR("text/plain");
    case EContentType::kTextHtml:
      return MCU_FLASHSTR("text/html");
  }
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(EContentType v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  if (v == EContentType::kApplicationJson) {
    return MCU_FLASHSTR("application/json");
  }
  if (v == EContentType::kTextPlain) {
    return MCU_FLASHSTR("text/plain");
  }
  if (v == EContentType::kTextHtml) {
    return MCU_FLASHSTR("text/html");
  }
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(EContentType::kApplicationJson == static_cast<EContentType>(0));
  static_assert(EContentType::kTextPlain == static_cast<EContentType>(1));
  static_assert(EContentType::kTextHtml == static_cast<EContentType>(2));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("application/json"),  // 0: kApplicationJson
      MCU_PSD("text/plain"),        // 1: kTextPlain
      MCU_PSD("text/html"),         // 2: kTextHtml
  );
  return mcucore::LookupFlashStringForDenseEnum<uint_fast8_t>(
      flash_string_table, EContentType::kApplicationJson,
      EContentType::kTextHtml, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(EHtmlPageSection v) {
  switch (v) {
    case EHtmlPageSection::kHead:
      return MCU_FLASHSTR("Head");
    case EHtmlPageSection::kBody:
      return MCU_FLASHSTR("Body");
    case EHtmlPageSection::kTrailer:
      return MCU_FLASHSTR("Trailer");
  }
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(EHtmlPageSection v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  if (v == EHtmlPageSection::kHead) {
    return MCU_FLASHSTR("Head");
  }
  if (v == EHtmlPageSection::kBody) {
    return MCU_FLASHSTR("Body");
  }
  if (v == EHtmlPageSection::kTrailer) {
    return MCU_FLASHSTR("Trailer");
  }
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(EHtmlPageSection::kHead == static_cast<EHtmlPageSection>(0));
  static_assert(EHtmlPageSection::kBody == static_cast<EHtmlPageSection>(1));
  static_assert(EHtmlPageSection::kTrailer == static_cast<EHtmlPageSection>(2));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("Head"),     // 0: kHead
      MCU_PSD("Body"),     // 1: kBody
      MCU_PSD("Trailer"),  // 2: kTrailer
  );
  return mcucore::LookupFlashStringForDenseEnum<uint_fast8_t>(
      flash_string_table, EHtmlPageSection::kHead, EHtmlPageSection::kTrailer,
      v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

namespace {

MCU_MAYBE_UNUSED_FUNCTION inline const __FlashStringHelper*
_ToFlashStringHelperViaSwitch(EDeviceEepromTagId v) {
  switch (v) {
    case EDeviceEepromTagId::kUniqueId:
      return MCU_FLASHSTR("UniqueId");
  }
  return nullptr;
}

}  // namespace

const __FlashStringHelper* ToFlashStringHelper(EDeviceEepromTagId v) {
#ifdef TO_FLASH_STRING_HELPER_PREFER_SWITCH
  return _ToFlashStringHelperViaSwitch(v);
#else  // not TO_FLASH_STRING_HELPER_PREFER_SWITCH
#ifdef TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  if (v == EDeviceEepromTagId::kUniqueId) {
    return MCU_FLASHSTR("UniqueId");
  }
  return nullptr;
#else   // not TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
  // Protection against enumerator definitions changing:
  static_assert(EDeviceEepromTagId::kUniqueId ==
                static_cast<EDeviceEepromTagId>(1));
  static MCU_FLASH_STRING_TABLE(  // Force new line.
      flash_string_table,
      MCU_PSD("UniqueId"),  // 1: kUniqueId
  );
  return mcucore::LookupFlashStringForDenseEnum<uint8_t>(
      flash_string_table, EDeviceEepromTagId::kUniqueId,
      EDeviceEepromTagId::kUniqueId, v);
#endif  // TO_FLASH_STRING_HELPER_PREFER_IF_STATEMENTS
#endif  // TO_FLASH_STRING_HELPER_PREFER_SWITCH
}

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

size_t PrintValueTo(EHtmlPageSection v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("EHtmlPageSection"),
                                          static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EDeviceEepromTagId v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return mcucore::PrintUnknownEnumValueTo(MCU_FLASHSTR("EDeviceEepromTagId"),
                                          static_cast<uint32_t>(v), out);
}

#if MCU_HOST_TARGET
// Support for debug logging of enums.

std::ostream& operator<<(std::ostream& os, RequestDecoderStatus v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, EHttpStatusCode v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, EHttpMethod v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, EApiGroup v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, EAlpacaApi v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, EManagementMethod v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, EDeviceType v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, EDeviceMethod v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, EParameter v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, ESensorName v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, EHttpHeader v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, EContentType v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, EHtmlPageSection v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

std::ostream& operator<<(std::ostream& os, EDeviceEepromTagId v) {
  char buffer[256];
  mcucore::PrintToBuffer print(buffer);
  PrintValueTo(v, print);
  return os << std::string_view(buffer, print.data_size());
}

#endif  // MCU_HOST_TARGET
}  // namespace alpaca

// END_SOURCE_GENERATED_BY_MAKE_ENUM_TO_STRING
