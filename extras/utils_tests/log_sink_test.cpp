#include "utils/log_sink.h"

#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {
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
  EXPECT_DEATH({ CheckSink sink(out, TASLIT("prefix")); },
               "TAS_CHECK FAILED: prefix");
}

TEST(CheckSinkDeathTest, InsertIntoNonTemporary) {
  PrintToStdString out;
  EXPECT_DEATH(
      {
        CheckSink sink(out, TASLIT("Prefix"));
        sink << "abc";
      },
      "TAS_CHECK FAILED: Prefix");
}

TEST(CheckSinkDeathTest, InsertIntoTemporary) {
  PrintToStdString out;
  EXPECT_DEATH({ CheckSink(out, TASLIT("message")) << "abc"; },
               "TAS_CHECK FAILED: message");
}

}  // namespace
}  // namespace alpaca
