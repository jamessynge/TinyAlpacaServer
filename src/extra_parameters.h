#ifndef TINY_ALPACA_SERVER_SRC_EXTRA_PARAMETERS_H_
#define TINY_ALPACA_SERVER_SRC_EXTRA_PARAMETERS_H_

// This file contains the optional support for recording the parameter enum and
// short string value of parameters for which support isn't built into the
// decoder (i.e. not ClientId and ClientTransactionId).
//
// To add a new such parameter, add an entry for it in the EParameter enum in
// decoder_constants.h, and a token for it in kRecognizedParameters in tokens.h.
//
// Author: james.synge@gmail.com

#include "src/config.h"
#include "src/decoder_constants.h"
#include "src/platform.h"
#include "src/string_view.h"

// The minimum is 2 to allow for testing of this feature.

static_assert(2 <= TAS_MAX_EXTRA_REQUEST_PARAMETERS &&
                  TAS_MAX_EXTRA_REQUEST_PARAMETERS < 256,
              "TAS_MAX_EXTRA_REQUEST_PARAMETERS must be in the range [2, 255]");

namespace alpaca {

constexpr uint8_t kMaxExtraParameters = TAS_MAX_EXTRA_REQUEST_PARAMETERS;

static_assert(0 <= TAS_MAX_EXTRA_REQUEST_PARAMETER_LENGTH &&
                  TAS_MAX_EXTRA_REQUEST_PARAMETER_LENGTH < 256,
              "TAS_MAX_EXTRA_REQUEST_PARAMETER_LENGTH must be "
              "positive and less than 256.");

constexpr uint8_t kMaxExtraParameterValueLength =
    TAS_MAX_EXTRA_REQUEST_PARAMETER_LENGTH;

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
  TinyString<kMaxExtraParameterValueLength> value;
};

// A minimal collection of extra parameters.
class ExtraParameterValueMap {
 public:
  using size_type = uint8_t;
  using const_iterator = const ExtraParameterValue*;
  using iterator = const_iterator;
  enum EInsertResult {
    kInserted,
    kDuplicateParameter,
    kValueTooLong,
    kTooManyParameters
  };

  ExtraParameterValueMap() { clear(); }

  void clear() { size_ = 0; }

  const_iterator begin() const { return entries_; }
  const_iterator end() const { return &entries_[size_]; }

  EInsertResult insert(EParameter parameter, const StringView& value) {
    if (value.size() > kMaxExtraParameterValueLength) {
      return kValueTooLong;
    } else if (contains(parameter)) {
      return kDuplicateParameter;
    } else if (size_ >= kMaxExtraParameters) {
      return kTooManyParameters;
    } else {
      entries_[size_].parameter = parameter;
      entries_[size_].value.Set(value);
      ++size_;
    }
    return kInserted;
  }

  bool contains(EParameter parameter) const {
    for (int ndx = 0; ndx < size_; ++ndx) {
      if (entries_[ndx].parameter == parameter) {
        return true;
      }
    }
    return false;
  }

  StringView find(EParameter parameter) const {
    for (int ndx = 0; ndx < size_; ++ndx) {
      if (entries_[ndx].parameter == parameter) {
        return entries_[ndx].value.ToStringView();
      }
    }
    return StringView();
  }

 private:
  uint8_t size_;
  ExtraParameterValue entries_[kMaxExtraParameters];
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_EXTRA_PARAMETERS_H_
