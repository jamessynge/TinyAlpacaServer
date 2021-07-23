#include "utils/log_sink.h"

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gtest.h"
#include "utils/inline_literal.h"
#include "utils/o_print_stream.h"

namespace alpaca {
namespace test {
namespace {

TEST(LogSinkTest, CreateAndDelete) {
  PrintToStdString out;
  { LogSink sink(out); }
  // Prints out a newline to end the message. So far there isn't a message
  // prefix (i.e. no time or location).
  EXPECT_EQ(out.str(), "\n");
}

TEST(LogSinkTest, InsertIntoNonTemporary) {
  PrintToStdString out;
  {
    LogSink sink(out);
    sink << "abc";
  }
  EXPECT_EQ(out.str(), "abc\n");
}

TEST(LogSinkTest, InsertIntoTemporary) {
  PrintToStdString out;
  LogSink(out) << "abc";
  EXPECT_EQ(out.str(), "abc\n");
}

TEST(CheckSinkDeathTest, CreateAndDelete) {
  PrintToStdString out;
  EXPECT_DEATH(
      {
        CheckSink sink(out, TAS_FLASHSTR("foo.cc"), 123,
                       TAS_FLASHSTR("prefix1"));
      },
      "TAS_CHECK FAILED: foo.cc:123] prefix1");
}

TEST(CheckSinkDeathTest, NoFileName) {
  PrintToStdString out;
  EXPECT_DEATH({ CheckSink sink(out, nullptr, 123, TAS_FLASHSTR("prefix2")); },
               "TAS_CHECK FAILED: prefix2");
}

TEST(CheckSinkDeathTest, NoLineNumber) {
  PrintToStdString out;
  EXPECT_DEATH(
      {
        CheckSink sink(out, TAS_FLASHSTR("foo.cc"), 0, TAS_FLASHSTR("prefix3"));
      },
      "TAS_CHECK FAILED: foo.cc] prefix3");
}

TEST(CheckSinkDeathTest, InsertIntoNonTemporary) {
  PrintToStdString out;
  EXPECT_DEATH(
      {
        CheckSink sink(out, TAS_FLASHSTR("foo/bar.cc"), 234,
                       TAS_FLASHSTR("Prefix4"));
        sink << "abc";
      },
      "TAS_CHECK FAILED: bar.cc:234] Prefix4");
}

TEST(CheckSinkDeathTest, InsertIntoTemporary) {
  PrintToStdString out;
  EXPECT_DEATH(
      {
        CheckSink(out, TAS_FLASHSTR("foo/bar/baz.h"), 321,
                  TAS_FLASHSTR("message"))
            << "abc";
      },
      "TAS_CHECK FAILED: baz.h:321] message");
}

}  // namespace
}  // namespace test
}  // namespace alpaca
