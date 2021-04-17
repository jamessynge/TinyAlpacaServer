#ifndef TINY_ALPACA_SERVER_SRC_UTILS_STATUS_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_STATUS_H_

// This is a simplistic version of absl::Status, where the status, a uint32_t,
// is either 0 (OK) or non-zero. The choice of uint32_t is to comply with the
// Alpaca choice of 32-bit error codes such as:
//
//     ActionNotImplementedException:
//         Reserved error code (0x8004040C) to indicate that the
//         requested action is not implemented in this driver.
//
// Author: james.synge@gmail.com

#include "utils/literal.h"
#include "utils/platform.h"

namespace alpaca {

// TODO(jamessynge): Consider adding an 'error code space' to Status so that we
// can support two or three spaces: ASCOM error codes, HTTP status codes, and
// possibly Unix errno values.
//
// Doing so could help avoid the need for the switch statement in
// WriteResponse::HttpErrorResponse, which has the effect of causing all of the
// error literals to be linked in to the binary if HttpErrorResponse is also
// linked in. Instead we could have a separate function for each HTTP status
// code, like those in ascom_error_codes.* for ASCOM error codes.
//
// Alternately, I could introduce HttpStatus, encapsulating an EHttpStatusCode
// and a message, which could in turn be used for producing a Status instance
// where appropriate.

class Status {
 public:
  Status() : code_(0) {}
  explicit Status(uint32_t code) : code_(code) {}
  Status(uint32_t code, Literal message)
      : code_(code), message_(code != 0 ? message : Literal()) {}
  bool ok() const { return code_ == 0; }
  uint32_t code() const { return code_; }
  const Literal message() const { return message_; }

 private:
  uint32_t code_;
  Literal message_;
};

inline Status OkStatus() { return Status(); }

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_STATUS_H_
