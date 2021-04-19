#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_REQUEST_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_REQUEST_H_

// AlpacaRequest holds the information about an HTTP request being (or already
// completely) decoded by RequestDecoder.
//
// Author: james.synge@gmail.com

#include "config.h"
#include "constants.h"
#include "utils/logging.h"
#include "utils/platform.h"
#include "utils/string_view.h"

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
constexpr uint32_t kResetServerTransactionId = 543212345;

struct AlpacaRequest {
  AlpacaRequest();

  // This resets the fields that won't always be explicity set when a request is
  // successfully decoded (i.e. http_method will be set always, but client_id
  // might not be).
  void Reset();

  void set_client_id(uint32_t id) {
    TAS_DCHECK(!have_client_id);
    client_id = id;
    have_client_id = true;
  }

  void set_client_transaction_id(uint32_t id) {
    TAS_DCHECK(!have_client_transaction_id);
    client_transaction_id = id;
    have_client_transaction_id = true;
  }

  void set_server_transaction_id(uint32_t id) {
    TAS_DCHECK(!have_server_transaction_id);
    server_transaction_id = id;
    have_server_transaction_id = true;
  }

  void set_connected(bool value) {
    TAS_DCHECK(!have_connected);
    connected = value;
    have_connected = true;
  }

  void set_brightness(uint32_t value) {
    TAS_DCHECK(!have_brightness);
    brightness = value;
    have_brightness = true;
  }

  // From the HTTP method and path:
  EHttpMethod http_method;
  EApiGroup api_group;
  EAlpacaApi api;

  // The device_* fields are only filled in when api is EAlpacaApi::kDeviceApi
  // or kDeviceSetup.
  EDeviceType device_type;
  uint32_t device_number;

  // The device_method is only field in when api is EAlpacaApi::kDeviceApi.
  EDeviceMethod device_method;

  // Parameters, either from the path (GET & HEAD) or the body (PUT), or both
  // (PUT).
  uint32_t client_id;
  uint32_t client_transaction_id;
  ESensorName sensor_name;
  bool connected;
  uint32_t brightness;

  // NOT from the client; this is set by the server/decoder at the *start* of
  // handling a request. We set this at the start so that even before we know
  // whether the request is valid, we can start logging (if we do logging).
  // The implications of setting this at the start is that we may execute
  // requests out of order w.r.t server_transaction_id.
  uint32_t server_transaction_id;

  // Set to zero by Reset, set to 1 when the corresponding field is set.
  unsigned int have_client_id : 1;
  unsigned int have_client_transaction_id : 1;
  unsigned int have_server_transaction_id : 1;
  unsigned int have_connected : 1;
  unsigned int have_brightness : 1;
  unsigned int do_close : 1;  // Set to true if client requests it.

#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
  ExtraParameterValueMap extra_parameters;
#endif  // TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_REQUEST_H_
