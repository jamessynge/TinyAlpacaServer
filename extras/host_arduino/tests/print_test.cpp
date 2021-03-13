#include "extras/host_arduino/print.h"

#include <sstream>

#include "extras/test_tools/print_to_std_string.h"
#include "extras/test_tools/sample_printable.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace {
using ::alpaca::PrintToStdString;
using ::alpaca::SamplePrintable;

TEST(PrintTest, SamplePrintable) {
  constexpr char kText[] = "asdlkjasd;flkjasf";
  SamplePrintable sp;
  sp.str = kText;
  PrintToStdString out;
  EXPECT_EQ(sp.printTo(out), sp.str.size());
  EXPECT_EQ(out.str(), kText);
}

TEST(PrintTest, StreamPrintable) {
  constexpr char kText[] = "sdlkadkjfdakjklaj";
  SamplePrintable sp;
  sp.str = kText;
  LOG(INFO) << sp;
  {
    std::ostringstream oss;
    oss << sp;
    EXPECT_EQ(oss.str(), kText);
  }
}

}  // namespace
