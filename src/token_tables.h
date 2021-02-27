#ifndef TINY_ALPACA_SERVER_SRC_TOKEN_TABLES_H_
#define TINY_ALPACA_SERVER_SRC_TOKEN_TABLES_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

#include "decoder_constants.h"
#include "platform.h"
#include "string_view.h"

namespace alpaca {

bool MatchHttpMethod(const StringView& view, EHttpMethod& match);

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_TOKEN_TABLES_H_
