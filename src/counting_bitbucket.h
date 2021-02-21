#ifndef TINY_ALPACA_SERVER_COUNTING_BITBUCKET_H_
#define TINY_ALPACA_SERVER_COUNTING_BITBUCKET_H_

// CountingBitbucket sums the number of characters (bytes) printed, but does no
// output. This is useful for computing the value to be placed in the
// Content-Length header.
//
// Author: james.synge@gmail.com

#include "platform.h"

namespace alpaca {

class CountingBitbucket : public Print {
 public:
  CountingBitbucket() : Print(), count_(0) {}

  // Sums the values of count passed to write. These are the overrides of the
  // two abstract virtual methods in Arduino's Print class.
  size_t write(uint8_t) override {
    count_ += 1;
    return 1;
  }
  size_t write(const uint8_t* buffer, size_t size) override {
    count_ += size;
    return size;
  }

  // Pull in the other variants of write; otherwise, only the above two are
  // visible.
  using Print::write;

  // The sum of counts passed to write.
  int count() const { return count_; }

 private:
  int count_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_COUNTING_BITBUCKET_H_
