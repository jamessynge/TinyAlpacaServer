#include "constants.h"

// GENERATED FILE. See make_enum_to_string.py

#include "experimental/users/jamessynge/arduino/mcucore/src/inline_literal.h"
#include "utils/print_misc.h"

namespace alpaca {

size_t PrintValueTo(RequestDecoderStatus v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("RequestDecoderStatus"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpStatusCode v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("EHttpStatusCode"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpMethod v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("EHttpMethod"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EApiGroup v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("EApiGroup"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EAlpacaApi v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("EAlpacaApi"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EManagementMethod v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("EManagementMethod"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EDeviceType v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("EDeviceType"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EDeviceMethod v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("EDeviceMethod"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EParameter v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("EParameter"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(ESensorName v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("ESensorName"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpHeader v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("EHttpHeader"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EContentType v, Print& out) {
  auto flash_string = ToFlashStringHelper(v);
  if (flash_string != nullptr) {
    return out.print(flash_string);
  }
  return PrintUnknownEnumValueTo(TAS_FLASHSTR("EContentType"),
                                 static_cast<uint32_t>(v), out);
}

const __FlashStringHelper* ToFlashStringHelper(RequestDecoderStatus v) {
  switch (v) {
    case RequestDecoderStatus::kReset:
      return TAS_FLASHSTR("Reset");
    case RequestDecoderStatus::kDecoding:
      return TAS_FLASHSTR("Decoding");
    case RequestDecoderStatus::kDecoded:
      return TAS_FLASHSTR("Decoded");
  }
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EHttpStatusCode v) {
  switch (v) {
    case EHttpStatusCode::kContinueDecoding:
      return TAS_FLASHSTR("ContinueDecoding");
    case EHttpStatusCode::kNeedMoreInput:
      return TAS_FLASHSTR("NeedMoreInput");
    case EHttpStatusCode::kHttpOk:
      return TAS_FLASHSTR("HttpOk");
    case EHttpStatusCode::kHttpBadRequest:
      return TAS_FLASHSTR("HttpBadRequest");
    case EHttpStatusCode::kHttpMethodNotAllowed:
      return TAS_FLASHSTR("HttpMethodNotAllowed");
    case EHttpStatusCode::kHttpNotAcceptable:
      return TAS_FLASHSTR("HttpNotAcceptable");
    case EHttpStatusCode::kHttpLengthRequired:
      return TAS_FLASHSTR("HttpLengthRequired");
    case EHttpStatusCode::kHttpPayloadTooLarge:
      return TAS_FLASHSTR("HttpPayloadTooLarge");
    case EHttpStatusCode::kHttpUnsupportedMediaType:
      return TAS_FLASHSTR("HttpUnsupportedMediaType");
    case EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge:
      return TAS_FLASHSTR("HttpRequestHeaderFieldsTooLarge");
    case EHttpStatusCode::kHttpInternalServerError:
      return TAS_FLASHSTR("HttpInternalServerError");
    case EHttpStatusCode::kHttpMethodNotImplemented:
      return TAS_FLASHSTR("HttpMethodNotImplemented");
    case EHttpStatusCode::kHttpVersionNotSupported:
      return TAS_FLASHSTR("HttpVersionNotSupported");
  }
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EHttpMethod v) {
  switch (v) {
    case EHttpMethod::kUnknown:
      return TAS_FLASHSTR("Unknown");
    case EHttpMethod::GET:
      return TAS_FLASHSTR("GET");
    case EHttpMethod::PUT:
      return TAS_FLASHSTR("PUT");
    case EHttpMethod::HEAD:
      return TAS_FLASHSTR("HEAD");
  }
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EApiGroup v) {
  switch (v) {
    case EApiGroup::kUnknown:
      return TAS_FLASHSTR("Unknown");
    case EApiGroup::kDevice:
      return TAS_FLASHSTR("Device");
    case EApiGroup::kManagement:
      return TAS_FLASHSTR("Management");
    case EApiGroup::kSetup:
      return TAS_FLASHSTR("Setup");
  }
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EAlpacaApi v) {
  switch (v) {
    case EAlpacaApi::kUnknown:
      return TAS_FLASHSTR("Unknown");
    case EAlpacaApi::kDeviceApi:
      return TAS_FLASHSTR("DeviceApi");
    case EAlpacaApi::kDeviceSetup:
      return TAS_FLASHSTR("DeviceSetup");
    case EAlpacaApi::kManagementApiVersions:
      return TAS_FLASHSTR("ManagementApiVersions");
    case EAlpacaApi::kManagementDescription:
      return TAS_FLASHSTR("ManagementDescription");
    case EAlpacaApi::kManagementConfiguredDevices:
      return TAS_FLASHSTR("ManagementConfiguredDevices");
    case EAlpacaApi::kServerSetup:
      return TAS_FLASHSTR("ServerSetup");
  }
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EManagementMethod v) {
  switch (v) {
    case EManagementMethod::kUnknown:
      return TAS_FLASHSTR("Unknown");
    case EManagementMethod::kDescription:
      return TAS_FLASHSTR("Description");
    case EManagementMethod::kConfiguredDevices:
      return TAS_FLASHSTR("ConfiguredDevices");
  }
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EDeviceType v) {
  switch (v) {
    case EDeviceType::kUnknown:
      return TAS_FLASHSTR("Unknown");
    case EDeviceType::kCamera:
      return TAS_FLASHSTR("Camera");
    case EDeviceType::kCoverCalibrator:
      return TAS_FLASHSTR("CoverCalibrator");
    case EDeviceType::kDome:
      return TAS_FLASHSTR("Dome");
    case EDeviceType::kFilterWheel:
      return TAS_FLASHSTR("FilterWheel");
    case EDeviceType::kFocuser:
      return TAS_FLASHSTR("Focuser");
    case EDeviceType::kObservingConditions:
      return TAS_FLASHSTR("ObservingConditions");
    case EDeviceType::kRotator:
      return TAS_FLASHSTR("Rotator");
    case EDeviceType::kSafetyMonitor:
      return TAS_FLASHSTR("SafetyMonitor");
    case EDeviceType::kSwitch:
      return TAS_FLASHSTR("Switch");
    case EDeviceType::kTelescope:
      return TAS_FLASHSTR("Telescope");
  }
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EDeviceMethod v) {
  switch (v) {
    case EDeviceMethod::kUnknown:
      return TAS_FLASHSTR("Unknown");
    case EDeviceMethod::kSetup:
      return TAS_FLASHSTR("Setup");
    case EDeviceMethod::kAction:
      return TAS_FLASHSTR("Action");
    case EDeviceMethod::kCommandBlind:
      return TAS_FLASHSTR("CommandBlind");
    case EDeviceMethod::kCommandBool:
      return TAS_FLASHSTR("CommandBool");
    case EDeviceMethod::kCommandString:
      return TAS_FLASHSTR("CommandString");
    case EDeviceMethod::kConnected:
      return TAS_FLASHSTR("Connected");
    case EDeviceMethod::kDescription:
      return TAS_FLASHSTR("Description");
    case EDeviceMethod::kDriverInfo:
      return TAS_FLASHSTR("DriverInfo");
    case EDeviceMethod::kDriverVersion:
      return TAS_FLASHSTR("DriverVersion");
    case EDeviceMethod::kInterfaceVersion:
      return TAS_FLASHSTR("InterfaceVersion");
    case EDeviceMethod::kName:
      return TAS_FLASHSTR("Name");
    case EDeviceMethod::kSupportedActions:
      return TAS_FLASHSTR("SupportedActions");
    case EDeviceMethod::kBrightness:
      return TAS_FLASHSTR("Brightness");
    case EDeviceMethod::kCalibratorState:
      return TAS_FLASHSTR("CalibratorState");
    case EDeviceMethod::kCoverState:
      return TAS_FLASHSTR("CoverState");
    case EDeviceMethod::kMaxBrightness:
      return TAS_FLASHSTR("MaxBrightness");
    case EDeviceMethod::kCalibratorOff:
      return TAS_FLASHSTR("CalibratorOff");
    case EDeviceMethod::kCalibratorOn:
      return TAS_FLASHSTR("CalibratorOn");
    case EDeviceMethod::kCloseCover:
      return TAS_FLASHSTR("CloseCover");
    case EDeviceMethod::kHaltCover:
      return TAS_FLASHSTR("HaltCover");
    case EDeviceMethod::kOpenCover:
      return TAS_FLASHSTR("OpenCover");
    case EDeviceMethod::kAveragePeriod:
      return TAS_FLASHSTR("AveragePeriod");
    case EDeviceMethod::kCloudCover:
      return TAS_FLASHSTR("CloudCover");
    case EDeviceMethod::kDewPoint:
      return TAS_FLASHSTR("DewPoint");
    case EDeviceMethod::kHumidity:
      return TAS_FLASHSTR("Humidity");
    case EDeviceMethod::kPressure:
      return TAS_FLASHSTR("Pressure");
    case EDeviceMethod::kRainRate:
      return TAS_FLASHSTR("RainRate");
    case EDeviceMethod::kRefresh:
      return TAS_FLASHSTR("Refresh");
    case EDeviceMethod::kSensorDescription:
      return TAS_FLASHSTR("SensorDescription");
    case EDeviceMethod::kSkyBrightness:
      return TAS_FLASHSTR("SkyBrightness");
    case EDeviceMethod::kSkyQuality:
      return TAS_FLASHSTR("SkyQuality");
    case EDeviceMethod::kSkyTemperature:
      return TAS_FLASHSTR("SkyTemperature");
    case EDeviceMethod::kStarFWHM:
      return TAS_FLASHSTR("StarFWHM");
    case EDeviceMethod::kTemperature:
      return TAS_FLASHSTR("Temperature");
    case EDeviceMethod::kTimeSinceLastUpdate:
      return TAS_FLASHSTR("TimeSinceLastUpdate");
    case EDeviceMethod::kWindDirection:
      return TAS_FLASHSTR("WindDirection");
    case EDeviceMethod::kWindGust:
      return TAS_FLASHSTR("WindGust");
    case EDeviceMethod::kWindSpeed:
      return TAS_FLASHSTR("WindSpeed");
    case EDeviceMethod::kIsSafe:
      return TAS_FLASHSTR("IsSafe");
    case EDeviceMethod::kMaxSwitch:
      return TAS_FLASHSTR("MaxSwitch");
    case EDeviceMethod::kCanWrite:
      return TAS_FLASHSTR("CanWrite");
    case EDeviceMethod::kGetSwitch:
      return TAS_FLASHSTR("GetSwitch");
    case EDeviceMethod::kGetSwitchDescription:
      return TAS_FLASHSTR("GetSwitchDescription");
    case EDeviceMethod::kGetSwitchName:
      return TAS_FLASHSTR("GetSwitchName");
    case EDeviceMethod::kGetSwitchValue:
      return TAS_FLASHSTR("GetSwitchValue");
    case EDeviceMethod::kMinSwitchValue:
      return TAS_FLASHSTR("MinSwitchValue");
    case EDeviceMethod::kMaxSwitchValue:
      return TAS_FLASHSTR("MaxSwitchValue");
    case EDeviceMethod::kSetSwitch:
      return TAS_FLASHSTR("SetSwitch");
    case EDeviceMethod::kSetSwitchName:
      return TAS_FLASHSTR("SetSwitchName");
    case EDeviceMethod::kSetSwitchValue:
      return TAS_FLASHSTR("SetSwitchValue");
    case EDeviceMethod::kSwitchStep:
      return TAS_FLASHSTR("SwitchStep");
  }
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EParameter v) {
  switch (v) {
    case EParameter::kUnknown:
      return TAS_FLASHSTR("Unknown");
    case EParameter::kAction:
      return TAS_FLASHSTR("Action");
    case EParameter::kClientID:
      return TAS_FLASHSTR("ClientID");
    case EParameter::kClientTransactionID:
      return TAS_FLASHSTR("ClientTransactionID");
    case EParameter::kCommand:
      return TAS_FLASHSTR("Command");
    case EParameter::kConnected:
      return TAS_FLASHSTR("Connected");
    case EParameter::kParameters:
      return TAS_FLASHSTR("Parameters");
    case EParameter::kRaw:
      return TAS_FLASHSTR("Raw");
    case EParameter::kBrightness:
      return TAS_FLASHSTR("Brightness");
    case EParameter::kAveragePeriod:
      return TAS_FLASHSTR("AveragePeriod");
    case EParameter::kSensorName:
      return TAS_FLASHSTR("SensorName");
    case EParameter::kId:
      return TAS_FLASHSTR("Id");
    case EParameter::kName:
      return TAS_FLASHSTR("Name");
    case EParameter::kState:
      return TAS_FLASHSTR("State");
    case EParameter::kValue:
      return TAS_FLASHSTR("Value");
  }
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(ESensorName v) {
  switch (v) {
    case ESensorName::kUnknown:
      return TAS_FLASHSTR("Unknown");
    case ESensorName::kCloudCover:
      return TAS_FLASHSTR("CloudCover");
    case ESensorName::kDewPoint:
      return TAS_FLASHSTR("DewPoint");
    case ESensorName::kHumidity:
      return TAS_FLASHSTR("Humidity");
    case ESensorName::kPressure:
      return TAS_FLASHSTR("Pressure");
    case ESensorName::kRainRate:
      return TAS_FLASHSTR("RainRate");
    case ESensorName::kSkyBrightness:
      return TAS_FLASHSTR("SkyBrightness");
    case ESensorName::kSkyQuality:
      return TAS_FLASHSTR("SkyQuality");
    case ESensorName::kSkyTemperature:
      return TAS_FLASHSTR("SkyTemperature");
    case ESensorName::kStarFWHM:
      return TAS_FLASHSTR("StarFWHM");
    case ESensorName::kTemperature:
      return TAS_FLASHSTR("Temperature");
    case ESensorName::kWindDirection:
      return TAS_FLASHSTR("WindDirection");
    case ESensorName::kWindGust:
      return TAS_FLASHSTR("WindGust");
    case ESensorName::kWindSpeed:
      return TAS_FLASHSTR("WindSpeed");
  }
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EHttpHeader v) {
  switch (v) {
    case EHttpHeader::kUnknown:
      return TAS_FLASHSTR("Unknown");
    case EHttpHeader::kConnection:
      return TAS_FLASHSTR("Connection");
    case EHttpHeader::kContentLength:
      return TAS_FLASHSTR("Content-Length");
    case EHttpHeader::kContentType:
      return TAS_FLASHSTR("Content-Type");
    case EHttpHeader::kDate:
      return TAS_FLASHSTR("Date");
  }
  return nullptr;
}

const __FlashStringHelper* ToFlashStringHelper(EContentType v) {
  switch (v) {
    case EContentType::kApplicationJson:
      return TAS_FLASHSTR("application/json");
    case EContentType::kTextPlain:
      return TAS_FLASHSTR("text/plain");
    case EContentType::kTextHtml:
      return TAS_FLASHSTR("text/html");
  }
  return nullptr;
}

#if TAS_HOST_TARGET
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
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
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
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
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
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
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
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
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
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
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
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
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
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
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
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
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
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
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
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
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
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
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
  return os << "Unknown {name}, value=" << static_cast<int64_t>(v);
}
#endif  // TAS_HOST_TARGET

}  // namespace alpaca
