#ifndef TINY_ALPACA_SERVER_SRC_MATCH_LITERALS_H_
#define TINY_ALPACA_SERVER_SRC_MATCH_LITERALS_H_

// Functions to match (lookup) the enum value corresponding to a given string.
//
// Author: james.synge@gmail.com

#include "constants.h"
#include "string_view.h"

namespace alpaca {

bool MatchHttpMethod(const mcucore::StringView& view, EHttpMethod& match);
bool MatchApiGroup(const mcucore::StringView& view, EApiGroup& match);

bool MatchManagementMethod(const mcucore::StringView& view,
                           EManagementMethod& match);

bool MatchDeviceType(const mcucore::StringView& view, EDeviceType& match);

bool MatchDeviceMethod(const EApiGroup group, const EDeviceType device_type,
                       const mcucore::StringView& view, EDeviceMethod& match);

bool MatchParameter(const mcucore::StringView& view, EParameter& match);

bool MatchHttpHeader(const mcucore::StringView& view, EHttpHeader& match);

bool MatchSensorName(const mcucore::StringView& view, ESensorName& match);

namespace internal {
// This is exposed only for testing.
bool MatchCommonDeviceMethod(const mcucore::StringView& view,
                             EDeviceMethod& match);
}  // namespace internal
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_MATCH_LITERALS_H_
