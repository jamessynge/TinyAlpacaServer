#include "src/counting_bitbucket.h"

// Tests of CountingBitbucket.
//
// Author: james.synge@gmail.com

#include "googletest/gmock.h"

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

TEST(CountingBitbucketTest, WriteString) {
  CountingBitbucket strm;
  strm.write("abc\r\n", 5);
  EXPECT_EQ(strm.count(), 5);
}

TEST(CountingBitbucketTest, WriteChar) {
  CountingBitbucket strm;
  strm.write('a');
  EXPECT_EQ(strm.count(), 1);
}

TEST(CountingBitbucketTest, WriteCString) {
  CountingBitbucket strm;
  strm.write("abc\r\n");
  EXPECT_EQ(strm.count(), 5);
}

TEST(CountingBitbucketTest, Mixed) {
  CountingBitbucket strm;
  strm.print('a');
  strm.print(123);
  EXPECT_EQ(strm.count(), 4);
}

}  // namespace
}  // namespace alpaca
