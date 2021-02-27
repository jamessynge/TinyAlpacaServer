// This file is included in literals.h and literals.cc, where the DEFINE_LITERAL
// macro has several different definitions, depending on the action being
// performed (e.g. declaring an accessor method).
//
// DEFINE_LITERAL1 is used for Literals where the id (name) of the literal is
// the same as the string literal itself.

#ifdef DEFINE_LITERAL1
#undef DEFINE_LITERAL1
#endif  // DEFINE_LITERAL1

#define DEFINE_LITERAL1(symbol) DEFINE_LITERAL(symbol, #symbol)

DEFINE_LITERAL1(action)
DEFINE_LITERAL1(api)
DEFINE_LITERAL1(averageperiod)
DEFINE_LITERAL1(brightness)
DEFINE_LITERAL1(calibratoroff)
DEFINE_LITERAL1(calibratoron)
DEFINE_LITERAL1(calibratorstate)
DEFINE_LITERAL1(ClientId)
DEFINE_LITERAL1(ClientTransactionId)
DEFINE_LITERAL1(closecover)
DEFINE_LITERAL1(cloudcover)
DEFINE_LITERAL1(commandblind)
DEFINE_LITERAL1(commandbool)
DEFINE_LITERAL1(commandstring)
DEFINE_LITERAL1(configureddevices)
DEFINE_LITERAL1(Connected)  // Mixed case for output.
DEFINE_LITERAL1(coverstate)
DEFINE_LITERAL1(description)
DEFINE_LITERAL1(dewpoint)
DEFINE_LITERAL1(driverinfo)
DEFINE_LITERAL1(driverversion)
DEFINE_LITERAL1(ErrorMessage)
DEFINE_LITERAL1(ErrorNumber)
DEFINE_LITERAL1(GET)
DEFINE_LITERAL1(haltcover)
DEFINE_LITERAL1(HEAD)
DEFINE_LITERAL1(humidity)
DEFINE_LITERAL1(interfaceversion)
DEFINE_LITERAL1(issafe)
DEFINE_LITERAL1(management)
DEFINE_LITERAL1(maxbrightness)
DEFINE_LITERAL1(Maximum)  // Used in AxisRatesResponse
DEFINE_LITERAL1(Minimum)  // Used in AxisRatesResponse
DEFINE_LITERAL1(name)
DEFINE_LITERAL1(observingconditions)
DEFINE_LITERAL1(opencover)
DEFINE_LITERAL1(pressure)
DEFINE_LITERAL1(PUT)
DEFINE_LITERAL1(rainrate)
DEFINE_LITERAL1(refresh)
DEFINE_LITERAL1(safetymonitor)
DEFINE_LITERAL1(sensordescription)
DEFINE_LITERAL1(ServerTransactionId)
DEFINE_LITERAL1(setup)
DEFINE_LITERAL1(skybrightness)
DEFINE_LITERAL1(skyquality)
DEFINE_LITERAL1(skytemperature)
DEFINE_LITERAL1(starfwhm)
DEFINE_LITERAL1(supportedactions)
DEFINE_LITERAL1(temperature)
DEFINE_LITERAL1(timesincelastupdate)
DEFINE_LITERAL1(Value)
DEFINE_LITERAL1(winddirection)
DEFINE_LITERAL1(windgust)
DEFINE_LITERAL1(windspeed)

// Other literals whose name can not be the same as the value, or whose name I
// desire to be different for some reason.


DEFINE_LITERAL(HttpAccept, "Accept")
DEFINE_LITERAL(HttpContentEncoding, "Content-Encoding")
DEFINE_LITERAL(HttpContentLength, "Content-Length")
DEFINE_LITERAL(HttpContentType, "Content-Type")
DEFINE_LITERAL(JsonTrue, "true") // Used in json_encoder.
DEFINE_LITERAL(JsonFalse, "false") // Used in json_encoder.
DEFINE_LITERAL(JsonNan, "NaN") // Used in json_encoder.
DEFINE_LITERAL(JsonNegInf, "-Inf") // Used in json_encoder.
DEFINE_LITERAL(JsonInf, "Inf") // Could add constexpr Literal::substr.
DEFINE_LITERAL(MimeTypeJson, "application/json")
DEFINE_LITERAL(MimeTypeWwwFormUrlEncoded, "application/x-www-form-urlencoded")

// Common fixedHTTP Response Headers. For now, assuming that we're NOT
// keeping the connection open from request to request. That could change.
DEFINE_LITERAL(kCommonResponseHeaders,
  "\r\n"  // We assume the response start line comes first, then ...
  "Server: TinyAlpacaServer\r\n"
  "Connection: close\r\n")


#undef DEFINE_LITERAL1