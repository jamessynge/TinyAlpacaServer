#include "extras/test_tools/case_insensitive_less.h"

#include <string.h>

#include <string>

namespace alpaca {
namespace test {

bool CaseInsensitiveLess::operator()(const std::string& lhs,
                                     const std::string& rhs) const {
  return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

}  // namespace test
}  // namespace alpaca
