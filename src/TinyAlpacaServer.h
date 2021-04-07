#ifndef TINY_ALPACA_SERVER_SRC_TINYALPACASERVER_H_
#define TINY_ALPACA_SERVER_SRC_TINYALPACASERVER_H_

// This file acts to export all of the headers that would be needed by a program
// (i.e. an Arduino Sketch file) using the Tiny Alpaca Server.

// TODO(jamessynge): Reduce this list to just the necessary headers. This will
// probably be helped along by splitting this huge library into several smaller
// ones (e.g. much of src/utils could be moved to a generic Arduino helpers
// library).

#include "alpaca_discovery_server.h"  // IWYU pragma: export
#include "alpaca_request.h"           // IWYU pragma: export
#include "alpaca_response.h"          // IWYU pragma: export
#include "ascom_error_codes.h"        // IWYU pragma: export
#include "config.h"                   // IWYU pragma: export
#include "constants.h"                // IWYU pragma: export
#include "device_api_handler_base.h"  // IWYU pragma: export
#include "device_info.h"              // IWYU pragma: export
#include "device_type_adapters/observing_conditions_adapter.h"  // IWYU pragma: export
#include "extra_parameters.h"                // IWYU pragma: export
#include "http_response_header.h"            // IWYU pragma: export
#include "json_response.h"                   // IWYU pragma: export
#include "literals.h"                        // IWYU pragma: export
#include "match_literals.h"                  // IWYU pragma: export
#include "request_decoder.h"                 // IWYU pragma: export
#include "request_decoder_listener.h"        // IWYU pragma: export
#include "request_listener.h"                // IWYU pragma: export
#include "server_connection.h"               // IWYU pragma: export
#include "server_description.h"              // IWYU pragma: export
#include "tiny_alpaca_request_handler.h"     // IWYU pragma: export
#include "tiny_alpaca_server.h"              // IWYU pragma: export
#include "utils/addresses.h"                 // IWYU pragma: export
#include "utils/any_printable.h"             // IWYU pragma: export
#include "utils/array.h"                     // IWYU pragma: export
#include "utils/array_view.h"                // IWYU pragma: export
#include "utils/connection.h"                // IWYU pragma: export
#include "utils/counting_bitbucket.h"        // IWYU pragma: export
#include "utils/eeprom_io.h"                 // IWYU pragma: export
#include "utils/hex_escape.h"                // IWYU pragma: export
#include "utils/inline_literal.h"            // IWYU pragma: export
#include "utils/ip_device.h"                 // IWYU pragma: export
#include "utils/json_encoder.h"              // IWYU pragma: export
#include "utils/literal.h"                   // IWYU pragma: export
#include "utils/literal_token.h"             // IWYU pragma: export
#include "utils/log_sink.h"                  // IWYU pragma: export
#include "utils/logging.h"                   // IWYU pragma: export
#include "utils/moving_average.h"            // IWYU pragma: export
#include "utils/o_print_stream.h"            // IWYU pragma: export
#include "utils/platform.h"                  // IWYU pragma: export
#include "utils/platform_ethernet.h"         // IWYU pragma: export
#include "utils/printable_cat.h"             // IWYU pragma: export
#include "utils/printable_progmem_string.h"  // IWYU pragma: export
#include "utils/server_connection_base.h"    // IWYU pragma: export
#include "utils/status.h"                    // IWYU pragma: export
#include "utils/status_or.h"                 // IWYU pragma: export
#include "utils/string_compare.h"            // IWYU pragma: export
#include "utils/string_view.h"               // IWYU pragma: export
#include "utils/tiny_string.h"               // IWYU pragma: export
#include "utils/traits/print_to_trait.h"     // IWYU pragma: export
#include "utils/traits/type_traits.h"        // IWYU pragma: export
#include "utils/utils_config.h"              // IWYU pragma: export

#endif  // TINY_ALPACA_SERVER_SRC_TINYALPACASERVER_H_
