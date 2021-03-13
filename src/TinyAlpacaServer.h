#ifndef TINY_ALPACA_SERVER_SRC_TINYALPACASERVER_H_
#define TINY_ALPACA_SERVER_SRC_TINYALPACASERVER_H_

// This file acts to export all of the headers that would be needed by a program
// (i.e. an Arduino Sketch file) using the Tiny Alpaca Server.

// TODO(jamessynge): Reduce this list to just the necessary headers.

#include "alpaca_request.h"
#include "alpaca_response.h"
#include "ascom_error_codes.h"
#include "config.h"
#include "constants.h"
#include "device_api_handler_base.h"
#include "device_info.h"
#include "device_type_adapters/observing_conditions_adapter.h"
#include "extra_parameters.h"
#include "http_response_header.h"
#include "json_response.h"
#include "literals.h"
#include "match_literals.h"
#include "request_decoder.h"
#include "request_decoder_listener.h"
#include "request_listener.h"
#include "server_connection.h"
#include "server_description.h"
#include "tiny_alpaca_server.h"
#include "utils/any_printable.h"
#include "utils/any_string.h"
#include "utils/array.h"
#include "utils/array_view.h"
#include "utils/connection.h"
#include "utils/counting_bitbucket.h"
#include "utils/escaping.h"
#include "utils/json_encoder.h"
#include "utils/literal.h"
#include "utils/literal_token.h"
#include "utils/logging.h"
#include "utils/platform.h"
#include "utils/platform_ethernet.h"
#include "utils/printable_cat.h"
#include "utils/server_connection_base.h"
#include "utils/status.h"
#include "utils/status_or.h"
#include "utils/string_compare.h"
#include "utils/string_view.h"
#include "utils/tiny_string.h"

#endif  // TINY_ALPACA_SERVER_SRC_TINYALPACASERVER_H_
