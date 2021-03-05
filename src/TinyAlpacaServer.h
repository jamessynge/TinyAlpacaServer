#ifndef TINY_ALPACA_SERVER_SRC_TINYALPACASERVER_H_
#define TINY_ALPACA_SERVER_SRC_TINYALPACASERVER_H_

// This file acts to export all of the headers that would be needed by a program
// (i.e. an Arduino Sketch file) using the Tiny Alpaca Server.

// TODO(jamessynge): Reduce this list to just the necessary headers.

#include "alpaca_request.h"
#include "alpaca_response.h"
#include "any_string.h"
#include "config.h"
#include "constants.h"
#include "counting_bitbucket.h"
#include "device_api_handler_base.h"
#include "device_info.h"
#include "escaping.h"
#include "extra_parameters.h"
#include "http_response_header.h"
#include "json_encoder.h"
#include "json_response.h"
#include "literal.h"
#include "literal_token.h"
#include "literals.h"
#include "logging.h"
#include "match_literals.h"
#include "platform.h"
#include "platform_ethernet.h"
#include "request_decoder.h"
#include "request_decoder_listener.h"
#include "server_description.h"
#include "string_compare.h"
#include "string_view.h"
#include "tiny_alpaca_server.h"
#include "tiny_string.h"

#endif  // TINY_ALPACA_SERVER_SRC_TINYALPACASERVER_H_
