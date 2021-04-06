# Bazel BUILD file for this package.

cc_library(
    name = "TinyAlpacaServer",
    hdrs = ["TinyAlpacaServer.h"],
    deps = [
        ":addresses",
        ":alpaca_discovery_server",
        ":alpaca_request",
        ":alpaca_response",
        ":any_printable",
        ":array",
        ":array_view",
        ":ascom_error_codes",
        ":config",
        ":connection",
        ":constants",
        ":counting_bitbucket",
        ":device_api_handler_base",
        ":device_info",
        ":eeprom_io",
        ":extra_parameters",
        ":hex_escape",
        ":http_response_header",
        ":inline_literal",
        ":ip_device",
        ":json_encoder",
        ":json_response",
        ":literal",
        ":literal_token",
        ":literals",
        ":log_sink",
        ":logging",
        ":match_literals",
        ":moving_average",
        ":o_print_stream",
        ":observing_conditions_adapter",
        ":platform",
        ":platform_ethernet",
        ":print_to_trait",
        ":printable_cat",
        ":request_decoder",
        ":request_decoder_listener",
        ":request_listener",
        ":server_connection",
        ":server_connection_base",
        ":server_description",
        ":status",
        ":status_or",
        ":string_compare",
        ":string_view",
        ":tiny_alpaca_server",
        ":tiny_string",
        ":type_traits",
        ":utils_config",
    ],
)

cc_library(
    name = "alpaca_discovery_server",
    srcs = ["alpaca_discovery_server.cc"],
    hdrs = ["alpaca_discovery_server.h"],
    deps = [
        ":hex_escape",
        ":literal",
        ":o_print_stream",
        ":platform",
        ":platform_ethernet",
        ":string_compare",
        ":string_view",
    ],
)

cc_library(
    name = "alpaca_request",
    srcs = ["alpaca_request.cc"],
    hdrs = ["alpaca_request.h"],
    deps = [
        ":config",
        ":constants",
        ":extra_parameters",
        ":logging",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "alpaca_response",
    srcs = ["alpaca_response.cc"],
    hdrs = ["alpaca_response.h"],
    deps = [
        ":alpaca_request",
        ":any_printable",
        ":ascom_error_codes",
        ":constants",
        ":counting_bitbucket",
        ":http_response_header",
        ":json_encoder",
        ":json_response",
        ":literals",
        ":platform",
        ":status_or",
    ],
)

cc_library(
    name = "ascom_error_codes",
    srcs = ["ascom_error_codes.cc"],
    hdrs = ["ascom_error_codes.h"],
    deps = [
        ":literals",
        ":platform",
        ":status",
    ],
)

# config.h specifies the set of features available (e.g. std::string) on
# the target platform of the Tiny Alpaca Server.
cc_library(
    name = "config",
    hdrs = ["config.h"],
)

cc_library(
    name = "constants",
    srcs = ["constants.cc"],
    hdrs = ["constants.h"],
    deps = [
        ":inline_literal",
        ":platform",
    ],
)

cc_library(
    name = "device_api_handler_base",
    srcs = ["device_api_handler_base.cc"],
    hdrs = ["device_api_handler_base.h"],
    deps = [
        ":alpaca_request",
        ":alpaca_response",
        ":ascom_error_codes",
        ":constants",
        ":counting_bitbucket",
        ":device_info",
        ":http_response_header",
        ":json_encoder",
        ":literals",
        ":platform",
    ],
)

cc_library(
    name = "device_info",
    srcs = ["device_info.cc"],
    hdrs = ["device_info.h"],
    deps = [
        ":constants",
        ":json_encoder",
        ":literal",
        ":literals",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "extra_parameters",
    hdrs = ["extra_parameters.h"],
    deps = [
        ":config",
        ":constants",
        ":platform",
        ":string_view",
        ":tiny_string",
    ],
)

cc_library(
    name = "http_response_header",
    srcs = ["http_response_header.cc"],
    hdrs = ["http_response_header.h"],
    deps = [
        ":constants",
        ":literal",
        ":literals",
        ":platform",
    ],
)

cc_library(
    name = "json_response",
    hdrs = ["json_response.h"],
    deps = [
        ":alpaca_request",
        ":any_printable",
        ":json_encoder",
        ":literals",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "literals",
    hdrs = ["literals.h"],
    textual_hdrs = ["literals.inc"],
    deps = [
        ":literal",
        ":platform",
    ],
)

cc_library(
    name = "log_experiment",
    hdrs = ["LogExperiment.h"],
    deps = [
        ":inline_literal",
        ":logging",
        ":platform",
    ],
)

cc_library(
    name = "match_literals",
    srcs = ["match_literals.cc"],
    hdrs = ["match_literals.h"],
    deps = [
        ":array_view",
        ":constants",
        ":literal",
        ":literal_token",
        ":literals",
        ":logging",
        ":string_view",
    ],
)

cc_library(
    name = "request_decoder",
    srcs = ["request_decoder.cc"],
    hdrs = ["request_decoder.h"],
    deps = [
        ":alpaca_request",
        ":config",
        ":constants",
        ":hex_escape",
        ":literals",
        ":logging",
        ":match_literals",
        ":platform",
        ":request_decoder_listener",
        ":string_compare",
        ":string_view",
    ],
)

cc_library(
    name = "request_decoder_listener",
    srcs = ["request_decoder_listener.cc"],
    hdrs = ["request_decoder_listener.h"],
    deps = [
        ":constants",
        ":hex_escape",
        ":logging",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "request_listener",
    hdrs = ["request_listener.h"],
    deps = [
        ":alpaca_request",
        ":constants",
        ":platform",
    ],
)

cc_library(
    name = "server_connection",
    srcs = ["server_connection.cc"],
    hdrs = ["server_connection.h"],
    deps = [
        ":alpaca_request",
        ":alpaca_response",
        ":constants",
        ":literals",
        ":platform",
        ":platform_ethernet",
        ":request_decoder",
        ":request_listener",
        ":server_connection_base",
        ":string_view",
    ],
)

cc_library(
    name = "server_description",
    srcs = ["server_description.cc"],
    hdrs = ["server_description.h"],
    deps = [
        ":json_encoder",
        ":literal",
        ":literals",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "tiny_alpaca_server",
    srcs = ["tiny_alpaca_server.cc"],
    hdrs = ["tiny_alpaca_server.h"],
    deps = [
        ":alpaca_response",
        ":any_printable",
        ":array",
        ":device_api_handler_base",
        ":literals",
        ":logging",
        ":platform",
        ":platform_ethernet",
        ":request_listener",
        ":server_connection",
        ":server_description",
        ":string_view",
    ],
)
