#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_CASE_INSENSITIVE_LESS_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_CASE_INSENSITIVE_LESS_H_

#include <string>

namespace alpaca {
namespace test {

struct CaseInsensitiveLess {
  bool operator()(const std::string& lhs, const std::string& rhs) const;
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_CASE_INSENSITIVE_LESS_H_
