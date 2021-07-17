// This file is included in literals.h, where the TAS_DEFINE_BUILTIN_LITERAL
// macro has multiple definitions, depending on the action being performed (e.g.
// declaring an accessor method).
//
// TAS_DEFINE_BUILTIN_LITERAL1 is used for Literals where the id (name) of the
// literal is the same as the string literal itself.

#ifdef TAS_DEFINE_BUILTIN_LITERAL1
#undef TAS_DEFINE_BUILTIN_LITERAL1
#endif  // TAS_DEFINE_BUILTIN_LITERAL1

#define TAS_DEFINE_BUILTIN_LITERAL1(symbol) \
  TAS_DEFINE_BUILTIN_LITERAL(symbol, #symbol)

TAS_DEFINE_BUILTIN_LITERAL1(action)
TAS_DEFINE_BUILTIN_LITERAL1(api)
TAS_DEFINE_BUILTIN_LITERAL1(apiversions)
TAS_DEFINE_BUILTIN_LITERAL1(AveragePeriod)
TAS_DEFINE_BUILTIN_LITERAL1(averageperiod)
TAS_DEFINE_BUILTIN_LITERAL1(brightness)
TAS_DEFINE_BUILTIN_LITERAL1(calibratoroff)
TAS_DEFINE_BUILTIN_LITERAL1(calibratoron)
TAS_DEFINE_BUILTIN_LITERAL1(calibratorstate)
TAS_DEFINE_BUILTIN_LITERAL1(camera)
TAS_DEFINE_BUILTIN_LITERAL1(canwrite)
TAS_DEFINE_BUILTIN_LITERAL1(ClientID)
TAS_DEFINE_BUILTIN_LITERAL1(ClientTransactionID)
TAS_DEFINE_BUILTIN_LITERAL1(close)
TAS_DEFINE_BUILTIN_LITERAL1(closecover)
TAS_DEFINE_BUILTIN_LITERAL1(cloudcover)
TAS_DEFINE_BUILTIN_LITERAL1(Command)
TAS_DEFINE_BUILTIN_LITERAL1(commandblind)
TAS_DEFINE_BUILTIN_LITERAL1(commandbool)
TAS_DEFINE_BUILTIN_LITERAL1(commandstring)
TAS_DEFINE_BUILTIN_LITERAL1(configureddevices)
TAS_DEFINE_BUILTIN_LITERAL1(connected)  // Lower case for path matching.
TAS_DEFINE_BUILTIN_LITERAL1(Connected)  // Mixed case for output.
TAS_DEFINE_BUILTIN_LITERAL1(Connection)
TAS_DEFINE_BUILTIN_LITERAL1(covercalibrator)
TAS_DEFINE_BUILTIN_LITERAL1(coverstate)
TAS_DEFINE_BUILTIN_LITERAL1(description)
TAS_DEFINE_BUILTIN_LITERAL1(DeviceName)
TAS_DEFINE_BUILTIN_LITERAL1(DeviceNumber)
TAS_DEFINE_BUILTIN_LITERAL1(DeviceType)
TAS_DEFINE_BUILTIN_LITERAL1(dewpoint)
TAS_DEFINE_BUILTIN_LITERAL1(dome)
TAS_DEFINE_BUILTIN_LITERAL1(driverinfo)
TAS_DEFINE_BUILTIN_LITERAL1(driverversion)
TAS_DEFINE_BUILTIN_LITERAL1(ErrorMessage)
TAS_DEFINE_BUILTIN_LITERAL1(ErrorNumber)
TAS_DEFINE_BUILTIN_LITERAL1(False)
TAS_DEFINE_BUILTIN_LITERAL1(filterwheel)
TAS_DEFINE_BUILTIN_LITERAL1(focuser)
TAS_DEFINE_BUILTIN_LITERAL1(GET)
TAS_DEFINE_BUILTIN_LITERAL1(getswitch)
TAS_DEFINE_BUILTIN_LITERAL1(getswitchdescription)
TAS_DEFINE_BUILTIN_LITERAL1(getswitchname)
TAS_DEFINE_BUILTIN_LITERAL1(getswitchvalue)
TAS_DEFINE_BUILTIN_LITERAL1(haltcover)
TAS_DEFINE_BUILTIN_LITERAL1(HEAD)
TAS_DEFINE_BUILTIN_LITERAL1(humidity)
TAS_DEFINE_BUILTIN_LITERAL1(Id)
TAS_DEFINE_BUILTIN_LITERAL1(interfaceversion)
TAS_DEFINE_BUILTIN_LITERAL1(issafe)
TAS_DEFINE_BUILTIN_LITERAL1(Location)
TAS_DEFINE_BUILTIN_LITERAL1(management)
TAS_DEFINE_BUILTIN_LITERAL1(Manufacturer)
TAS_DEFINE_BUILTIN_LITERAL1(ManufacturerVersion)
TAS_DEFINE_BUILTIN_LITERAL1(maxbrightness)
TAS_DEFINE_BUILTIN_LITERAL1(Maximum)  // Used in AxisRatesResponse
TAS_DEFINE_BUILTIN_LITERAL1(maxswitch)
TAS_DEFINE_BUILTIN_LITERAL1(maxswitchvalue)
TAS_DEFINE_BUILTIN_LITERAL1(Minimum)  // Used in AxisRatesResponse
TAS_DEFINE_BUILTIN_LITERAL1(minswitchvalue)
TAS_DEFINE_BUILTIN_LITERAL1(name)
TAS_DEFINE_BUILTIN_LITERAL1(Name)  // Mixed case for output.
TAS_DEFINE_BUILTIN_LITERAL1(observingconditions)
TAS_DEFINE_BUILTIN_LITERAL1(OK)
TAS_DEFINE_BUILTIN_LITERAL1(opencover)
TAS_DEFINE_BUILTIN_LITERAL1(Parameters)
TAS_DEFINE_BUILTIN_LITERAL1(pressure)
TAS_DEFINE_BUILTIN_LITERAL1(PUT)
TAS_DEFINE_BUILTIN_LITERAL1(rainrate)
TAS_DEFINE_BUILTIN_LITERAL1(Raw)
TAS_DEFINE_BUILTIN_LITERAL1(refresh)
TAS_DEFINE_BUILTIN_LITERAL1(rotator)
TAS_DEFINE_BUILTIN_LITERAL1(safetymonitor)
TAS_DEFINE_BUILTIN_LITERAL1(sensordescription)
TAS_DEFINE_BUILTIN_LITERAL1(SensorName)
TAS_DEFINE_BUILTIN_LITERAL1(Server)
TAS_DEFINE_BUILTIN_LITERAL1(ServerName)
TAS_DEFINE_BUILTIN_LITERAL1(ServerTransactionID)
TAS_DEFINE_BUILTIN_LITERAL1(setswitch)
TAS_DEFINE_BUILTIN_LITERAL1(setswitchname)
TAS_DEFINE_BUILTIN_LITERAL1(setswitchvalue)
TAS_DEFINE_BUILTIN_LITERAL1(setup)
TAS_DEFINE_BUILTIN_LITERAL1(skybrightness)
TAS_DEFINE_BUILTIN_LITERAL1(skyquality)
TAS_DEFINE_BUILTIN_LITERAL1(skytemperature)
TAS_DEFINE_BUILTIN_LITERAL1(starfullwidthhalfmax)
TAS_DEFINE_BUILTIN_LITERAL1(starfwhm)
TAS_DEFINE_BUILTIN_LITERAL1(supportedactions)
TAS_DEFINE_BUILTIN_LITERAL1(switchstep)
TAS_DEFINE_BUILTIN_LITERAL1(telescope)
TAS_DEFINE_BUILTIN_LITERAL1(temperature)
TAS_DEFINE_BUILTIN_LITERAL1(timesincelastupdate)
TAS_DEFINE_BUILTIN_LITERAL1(TinyAlpacaServer)
TAS_DEFINE_BUILTIN_LITERAL1(True)
TAS_DEFINE_BUILTIN_LITERAL1(UniqueID)
TAS_DEFINE_BUILTIN_LITERAL1(v1)
TAS_DEFINE_BUILTIN_LITERAL1(Value)
TAS_DEFINE_BUILTIN_LITERAL1(winddirection)
TAS_DEFINE_BUILTIN_LITERAL1(windgust)
TAS_DEFINE_BUILTIN_LITERAL1(windspeed)
TAS_DEFINE_BUILTIN_LITERAL1(State)

// Other literals whose names can not be the same as the value (e.g. switch and
// true), or whose name I desire to be different for some reason.

TAS_DEFINE_BUILTIN_LITERAL(DeviceTypeSwitch, "switch")

// Literals used during decoding of the request, and maybe for output.
TAS_DEFINE_BUILTIN_LITERAL(HttpVersionEndOfLine, "HTTP/1.1\r\n")
TAS_DEFINE_BUILTIN_LITERAL(HttpEndOfLine, "\r\n")
TAS_DEFINE_BUILTIN_LITERAL(HttpAccept, "Accept")
TAS_DEFINE_BUILTIN_LITERAL(HttpContentEncoding, "Content-Encoding")
TAS_DEFINE_BUILTIN_LITERAL(HttpContentLength, "Content-Length")
TAS_DEFINE_BUILTIN_LITERAL(HttpContentType, "Content-Type")
TAS_DEFINE_BUILTIN_LITERAL(HttpKeepAlive, "Keep-Alive")

TAS_DEFINE_BUILTIN_LITERAL(MimeTypeWwwFormUrlEncoded,
                           "application/x-www-form-urlencoded")

// Output Content-Types.
TAS_DEFINE_BUILTIN_LITERAL(MimeTypeJson, "application/json")
TAS_DEFINE_BUILTIN_LITERAL(MimeTypeTextPlain, "text/plain")
TAS_DEFINE_BUILTIN_LITERAL(MimeTypeTextHtml, "text/html")

// Literals used during output.
TAS_DEFINE_BUILTIN_LITERAL(HttpVersion, "HTTP/1.1")
TAS_DEFINE_BUILTIN_LITERAL(HttpOkStatus, "HTTP/1.1 200 OK")
TAS_DEFINE_BUILTIN_LITERAL(HttpConnectionClose, "Connection: close\r\n")
TAS_DEFINE_BUILTIN_LITERAL(HttpServerHeader, "Server: TinyAlpacaServer\r\n")

// HTTP Response Reason Phrases:
TAS_DEFINE_BUILTIN_LITERAL(HttpBadRequest, "Bad Request")
TAS_DEFINE_BUILTIN_LITERAL(HttpNotFound, "Not Found")
TAS_DEFINE_BUILTIN_LITERAL(HttpMethodNotAllowed, "Method Not Allowed")
TAS_DEFINE_BUILTIN_LITERAL(HttpNotAcceptable, "Not Acceptable")
TAS_DEFINE_BUILTIN_LITERAL(HttpLengthRequired, "Length Required")
TAS_DEFINE_BUILTIN_LITERAL(HttpPayloadTooLarge, "Payload Too Large")
TAS_DEFINE_BUILTIN_LITERAL(HttpUnsupportedMediaType, "Unsupported Media Type")
TAS_DEFINE_BUILTIN_LITERAL(HttpRequestHeaderFieldsTooLarge,
                           "Request Header Fields Too Large")
TAS_DEFINE_BUILTIN_LITERAL(HttpMethodNotImplemented, "Method Not Implemented")
TAS_DEFINE_BUILTIN_LITERAL(HttpVersionNotSupported,
                           "HTTP Version Not Supported")
TAS_DEFINE_BUILTIN_LITERAL(HttpInternalServerError, "Internal Server Error")

// Error bodies:
TAS_DEFINE_BUILTIN_LITERAL(ApiUnknown,
                           "request.api value is unknown/unexpected (bug).")
TAS_DEFINE_BUILTIN_LITERAL(NoSuchDevice, "Device type and number not found.")

// ASCOM Error Messages (to be refined).
TAS_DEFINE_BUILTIN_LITERAL(ErrorActionNotImplemented,
                           "The requested action is not implemented")
TAS_DEFINE_BUILTIN_LITERAL(
    InvalidOperation,
    "The requested operation can not be undertaken at this time")
TAS_DEFINE_BUILTIN_LITERAL(ErrorInvalidValue, "Invalid value")
TAS_DEFINE_BUILTIN_LITERAL(
    ErrorInvalidWhileParked,
    "The operation is invalid because the mount is parked.")
TAS_DEFINE_BUILTIN_LITERAL(ErrorInvalidWhileSlaved,
                           "The operation is invalid because the mount is "
                           "currently in a Slaved state.")
TAS_DEFINE_BUILTIN_LITERAL(ErrorNotConnected,
                           "The communications channel is not connected")
TAS_DEFINE_BUILTIN_LITERAL(ErrorNotImplemented, "The method is not implemented")
TAS_DEFINE_BUILTIN_LITERAL(ErrorNotInCacheException, "NotInCacheException")
TAS_DEFINE_BUILTIN_LITERAL(ErrorSettingsProviderError, "SettingsProviderError")
TAS_DEFINE_BUILTIN_LITERAL(ErrorValueNotSet, "The value has not been set.")
TAS_DEFINE_BUILTIN_LITERAL(ErrorUnspecifiedError, "UnspecifiedError")

#undef TAS_DEFINE_BUILTIN_LITERAL1
