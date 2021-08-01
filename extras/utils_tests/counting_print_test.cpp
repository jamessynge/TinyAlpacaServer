#include "utils/counting_print.h"

// Tests of CountingPrint.
//
// Author: james.synge@gmail.com

#include "gtest/gtest.h"

namespace alpaca {
namespace test {
namespace {

TEST(CountingPrintTest, Unused) {
  PrintNoOp no_op;
  CountingPrint counter(no_op);
  EXPECT_EQ(counter.count(), 0);
}

TEST(CountingPrintTest, EmptyStringView) {
  PrintNoOp no_op;
  CountingPrint counter(no_op);
  counter.write("", 0);
  EXPECT_EQ(counter.count(), 0);
}

TEST(CountingPrintTest, WriteString) {
  PrintNoOp no_op;
  CountingPrint counter(no_op);
  counter.write("abc\r\n", 5);
  EXPECT_EQ(counter.count(), 5);
}

TEST(CountingPrintTest, WriteChar) {
  PrintNoOp no_op;
  CountingPrint counter(no_op);
  counter.write('a');
  EXPECT_EQ(counter.count(), 1);
}

TEST(CountingPrintTest, WriteCString) {
  PrintNoOp no_op;
  CountingPrint counter(no_op);
  counter.write("abc\r\n");
  EXPECT_EQ(counter.count(), 5);
}

TEST(CountingPrintTest, Mixed) {
  PrintNoOp no_op;
  CountingPrint counter(no_op);
  counter.print('a');
  counter.print(123);
  EXPECT_EQ(counter.count(), 4);
}

}  // namespace
}  // namespace test
}  // namespace alpaca
