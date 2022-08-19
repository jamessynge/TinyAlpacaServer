#ifndef TINY_ALPACA_SERVER_SRC_TINYALPACASERVER_H_
#define TINY_ALPACA_SERVER_SRC_TINYALPACASERVER_H_

// This file acts to export all of the headers that would be needed by a program
// (i.e. an Arduino Sketch file) using the Tiny Alpaca Server.
//
// Author: james.synge@gmail.com

// TODO(jamessynge): (Maybe) Reduce this list to just the necessary headers,
// i.e. those that will be used by sketches using Tiny Alpaca Server.

#include "alpaca_devices.h"               // IWYU pragma: export
#include "alpaca_discovery_server.h"      // IWYU pragma: export
#include "alpaca_request.h"               // IWYU pragma: export
#include "alpaca_response.h"              // IWYU pragma: export
#include "ascom_error_codes.h"            // IWYU pragma: export
#include "config.h"                       // IWYU pragma: export
#include "configured_devices_response.h"  // IWYU pragma: export
#include "constants.h"                    // IWYU pragma: export
#include "device_description.h"           // IWYU pragma: export
#include "device_interface.h"             // IWYU pragma: export
#include "device_types/cover_calibrator/cover_calibrator_adapter.h"  // IWYU pragma: export
#include "device_types/cover_calibrator/cover_calibrator_constants.h"  // IWYU pragma: export
#include "device_types/device_impl_base.h"  // IWYU pragma: export
#include "device_types/observing_conditions/observing_conditions_adapter.h"  // IWYU pragma: export
#include "device_types/switch/multi_switch_adapter.h"  // IWYU pragma: export
#include "device_types/switch/switch_adapter.h"        // IWYU pragma: export
#include "device_types/switch/switch_interface.h"      // IWYU pragma: export
#include "device_types/switch/toggle_switch_base.h"    // IWYU pragma: export
#include "eeprom_ids.h"                                // IWYU pragma: export
#include "extra_parameters.h"                          // IWYU pragma: export
#include "http_response_header.h"                      // IWYU pragma: export
#include "json_response.h"                             // IWYU pragma: export
#include "literals.h"                                  // IWYU pragma: export
#include "match_literals.h"                            // IWYU pragma: export
#include "request_decoder.h"                           // IWYU pragma: export
#include "request_decoder_listener.h"                  // IWYU pragma: export
#include "request_listener.h"                          // IWYU pragma: export
#include "server_connection.h"                         // IWYU pragma: export
#include "server_context.h"                            // IWYU pragma: export
#include "server_description.h"                        // IWYU pragma: export
#include "server_socket_and_connection.h"              // IWYU pragma: export
#include "server_sockets_and_connections.h"            // IWYU pragma: export
#include "tiny_alpaca_device_server.h"                 // IWYU pragma: export
#include "tiny_alpaca_network_server.h"                // IWYU pragma: export
#include "utils/avr_timer_counter.h"                   // IWYU pragma: export
#include "utils/moving_average.h"                      // IWYU pragma: export

#endif  // TINY_ALPACA_SERVER_SRC_TINYALPACASERVER_H_
