#ifndef TINY_ALPACA_SERVER_ENCODER_COUNTING_BITBUCKET_H_
#define TINY_ALPACA_SERVER_ENCODER_COUNTING_BITBUCKET_H_

#include <stddef.h>

#include "tiny-alpaca-server/common/host_printable.h"

namespace alpaca {

// Counts the number of characters (bytes) written, but does no output.
// This is useful for computing the value to be placed in the Content-Length
// header.
class CountingBitbucket : public Print {
 public:
  CountingBitbucket() : Print(), count_(0) {}

  // Sums the values of count passed to write.
  size_t write(const char* const buffer, const size_t count) override {
    count_ += count;
    return count;
  }

  // The sum of counts passed to write.
  int count() const { return count_; }

 private:
  int count_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_ENCODER_COUNTING_BITBUCKET_H_
