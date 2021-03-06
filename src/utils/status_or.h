#ifndef TINY_ALPACA_SERVER_SRC_UTILS_STATUS_OR_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_STATUS_OR_H_

// This is a simplistic version of absl::StatusOr, supporting methods that
// need to return a value, or an error status.

#include "logging.h"
#include "platform.h"
#include "utils/status.h"

namespace alpaca {

template <typename T>
class StatusOr {
 public:
  /*implicit*/ StatusOr(const T& t)  // NOLINT
      : t_(t), ok_(true) {}
  /*implicit*/ StatusOr(const Status& status)  // NOLINT
      : status_(status), ok_(status.ok()) {
    if (status.ok()) {
      t_ = {};
    }
  }

  bool ok() const { return ok_; }

  const T& value() const {
    TAS_CHECK(ok_, "Hey, there isn't a value!");
    return t_;
  }

  Status status() const {
    if (ok_) {
      return Status();
    } else {
      return status_;
    }
  }

 private:
  // Note: it could be useful to have a simple version of std::variant with
  // which to implement StatusOr.
  union {
    Status status_;
    T t_;
  };
  bool ok_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_STATUS_OR_H_
