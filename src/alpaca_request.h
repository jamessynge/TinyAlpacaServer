#ifndef TINY_ALPACA_SERVER_ALPACA_REQUEST_H_
#define TINY_ALPACA_SERVER_ALPACA_REQUEST_H_

// Author: james.synge@gmail.com

#include "config.h"
#include "decoder_constants.h"
#include "platform.h"
#include "string_view.h"

#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
#include "extra_parameters.h"
#endif  // TAS_ENABLE_EXTRA_REQUEST_PARAMETERS

namespace alpaca {

// When compiled for debug, this is what we reset fields to. These enable a
// test to check for fields not being overwritten by the decoder unless valid
// values are found.
constexpr uint32_t kResetDeviceNumber = 123456789;
constexpr uint32_t kResetClientId = 987654321;
constexpr uint32_t kResetClientTransactionId = 198765432;

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

  // Set to zero by Reset, set to 1 when the corresponding field is set.
  unsigned int found_client_id : 1;
  unsigned int found_client_transaction_id : 1;

#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
  ExtraParameterValueMap extra_parameters;
#endif  // TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_ALPACA_REQUEST_H_
