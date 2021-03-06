#ifndef TINY_ALPACA_SERVER_SRC_MATCH_LITERALS_H_
#define TINY_ALPACA_SERVER_SRC_MATCH_LITERALS_H_

// Functions to match (lookup) the enum value corresponding to a given string.

#include "constants.h"
#include "utils/string_view.h"

namespace alpaca {

bool MatchHttpMethod(const StringView& view, EHttpMethod& match);
bool MatchApiGroup(const StringView& view, EApiGroup& match);

bool MatchManagementMethod(const StringView& view, EManagementMethod& match);

bool MatchDeviceType(const StringView& view, EDeviceType& match);

bool MatchDeviceMethod(const EApiGroup group, const EDeviceType device_type,
                       const StringView& view, EDeviceMethod& match);

bool MatchParameter(const StringView& view, EParameter& match);

bool MatchHttpHeader(const StringView& view, EHttpHeader& match);

namespace internal {
// This is exposed only for testing.
bool MatchCommonDeviceMethod(const StringView& view, EDeviceMethod& match);
}  // namespace internal
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_MATCH_LITERALS_H_
