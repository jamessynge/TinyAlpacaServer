#ifndef TINY_ALPACA_SERVER_SRC_MATCH_LITERALS_H_
#define TINY_ALPACA_SERVER_SRC_MATCH_LITERALS_H_

// Functions to match (lookup) the enum value corresponding to a given string.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "constants.h"

namespace alpaca {

// Match `view` against known HTTP methods (e.g. GET). If successful, set
// `match` to the corresponding enum and return true. Else return false.
bool MatchHttpMethod(const mcucore::StringView& view, EHttpMethod& match);

// Match `view` against ASCOM Alpaca top-level path names (i.e. "setup",
// "management" or "api"). If successful, set `match` to the corresponding enum
// and return true. Else return false.
bool MatchApiGroup(const mcucore::StringView& view, EApiGroup& match);

// Match `view` against supported ASCOM Alpaca management methods (e.g.
// "configureddevices"). If successful, set `match` to the corresponding enum
// and return true. Else return false.
bool MatchManagementMethod(const mcucore::StringView& view,
                           EManagementMethod& match);

// Match `view` against supported ASCOM Alpaca device types (e.g. "camera"). If
// successful, set `match` to the corresponding enum and return true. Else
// return false.
bool MatchDeviceType(const mcucore::StringView& view, EDeviceType& match);

// Match `view` against supported ASCOM Alpaca device methods given the group
// and device_type. If successful, set `match` to the corresponding enum and
// return true. Else return false.
bool MatchDeviceMethod(EApiGroup group, EDeviceType device_type,
                       const mcucore::StringView& view, EDeviceMethod& match);

// Match `view` against parameter names defined for ASCOM Alpaca requests (e.g.
// "ClientID"). If successful, set `match` to the corresponding enum and return
// true. Else return false.
bool MatchParameter(const mcucore::StringView& view, EParameter& match);

// Match `view` against HTTP header names necessary to process requests
// correctly (e.g. Content-Length). If successful, set `match` to the
// corresponding enum and return true. Else return false.
bool MatchHttpHeader(const mcucore::StringView& view, EHttpHeader& match);

// Match `view` against supported ASCOM Alpaca weather sensor names, though the
// names are really the names of the quantity measured (e.g. "humidity"). If
// successful, set `match` to the corresponding enum and return true. Else
// return false.
bool MatchSensorName(const mcucore::StringView& view, ESensorName& match);

namespace internal {
// Match `view` against ASCOM Alpaca Device API methods which apply to all
// devices (e.g. "connected". If successful, set `match` to the corresponding
// enum and return true. Else return false.
// This is exposed only for testing.
bool MatchCommonDeviceMethod(const mcucore::StringView& view,
                             EDeviceMethod& match);
}  // namespace internal
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_MATCH_LITERALS_H_
