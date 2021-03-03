// This file is included in literals.h, where the TAS_DEFINE_BUILTIN_LITERAL
// macro has multiple definitions, depending on the action being performed (e.g. declaring an accessor method).
//
// TAS_DEFINE_BUILTIN_LITERAL1 is used for Literals where the id (name) of the literal is
// the same as the string literal itself.

#ifdef TAS_DEFINE_BUILTIN_LITERAL1
#undef TAS_DEFINE_BUILTIN_LITERAL1
#endif  // TAS_DEFINE_BUILTIN_LITERAL1

#define TAS_DEFINE_BUILTIN_LITERAL1(symbol) \
  TAS_DEFINE_BUILTIN_LITERAL(symbol, #symbol)

TAS_DEFINE_BUILTIN_LITERAL1(action)
TAS_DEFINE_BUILTIN_LITERAL1(api)
TAS_DEFINE_BUILTIN_LITERAL1(averageperiod)
TAS_DEFINE_BUILTIN_LITERAL1(brightness)
TAS_DEFINE_BUILTIN_LITERAL1(calibratoroff)
TAS_DEFINE_BUILTIN_LITERAL1(calibratoron)
TAS_DEFINE_BUILTIN_LITERAL1(calibratorstate)
TAS_DEFINE_BUILTIN_LITERAL1(camera)
TAS_DEFINE_BUILTIN_LITERAL1(ClientId)
TAS_DEFINE_BUILTIN_LITERAL1(ClientTransactionId)
TAS_DEFINE_BUILTIN_LITERAL1(closecover)
TAS_DEFINE_BUILTIN_LITERAL1(cloudcover)
TAS_DEFINE_BUILTIN_LITERAL1(commandblind)
TAS_DEFINE_BUILTIN_LITERAL1(commandbool)
TAS_DEFINE_BUILTIN_LITERAL1(commandstring)
TAS_DEFINE_BUILTIN_LITERAL1(configureddevices)
TAS_DEFINE_BUILTIN_LITERAL1(connected)  // Lower case for path matching.
TAS_DEFINE_BUILTIN_LITERAL1(Connected)  // Mixed case for output.
TAS_DEFINE_BUILTIN_LITERAL1(covercalibrator)
TAS_DEFINE_BUILTIN_LITERAL1(coverstate)
TAS_DEFINE_BUILTIN_LITERAL1(description)
TAS_DEFINE_BUILTIN_LITERAL1(DeviceType)
TAS_DEFINE_BUILTIN_LITERAL1(DeviceNumber)
TAS_DEFINE_BUILTIN_LITERAL1(UniqueID)
TAS_DEFINE_BUILTIN_LITERAL1(dewpoint)
TAS_DEFINE_BUILTIN_LITERAL1(dome)
TAS_DEFINE_BUILTIN_LITERAL1(driverinfo)
TAS_DEFINE_BUILTIN_LITERAL1(driverversion)
TAS_DEFINE_BUILTIN_LITERAL1(ErrorMessage)
TAS_DEFINE_BUILTIN_LITERAL1(ErrorNumber)
TAS_DEFINE_BUILTIN_LITERAL1(filterwheel)
TAS_DEFINE_BUILTIN_LITERAL1(focuser)
TAS_DEFINE_BUILTIN_LITERAL1(GET)
TAS_DEFINE_BUILTIN_LITERAL1(haltcover)
TAS_DEFINE_BUILTIN_LITERAL1(HEAD)
TAS_DEFINE_BUILTIN_LITERAL1(humidity)
TAS_DEFINE_BUILTIN_LITERAL1(interfaceversion)
TAS_DEFINE_BUILTIN_LITERAL1(issafe)
TAS_DEFINE_BUILTIN_LITERAL1(Location)
TAS_DEFINE_BUILTIN_LITERAL1(management)
TAS_DEFINE_BUILTIN_LITERAL1(Manufacturer)
TAS_DEFINE_BUILTIN_LITERAL1(ManufacturerVersion)
TAS_DEFINE_BUILTIN_LITERAL1(maxbrightness)
TAS_DEFINE_BUILTIN_LITERAL1(Maximum)  // Used in AxisRatesResponse
TAS_DEFINE_BUILTIN_LITERAL1(Minimum)  // Used in AxisRatesResponse
TAS_DEFINE_BUILTIN_LITERAL1(name)
TAS_DEFINE_BUILTIN_LITERAL1(Name)
TAS_DEFINE_BUILTIN_LITERAL1(observingconditions)
TAS_DEFINE_BUILTIN_LITERAL1(opencover)
TAS_DEFINE_BUILTIN_LITERAL1(pressure)
TAS_DEFINE_BUILTIN_LITERAL1(PUT)
TAS_DEFINE_BUILTIN_LITERAL1(rainrate)
TAS_DEFINE_BUILTIN_LITERAL1(refresh)
TAS_DEFINE_BUILTIN_LITERAL1(rotator)
TAS_DEFINE_BUILTIN_LITERAL1(safetymonitor)
TAS_DEFINE_BUILTIN_LITERAL1(sensordescription)
TAS_DEFINE_BUILTIN_LITERAL1(ServerName)
TAS_DEFINE_BUILTIN_LITERAL1(ServerTransactionId)
TAS_DEFINE_BUILTIN_LITERAL1(setup)
TAS_DEFINE_BUILTIN_LITERAL1(skybrightness)
TAS_DEFINE_BUILTIN_LITERAL1(skyquality)
TAS_DEFINE_BUILTIN_LITERAL1(skytemperature)
TAS_DEFINE_BUILTIN_LITERAL1(starfwhm)
TAS_DEFINE_BUILTIN_LITERAL1(supportedactions)
TAS_DEFINE_BUILTIN_LITERAL1(telescope)
TAS_DEFINE_BUILTIN_LITERAL1(temperature)
TAS_DEFINE_BUILTIN_LITERAL1(timesincelastupdate)
TAS_DEFINE_BUILTIN_LITERAL1(Value)
TAS_DEFINE_BUILTIN_LITERAL1(winddirection)
TAS_DEFINE_BUILTIN_LITERAL1(windgust)
TAS_DEFINE_BUILTIN_LITERAL1(windspeed)

// Other literals whose name can not be the same as the value, or whose name I
// desire to be different for some reason.


TAS_DEFINE_BUILTIN_LITERAL(HttpVersionEndOfLine, "HTTP/1.1\r\n")
TAS_DEFINE_BUILTIN_LITERAL(HttpAccept, "Accept")
TAS_DEFINE_BUILTIN_LITERAL(HttpContentEncoding, "Content-Encoding")
TAS_DEFINE_BUILTIN_LITERAL(HttpContentLength, "Content-Length")
TAS_DEFINE_BUILTIN_LITERAL(HttpContentType, "Content-Type")
TAS_DEFINE_BUILTIN_LITERAL(JsonTrue, "true") // Used in json_encoder.
TAS_DEFINE_BUILTIN_LITERAL(JsonFalse, "false") // Used in json_encoder.
TAS_DEFINE_BUILTIN_LITERAL(JsonNan, "NaN") // Used in json_encoder.
TAS_DEFINE_BUILTIN_LITERAL(JsonNegInf, "-Inf") // Used in json_encoder.
TAS_DEFINE_BUILTIN_LITERAL(JsonInf, "Inf") // Could add constexpr Literal::substr.
TAS_DEFINE_BUILTIN_LITERAL(MimeTypeJson, "application/json")
TAS_DEFINE_BUILTIN_LITERAL(MimeTypeWwwFormUrlEncoded, "application/x-www-form-urlencoded")
TAS_DEFINE_BUILTIN_LITERAL(DeviceTypeSwitch, "switch")

// Common fixedHTTP Response Headers. For now, assuming that we're NOT
// keeping the connection open from request to request. That could change.
TAS_DEFINE_BUILTIN_LITERAL(kCommonResponseHeaders,
  "\r\n"  // We assume the response start line comes first, then ...
  "Server: TinyAlpacaServer\r\n"
  "Connection: close\r\n")


#undef TAS_DEFINE_BUILTIN_LITERAL1
