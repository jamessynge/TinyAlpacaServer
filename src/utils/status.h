#ifndef TINY_ALPACA_SERVER_SRC_UTILS_STATUS_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_STATUS_H_

// This is a simplistic version of absl::Status, where the status, a uint32_t,
// is either 0 (OK) or non-zero. The choice of uint32_t is to comply with the
// Alpaca choice of 32-bit error codes such as:
//
//     ActionNotImplementedException:
//         Reserved error code (0x8004040C) to indicate that the
//         requested action is not implemented in this driver.

#include "utils/platform.h"

namespace alpaca {

class Status {
 public:
  Status() : code_(0) {}
  explicit Status(uint32_t code) : code_(code) {}
  bool ok() const { return code_ == 0; }
  uint32_t code() const { return code_; }

 private:
  uint32_t code_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_STATUS_H_
