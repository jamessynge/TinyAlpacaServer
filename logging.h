#ifndef TINY_ALPACA_SERVER_LOGGING_H_
#define TINY_ALPACA_SERVER_LOGGING_H_

// This exports the logging macros used in Alpaca Decoder. They're available
// publicly in the tensorflow file: core/platform/default/logging.h.
//
// I need to figure out how to make them available for Alpaca Decoder. For now,
// these are all NO-OPS.

#include <sstream>

namespace alpaca {

// LogMessageNull supports the DVLOG macro by simply dropping any log messages.
class LogMessageNull : public std::basic_ostringstream<char> {
 public:
  LogMessageNull() {}
  ~LogMessageNull() override {}
};
}  // namespace alpaca

#define LOG(severity) \
  while (false && ((severity) > 0)) ::alpaca::LogMessageNull()
#define DLOG(severity) LOG(severity)
#define DVLOG(level) LOG(level)

#define DCHECK(condition) \
  while (false && (condition)) LOG(FATAL)

#define _ALPACA_DCHECK_NO_OP(x, y) \
  while (false && ((void)(x), (void)(y), 0)) LOG(FATAL)

#define DCHECK_EQ(x, y) _ALPACA_DCHECK_NO_OP(x, y)
#define DCHECK_NE(x, y) _ALPACA_DCHECK_NO_OP(x, y)
#define DCHECK_LE(x, y) _ALPACA_DCHECK_NO_OP(x, y)
#define DCHECK_LT(x, y) _ALPACA_DCHECK_NO_OP(x, y)
#define DCHECK_GE(x, y) _ALPACA_DCHECK_NO_OP(x, y)
#define DCHECK_GT(x, y) _ALPACA_DCHECK_NO_OP(x, y)

#endif  // TINY_ALPACA_SERVER_LOGGING_H_
