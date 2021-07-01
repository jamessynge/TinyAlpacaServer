#ifndef TINY_ALPACA_SERVER_SRC_UTILS_STATUS_OR_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_STATUS_OR_H_

// This is a simplistic version of absl::StatusOr, supporting methods that
// need to return a value, or an error status.
//
// Author: james.synge@gmail.com

#include "utils/logging.h"
#include "utils/platform.h"
#include "utils/status.h"

namespace alpaca {

// T can not be a class with virtual functions, nor with a destructor, because
// it must be valid as the type of a member of an anonymous union.
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
    TAS_CHECK(ok_) << FLASHSTR("Hey, there isn't a value!");
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

#define TAS_ASSIGN_OR_RETURN(lhs, status_or_expression)                \
  TAS_ASSIGN_OR_RETURN_IMPL_(                                          \
      TAS_STATUS_MACROS_CONCAT_NAME(_status_or_value_, __LINE__), lhs, \
      status_or_expression)

#define TAS_ASSIGN_OR_RETURN_IMPL_(statusor, lhs, status_or_expression) \
  auto statusor = status_or_expression;                                 \
  if (!statusor.ok()) {                                                 \
    return statusor.status();                                           \
  }                                                                     \
  lhs = statusor.value();

// Internal helper for concatenating macro values.
#define TAS_STATUS_MACROS_CONCAT_NAME_INNER_(x, y) x##y
#define TAS_STATUS_MACROS_CONCAT_NAME(x, y) \
  TAS_STATUS_MACROS_CONCAT_NAME_INNER_(x, y)

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_STATUS_OR_H_
