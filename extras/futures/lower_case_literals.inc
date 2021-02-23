// LOWER CASE literal strings.
//
// This file is included in literals.h, where the DEFINE_LITERAL macro may be given
// various different definitions, depending on need.

DEFINE_LITERAL(kEmpty, "")

// HTTP Method Names (all upper case):
DEFINE_LITERAL(kGET, "GET")
DEFINE_LITERAL(kPUT, "PUT")
DEFINE_LITERAL(kHEAD, "HEAD")

// ASCOM Device Types (all lower case):
DEFINE_LITERAL(kObservingConditions, "observingconditions")
DEFINE_LITERAL(kSafetyMonitor, "safetymonitor")

// ASCOM common methods (all lower case):
DEFINE_LITERAL(kConnected, "connected")
DEFINE_LITERAL(kDescription, "description")
DEFINE_LITERAL(kDriverInfo, "driverinfo")
DEFINE_LITERAL(kDriverVersion, "driverversion")
DEFINE_LITERAL(kInterfaceVersion, "interfaceversion")
DEFINE_LITERAL(kName, "name")
DEFINE_LITERAL(kSupportedActions, "supportedactions")

// ObservingConditions methods:
DEFINE_LITERAL(kAveragePeriod, "averageperiod")
DEFINE_LITERAL(kCloudCover, "cloudcover")
DEFINE_LITERAL(kDewPoint, "dewpoint")
DEFINE_LITERAL(kHumidity, "humidity")
DEFINE_LITERAL(kPressure, "pressure")
DEFINE_LITERAL(kRefresh, "refresh")
DEFINE_LITERAL(kRainRate, "rainrate")
DEFINE_LITERAL(kTemperature, "temperature")
DEFINE_LITERAL(kSensorDescription, "sensordescription")

// SafetyMonitor methods:
DEFINE_LITERAL(kIsSafe, "issafe")

// Query parameter and JSON property names. Mixed case (necessary for JSON
// output, though it means we need to perform case independent comparison when
// decoding).
DEFINE_LITERAL(kClientId, "clientid")
DEFINE_LITERAL(kClientTransactionId, "clienttransactionid")

// HTTP header names: Mixed case.
DEFINE_LITERAL(kHttpAccept, "Accept")
DEFINE_LITERAL(kHttpContentLength, "Content-Length")
DEFINE_LITERAL(kHttpContentType, "Content-Type")
DEFINE_LITERAL(kHttpContentEncoding, "Content-Encoding")
