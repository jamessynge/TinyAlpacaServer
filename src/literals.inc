// Non-lower case literal strings.
//
// This file is included in literals.h, where the DEFINE_LITERAL macro may be given
// various different definitions, depending on need.

// HTTP Method Names (all upper case):
DEFINE_LITERAL(GET, "GET")
DEFINE_LITERAL(PUT, "PUT")
DEFINE_LITERAL(HEAD, "HEAD")

// ASCOM Device Types (all lower case):
DEFINE_LITERAL(ObservingConditions, "observingconditions")
DEFINE_LITERAL(SafetyMonitor, "safetymonitor")

// ASCOM common methods.
DEFINE_LITERAL(Connected, "Connected")  // Mixed case for output.
DEFINE_LITERAL(Description, "description")
DEFINE_LITERAL(DriverInfo, "driverinfo")
DEFINE_LITERAL(DriverVersion, "driverversion")
DEFINE_LITERAL(InterfaceVersion, "interfaceversion")
DEFINE_LITERAL(Name, "name")
DEFINE_LITERAL(SupportedActions, "supportedactions")

// ObservingConditions methods:
DEFINE_LITERAL(AveragePeriod, "averageperiod")
DEFINE_LITERAL(CloudCover, "cloudcover")
DEFINE_LITERAL(DewPoint, "dewpoint")
DEFINE_LITERAL(Humidity, "humidity")
DEFINE_LITERAL(Pressure, "pressure")
DEFINE_LITERAL(Refresh, "refresh")
DEFINE_LITERAL(RainRate, "rainrate")
DEFINE_LITERAL(Temperature, "temperature")
DEFINE_LITERAL(SensorDescription, "sensordescription")

// SafetyMonitor methods:
DEFINE_LITERAL(IsSafe, "issafe")

// Query parameter and JSON property names. Mixed case (necessary for JSON
// output, though it means we need to perform case independent comparison when
// decoding).
DEFINE_LITERAL(ClientId, "ClientId")
DEFINE_LITERAL(ClientTransactionId, "ClientTransactionId")
DEFINE_LITERAL(ServerTransactionId, "ServerTransactionId")

// HTTP header names: Mixed case.
DEFINE_LITERAL(HttpAccept, "Accept")
DEFINE_LITERAL(HttpContentLength, "Content-Length")
DEFINE_LITERAL(HttpContentType, "Content-Type")
DEFINE_LITERAL(HttpContentEncoding, "Content-Encoding")
