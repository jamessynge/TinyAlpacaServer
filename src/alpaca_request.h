#ifndef TINY_ALPACA_SERVER_REQUEST_H_
#define TINY_ALPACA_SERVER_REQUEST_H_

// Author: james.synge@gmail.com

#include <cstdint>
#include <cstring>

#include "decoder_constants.h"
#include "string_view.h"

namespace alpaca {

// When compiled for debug, this is what we reset fields to. These enable a
// test to check for fields not being overwritten by the decoder unless valid
// values are found.
constexpr uint32_t kResetDeviceNumber = 123456789;
constexpr uint32_t kResetClientId = 987654321;
constexpr uint32_t kResetClientTransactionId = 198765432;

constexpr uint8_t kMaxValueLength =
    32;  // TODO(jamessynge): determine appropriate value.

// A very, very small string class with an embedded char array and size.
template <uint8_t N>
class TinyString {
 public:
  void Clear() { size_ = 0; }

  bool Set(const StringView& view) {
    Clear();
    if (view.size() > N) {
      return false;
    }
    memcpy(data_, view.data(), view.size());
    return true;
  }

  StringView ToStringView() const { return StringView(data_, size_); }

 private:
  uint8_t size_{0};
  char data_[N];
};

struct ExtraParameterValue {
  EParameter parameter;
  TinyString<kMaxValueLength> value;
};

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

  // uint8_t num_extra_parameters;
  // ExtraParameterValue extra_parameters[2];
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_REQUEST_H_
