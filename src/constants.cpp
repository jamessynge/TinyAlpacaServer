#include "constants.h"

// GENERATED FILE. See make_enum_to_string.py

#include "utils/inline_literal.h"
#include "utils/print_misc.h"

namespace alpaca {

size_t PrintValueTo(RequestDecoderStatus v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("RequestDecoderStatus"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpStatusCode v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("EHttpStatusCode"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpMethod v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("EHttpMethod"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EApiGroup v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("EApiGroup"), static_cast<uint32_t>(v),
                                 out);
}

size_t PrintValueTo(EAlpacaApi v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("EAlpacaApi"), static_cast<uint32_t>(v),
                                 out);
}

size_t PrintValueTo(EManagementMethod v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("EManagementMethod"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EDeviceType v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("EDeviceType"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EDeviceMethod v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("EDeviceMethod"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EParameter v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("EParameter"), static_cast<uint32_t>(v),
                                 out);
}

size_t PrintValueTo(ESensorName v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("ESensorName"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpHeader v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("EHttpHeader"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EContentType v, Print& out) {
  auto printable = ToPrintableProgmemString(v);
  if (printable.size() > 0) {
    return printable.printTo(out);
  }
  return PrintUnknownEnumValueTo(TASLIT("EContentType"),
                                 static_cast<uint32_t>(v), out);
}

PrintableProgmemString ToPrintableProgmemString(RequestDecoderStatus v) {
  switch (v) {
    case RequestDecoderStatus::kReset:
      return TASLIT("Reset");
    case RequestDecoderStatus::kDecoding:
      return TASLIT("Decoding");
    case RequestDecoderStatus::kDecoded:
      return TASLIT("Decoded");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(EHttpStatusCode v) {
  switch (v) {
    case EHttpStatusCode::kContinueDecoding:
      return TASLIT("ContinueDecoding");
    case EHttpStatusCode::kNeedMoreInput:
      return TASLIT("NeedMoreInput");
    case EHttpStatusCode::kHttpOk:
      return TASLIT("HttpOk");
    case EHttpStatusCode::kHttpBadRequest:
      return TASLIT("HttpBadRequest");
    case EHttpStatusCode::kHttpMethodNotAllowed:
      return TASLIT("HttpMethodNotAllowed");
    case EHttpStatusCode::kHttpNotAcceptable:
      return TASLIT("HttpNotAcceptable");
    case EHttpStatusCode::kHttpLengthRequired:
      return TASLIT("HttpLengthRequired");
    case EHttpStatusCode::kHttpPayloadTooLarge:
      return TASLIT("HttpPayloadTooLarge");
    case EHttpStatusCode::kHttpUnsupportedMediaType:
      return TASLIT("HttpUnsupportedMediaType");
    case EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge:
      return TASLIT("HttpRequestHeaderFieldsTooLarge");
    case EHttpStatusCode::kHttpInternalServerError:
      return TASLIT("HttpInternalServerError");
    case EHttpStatusCode::kHttpMethodNotImplemented:
      return TASLIT("HttpMethodNotImplemented");
    case EHttpStatusCode::kHttpVersionNotSupported:
      return TASLIT("HttpVersionNotSupported");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(EHttpMethod v) {
  switch (v) {
    case EHttpMethod::kUnknown:
      return TASLIT("Unknown");
    case EHttpMethod::GET:
      return TASLIT("GET");
    case EHttpMethod::PUT:
      return TASLIT("PUT");
    case EHttpMethod::HEAD:
      return TASLIT("HEAD");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(EApiGroup v) {
  switch (v) {
    case EApiGroup::kUnknown:
      return TASLIT("Unknown");
    case EApiGroup::kDevice:
      return TASLIT("Device");
    case EApiGroup::kManagement:
      return TASLIT("Management");
    case EApiGroup::kSetup:
      return TASLIT("Setup");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(EAlpacaApi v) {
  switch (v) {
    case EAlpacaApi::kUnknown:
      return TASLIT("Unknown");
    case EAlpacaApi::kDeviceApi:
      return TASLIT("DeviceApi");
    case EAlpacaApi::kDeviceSetup:
      return TASLIT("DeviceSetup");
    case EAlpacaApi::kManagementApiVersions:
      return TASLIT("ManagementApiVersions");
    case EAlpacaApi::kManagementDescription:
      return TASLIT("ManagementDescription");
    case EAlpacaApi::kManagementConfiguredDevices:
      return TASLIT("ManagementConfiguredDevices");
    case EAlpacaApi::kServerSetup:
      return TASLIT("ServerSetup");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(EManagementMethod v) {
  switch (v) {
    case EManagementMethod::kUnknown:
      return TASLIT("Unknown");
    case EManagementMethod::kDescription:
      return TASLIT("Description");
    case EManagementMethod::kConfiguredDevices:
      return TASLIT("ConfiguredDevices");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(EDeviceType v) {
  switch (v) {
    case EDeviceType::kUnknown:
      return TASLIT("Unknown");
    case EDeviceType::kCamera:
      return TASLIT("Camera");
    case EDeviceType::kCoverCalibrator:
      return TASLIT("CoverCalibrator");
    case EDeviceType::kDome:
      return TASLIT("Dome");
    case EDeviceType::kFilterWheel:
      return TASLIT("FilterWheel");
    case EDeviceType::kFocuser:
      return TASLIT("Focuser");
    case EDeviceType::kObservingConditions:
      return TASLIT("ObservingConditions");
    case EDeviceType::kRotator:
      return TASLIT("Rotator");
    case EDeviceType::kSafetyMonitor:
      return TASLIT("SafetyMonitor");
    case EDeviceType::kSwitch:
      return TASLIT("Switch");
    case EDeviceType::kTelescope:
      return TASLIT("Telescope");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(EDeviceMethod v) {
  switch (v) {
    case EDeviceMethod::kUnknown:
      return TASLIT("Unknown");
    case EDeviceMethod::kSetup:
      return TASLIT("Setup");
    case EDeviceMethod::kAction:
      return TASLIT("Action");
    case EDeviceMethod::kCommandBlind:
      return TASLIT("CommandBlind");
    case EDeviceMethod::kCommandBool:
      return TASLIT("CommandBool");
    case EDeviceMethod::kCommandString:
      return TASLIT("CommandString");
    case EDeviceMethod::kConnected:
      return TASLIT("Connected");
    case EDeviceMethod::kDescription:
      return TASLIT("Description");
    case EDeviceMethod::kDriverInfo:
      return TASLIT("DriverInfo");
    case EDeviceMethod::kDriverVersion:
      return TASLIT("DriverVersion");
    case EDeviceMethod::kInterfaceVersion:
      return TASLIT("InterfaceVersion");
    case EDeviceMethod::kName:
      return TASLIT("Name");
    case EDeviceMethod::kSupportedActions:
      return TASLIT("SupportedActions");
    case EDeviceMethod::kBrightness:
      return TASLIT("Brightness");
    case EDeviceMethod::kCalibratorState:
      return TASLIT("CalibratorState");
    case EDeviceMethod::kCoverState:
      return TASLIT("CoverState");
    case EDeviceMethod::kMaxBrightness:
      return TASLIT("MaxBrightness");
    case EDeviceMethod::kCalibratorOff:
      return TASLIT("CalibratorOff");
    case EDeviceMethod::kCalibratorOn:
      return TASLIT("CalibratorOn");
    case EDeviceMethod::kCloseCover:
      return TASLIT("CloseCover");
    case EDeviceMethod::kHaltCover:
      return TASLIT("HaltCover");
    case EDeviceMethod::kOpenCover:
      return TASLIT("OpenCover");
    case EDeviceMethod::kAveragePeriod:
      return TASLIT("AveragePeriod");
    case EDeviceMethod::kCloudCover:
      return TASLIT("CloudCover");
    case EDeviceMethod::kDewPoint:
      return TASLIT("DewPoint");
    case EDeviceMethod::kHumidity:
      return TASLIT("Humidity");
    case EDeviceMethod::kPressure:
      return TASLIT("Pressure");
    case EDeviceMethod::kRainRate:
      return TASLIT("RainRate");
    case EDeviceMethod::kRefresh:
      return TASLIT("Refresh");
    case EDeviceMethod::kSensorDescription:
      return TASLIT("SensorDescription");
    case EDeviceMethod::kSkyBrightness:
      return TASLIT("SkyBrightness");
    case EDeviceMethod::kSkyQuality:
      return TASLIT("SkyQuality");
    case EDeviceMethod::kSkyTemperature:
      return TASLIT("SkyTemperature");
    case EDeviceMethod::kStarFullWidthHalfMax:
      return TASLIT("StarFullWidthHalfMax");
    case EDeviceMethod::kTemperature:
      return TASLIT("Temperature");
    case EDeviceMethod::kTimeSinceLastUpdate:
      return TASLIT("TimeSinceLastUpdate");
    case EDeviceMethod::kWindDirection:
      return TASLIT("WindDirection");
    case EDeviceMethod::kWindGust:
      return TASLIT("WindGust");
    case EDeviceMethod::kWindSpeed:
      return TASLIT("WindSpeed");
    case EDeviceMethod::kIsSafe:
      return TASLIT("IsSafe");
    case EDeviceMethod::kMaxSwitch:
      return TASLIT("MaxSwitch");
    case EDeviceMethod::kCanWrite:
      return TASLIT("CanWrite");
    case EDeviceMethod::kGetSwitch:
      return TASLIT("GetSwitch");
    case EDeviceMethod::kGetSwitchDescription:
      return TASLIT("GetSwitchDescription");
    case EDeviceMethod::kGetSwitchName:
      return TASLIT("GetSwitchName");
    case EDeviceMethod::kGetSwitchValue:
      return TASLIT("GetSwitchValue");
    case EDeviceMethod::kMinSwitchValue:
      return TASLIT("MinSwitchValue");
    case EDeviceMethod::kMaxSwitchValue:
      return TASLIT("MaxSwitchValue");
    case EDeviceMethod::kSetSwitch:
      return TASLIT("SetSwitch");
    case EDeviceMethod::kSetSwitchName:
      return TASLIT("SetSwitchName");
    case EDeviceMethod::kSetSwitchValue:
      return TASLIT("SetSwitchValue");
    case EDeviceMethod::kSwitchStep:
      return TASLIT("SwitchStep");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(EParameter v) {
  switch (v) {
    case EParameter::kUnknown:
      return TASLIT("Unknown");
    case EParameter::kAction:
      return TASLIT("Action");
    case EParameter::kClientID:
      return TASLIT("ClientID");
    case EParameter::kClientTransactionID:
      return TASLIT("ClientTransactionID");
    case EParameter::kCommand:
      return TASLIT("Command");
    case EParameter::kConnected:
      return TASLIT("Connected");
    case EParameter::kParameters:
      return TASLIT("Parameters");
    case EParameter::kRaw:
      return TASLIT("Raw");
    case EParameter::kBrightness:
      return TASLIT("Brightness");
    case EParameter::kSensorName:
      return TASLIT("SensorName");
    case EParameter::kId:
      return TASLIT("Id");
    case EParameter::kName:
      return TASLIT("Name");
    case EParameter::kState:
      return TASLIT("State");
    case EParameter::kValue:
      return TASLIT("Value");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(ESensorName v) {
  switch (v) {
    case ESensorName::kUnknown:
      return TASLIT("Unknown");
    case ESensorName::kCloudCover:
      return TASLIT("CloudCover");
    case ESensorName::kDewPoint:
      return TASLIT("DewPoint");
    case ESensorName::kHumidity:
      return TASLIT("Humidity");
    case ESensorName::kPressure:
      return TASLIT("Pressure");
    case ESensorName::kRainRate:
      return TASLIT("RainRate");
    case ESensorName::kSkyBrightness:
      return TASLIT("SkyBrightness");
    case ESensorName::kSkyQuality:
      return TASLIT("SkyQuality");
    case ESensorName::kSkyTemperature:
      return TASLIT("SkyTemperature");
    case ESensorName::kStarFullWidthHalfMax:
      return TASLIT("StarFullWidthHalfMax");
    case ESensorName::kTemperature:
      return TASLIT("Temperature");
    case ESensorName::kWindDirection:
      return TASLIT("WindDirection");
    case ESensorName::kWindGust:
      return TASLIT("WindGust");
    case ESensorName::kWindSpeed:
      return TASLIT("WindSpeed");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(EHttpHeader v) {
  switch (v) {
    case EHttpHeader::kUnknown:
      return TASLIT("Unknown");
    case EHttpHeader::kHttpAccept:
      return TASLIT("HttpAccept");
    case EHttpHeader::kHttpContentLength:
      return TASLIT("HttpContentLength");
    case EHttpHeader::kHttpContentType:
      return TASLIT("HttpContentType");
    case EHttpHeader::kHttpContentEncoding:
      return TASLIT("HttpContentEncoding");
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(EContentType v) {
  switch (v) {
    case EContentType::kApplicationJson:
      return TASLIT("application/json");
    case EContentType::kTextPlain:
      return TASLIT("text/plain");
    case EContentType::kTextHtml:
      return TASLIT("text/html");
  }
  return PrintableProgmemString();
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
    case EDeviceMethod::kStarFullWidthHalfMax:
      return os << "StarFullWidthHalfMax";
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
    case ESensorName::kStarFullWidthHalfMax:
      return os << "StarFullWidthHalfMax";
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
    case EHttpHeader::kHttpAccept:
      return os << "HttpAccept";
    case EHttpHeader::kHttpContentLength:
      return os << "HttpContentLength";
    case EHttpHeader::kHttpContentType:
      return os << "HttpContentType";
    case EHttpHeader::kHttpContentEncoding:
      return os << "HttpContentEncoding";
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
