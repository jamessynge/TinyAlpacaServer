#ifndef TINY_ALPACA_SERVER_SRC_ALPACA_REQUEST_H_
#define TINY_ALPACA_SERVER_SRC_ALPACA_REQUEST_H_

// AlpacaRequest holds the information about an HTTP request being (or already
// completely) decoded by RequestDecoder.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "config.h"
#include "constants.h"

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

  // This resets the fields that won't always be explicitly set when a request
  // is successfully decoded (i.e. http_method will be set always, but client_id
  // might not be).
  void Reset();

  void set_client_id(uint32_t id) {
    client_id = id;
    have_client_id = true;
  }

  void set_client_transaction_id(uint32_t id) {
    client_transaction_id = id;
    have_client_transaction_id = true;
  }

  void set_server_transaction_id(uint32_t id) {
    server_transaction_id = id;
    have_server_transaction_id = true;
  }

  void set_connected(bool value) {
    connected = value;
    have_connected = true;
  }

  void set_brightness(int32_t value) {
    brightness = value;
    have_brightness = true;
  }

  void set_id(int32_t value) {
    id = value;
    have_id = true;
  }

  void set_state(bool value) {
    state = value;
    have_state = true;
  }

  void set_value(double v) {
    value = v;
    have_value = true;
  }

  void set_average_period(double v) {
    average_period = v;
    have_average_period = true;
  }

  bool set_string_value(const mcucore::StringView& value) {
    if (have_string_value || !string_value.Set(value.data(), value.size())) {
      return false;
    }
    have_string_value = true;
    return true;
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
  // TODO(jamessynge): Rework so that we don't waste space for parameters that
  // aren't provided in the same request. Probably want to dispatch to device
  // specific code each time we decode a parameter so that code can choose where
  // in some device and method specific union field the parameter should be
  // stored. ALTERNATIVELY, use ExtraParameters for storage, and call the device
  // specific code to decide whether to store the value.
  uint32_t client_id;
  uint32_t client_transaction_id;
  ESensorName sensor_name;
  bool connected;
  int32_t brightness;
  int32_t id;  // Switch id.
  bool state;
  double value;
  double average_period;
  mcucore::TinyString<32> string_value;

  // NOT from the client; this is set by the server/decoder at the *start* of
  // handling a request. We set this at the start so that even before we know
  // whether the request is valid, we can start logging (if we do logging).
  // The implications of setting this at the start is that we may execute
  // requests out of order w.r.t server_transaction_id.
  uint32_t server_transaction_id;

#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
  ExtraParameterValueMap extra_parameters;
#endif  // TAS_ENABLE_EXTRA_REQUEST_PARAMETERS

  // Set to zero by Reset, set to 1 when the corresponding field is set.
  unsigned int have_client_id : 1;
  unsigned int have_client_transaction_id : 1;
  unsigned int have_server_transaction_id : 1;
  unsigned int have_connected : 1;
  unsigned int have_brightness : 1;
  unsigned int have_id : 1;
  unsigned int have_state : 1;
  unsigned int have_value : 1;
  unsigned int have_average_period : 1;
  unsigned int have_string_value : 1;

  // So far we only support a single content type for PUT requests, so don't
  // store it.
  unsigned int saw_content_type : 1;

  unsigned int do_close : 1;  // Set to true if client requests it.
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ALPACA_REQUEST_H_
