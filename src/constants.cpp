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
  }
  return PrintableProgmemString();
}

PrintableProgmemString ToPrintableProgmemString(EParameter v) {
  switch (v) {
    case EParameter::kUnknown:
      return TASLIT("Unknown");
    case EParameter::kAction:
      return TASLIT("Action");
    case EParameter::kBrightness:
      return TASLIT("Brightness");
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
    case EParameter::kSensorName:
      return TASLIT("SensorName");
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

}  // namespace alpaca
