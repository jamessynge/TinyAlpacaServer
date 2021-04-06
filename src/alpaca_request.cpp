#include "alpaca_request.h"

// Author: james.synge@gmail.com

#include "constants.h"

namespace alpaca {

AlpacaRequest::AlpacaRequest() {
  // This call is mainly a benefit to tests. The server/decoder should call
  // Reset when it is starting to decode a new HTTP request.
  Reset();
}

void AlpacaRequest::Reset() {
  http_method = EHttpMethod::kUnknown;
  sensor_name = ESensorName::kUnknown;

  have_client_id = false;
  have_client_transaction_id = false;
  have_server_transaction_id = false;
  do_close = false;

  // Theoretically we don't need to clear the following fields because they
  // shouldn't be examined unless the decoder has returned kHttpOk. However, it
  // makes writing tests easier if we do so.
  api_group = EApiGroup::kUnknown;
  api = EAlpacaApi::kUnknown;
  device_type = EDeviceType::kUnknown;
  device_number = kResetDeviceNumber;
  device_method = EDeviceMethod::kUnknown;

  // The server transaction id is set at the start of the decoding process, with
  // the aim of correlating error logs and responses with the request; this
  // assumes that the client sends a transaction id of its own.
  server_transaction_id = kResetServerTransactionId;

  // Parameters that may be provided for all requests, so we handle them
  // specifically.
  client_id = kResetClientId;
  client_transaction_id = kResetClientTransactionId;

#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
  extra_parameters.clear();
#endif  // TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
}

}  // namespace alpaca
