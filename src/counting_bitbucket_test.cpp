#include "counting_bitbucket.h"

// Tests of CountingBitbucket.
//
// Author: james.synge@gmail.com

#include "gmock/gmock.h"

namespace alpaca {
namespace {

TEST(CountingBitbucketTest, Unused) {
  CountingBitbucket strm;
  EXPECT_EQ(strm.count(), 0);
}

TEST(CountingBitbucketTest, EmptyStringView) {
  CountingBitbucket strm;
  strm.write("", 0);
  EXPECT_EQ(strm.count(), 0);
}

TEST(CountingBitbucketTest, Simple) {
  CountingBitbucket strm;
  strm.write("abc\r\n", 5);
  EXPECT_EQ(strm.count(), 5);
}

}  // namespace
}  // namespace alpaca
