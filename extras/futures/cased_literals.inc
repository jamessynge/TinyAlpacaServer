// Non-lower case literal strings.
//
// This file is included in literals.h, where the DEFINE_LITERAL macro may be given
// various different definitions, depending on need.


// HTTP Method Names (all upper case):
DEFINE_LITERAL(kGET, "GET")
DEFINE_LITERAL(kPUT, "PUT")
DEFINE_LITERAL(kHEAD, "HEAD")

// Query parameter and JSON property names. Mixed case (necessary for JSON
// output, though it means we need to perform case independent comparison when
// decoding).
DEFINE_LITERAL(kClientId, "ClientId")
DEFINE_LITERAL(kClientTransactionId, "ClientTransactionId")
DEFINE_LITERAL(kServerTransactionId, "ServerTransactionId")

// HTTP header names: Mixed case.
DEFINE_LITERAL(kHttpAccept, "Accept")
DEFINE_LITERAL(kHttpContentLength, "Content-Length")
DEFINE_LITERAL(kHttpContentType, "Content-Type")
DEFINE_LITERAL(kHttpContentEncoding, "Content-Encoding")
