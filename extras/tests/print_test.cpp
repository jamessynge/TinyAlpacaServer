#include "extras/host_arduino/print.h"

#include <sstream>

#include "extras/tests/test_utils.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"

namespace alpaca {

struct SamplePrintable : public Printable {
  size_t printTo(Print& p) const override {
    return p.write(str.data(), str.size());
  }

  std::string str;
};

namespace {

TEST(PrintTest, SamplePrintable) {
  constexpr char kText[] = "asdlkjasd;flkjasf";
  SamplePrintable sp;
  sp.str = kText;
  PrintToString out;
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
}  // namespace alpaca
