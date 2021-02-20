#include "tiny-alpaca-server/counting_bitbucket.h"

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
  strm.write("abc", 3);
  EXPECT_EQ(strm.count(), 3);
}

}  // namespace
}  // namespace alpaca
