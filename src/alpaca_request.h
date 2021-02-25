#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_REQUEST_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_REQUEST_H_

// Author: james.synge@gmail.com

#include "src/config.h"
#include "src/decoder_constants.h"
#include "src/platform.h"
#include "src/string_view.h"

#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
#include "src/extra_parameters.h"
#endif  // TAS_ENABLE_EXTRA_REQUEST_PARAMETERS

namespace alpaca {

// When compiled for debug, this is what we reset fields to. These enable a
// test to check for fields not being overwritten by the decoder unless valid
// values are found.
constexpr uint32_t kResetDeviceNumber = 123456789;
constexpr uint32_t kResetClientId = 987654321;
constexpr uint32_t kResetClientTransactionId = 198765432;
constexpr uint32_t kResetServerTransactionId = 543212345;

struct AlpacaRequest {
  // This resets the fields that won't always be explicity set when a request is
  // successfully decoded (i.e. http_method will be set always, but client_id
  // might not be).
  void Reset();

  // From the HTTP method and path:
  EHttpMethod http_method;
  EDeviceType device_type;
  uint32_t device_number;
  EMethod ascom_method;

  // Parameters, either from the path (GET) or the body (PUT).
  //
  // QUESTION: Is it required that the client provide client_id and
  // client_transaction_id? It doesn't appear so from the documentation of ASCOM
  // Alpaca.
  uint32_t client_id;
  uint32_t client_transaction_id;

  // NOT from the client; this is set by the server/decoder at the *start* of
  // handling a request. We set this at the start so that even before we know
  // whether the request is valid, we can start logging (if we do logging).
  // The implications of setting this at the start is that we may execute
  // requests out of order w.r.t server_transaction_id.
  uint32_t server_transaction_id;

  // Set to zero by Reset, set to 1 when the corresponding field is set.
  unsigned int found_client_id : 1;
  unsigned int found_client_transaction_id : 1;
  unsigned int have_server_transaction_id : 1;

#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
  ExtraParameterValueMap extra_parameters;
#endif  // TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_REQUEST_H_
