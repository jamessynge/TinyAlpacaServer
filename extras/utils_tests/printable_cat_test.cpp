#include "utils/printable_cat.h"

#include <string>

#include "absl/strings/str_cat.h"
#include "extras/test_tools/print_to_std_string.h"
#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "utils/any_printable.h"
#include "utils/literal.h"
#include "utils/string_view.h"

namespace alpaca {
namespace {

TEST(PrintableCatTest, Strings) {
  char space = ' ';
  StringView abc("abc");
  Literal def("def");
  AnyPrintable ghi(StringView("ghi"));
  auto p = PrintableCat(abc, space, def, space, ghi);
  EXPECT_EQ(PrintValueToStdString(p), "abc def ghi");
}

TEST(PrintableCatTest, Numbers) {
  char space = ' ';
  auto p = PrintableCat(0, space, 3.14f, space, 3.14);
  EXPECT_EQ(PrintValueToStdString(p), absl::StrCat("0 ", std::to_string(3.14f),
                                                   " ", std::to_string(3.14)));
}

}  // namespace
}  // namespace alpaca
