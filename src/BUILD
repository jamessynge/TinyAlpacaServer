# Bazel BUILD file for Tiny Alpaca Server.
# NOTE: Needs much work to support building on an arbitrary machine.

cc_library(
    name = "TinyAlpacaServer",
    hdrs = ["TinyAlpacaServer.h"],
    deps = [
        ":alpaca_devices",
        ":alpaca_discovery_server",
        ":alpaca_request",
        ":alpaca_response",
        ":ascom_error_codes",
        ":config",
        ":constants",
        ":device_info",
        ":device_interface",
        ":extra_parameters",
        ":http_response_header",
        ":json_response",
        ":literals",
        ":match_literals",
        ":request_decoder",
        ":request_decoder_listener",
        ":request_listener",
        ":server_connection",
        ":server_description",
        ":server_socket_and_connection",
        ":server_sockets_and_connections",
        ":tiny_alpaca_server",
        "//src/device_types:device_impl_base",
        "//src/device_types/cover_calibrator:cover_calibrator_adapter",
        "//src/device_types/cover_calibrator:cover_calibrator_constants",
        "//src/device_types/observing_conditions:observing_conditions_adapter",
        "//src/device_types/switch:multi_switch_adapter",
        "//src/device_types/switch:switch_adapter",
        "//src/device_types/switch:switch_interface",
        "//src/utils:addresses",
        "//src/utils:any_printable",
        "//src/utils:array",
        "//src/utils:array_view",
        "//src/utils:avr_timer_counter",
        "//src/utils:connection",
        "//src/utils:counting_print",
        "//src/utils:eeprom_io",
        "//src/utils:hex_escape",
        "//src/utils:inline_literal",
        "//src/utils:ip_device",
        "//src/utils:json_encoder",
        "//src/utils:json_encoder_helpers",
        "//src/utils:literal",
        "//src/utils:log_sink",
        "//src/utils:logging",
        "//src/utils:moving_average",
        "//src/utils:o_print_stream",
        "//src/utils:platform",
        "//src/utils:platform_ethernet",
        "//src/utils:print_misc",
        "//src/utils:printable_cat",
        "//src/utils:printable_progmem_string",
        "//src/utils:server_socket",
        "//src/utils:socket_listener",
        "//src/utils:status",
        "//src/utils:status_or",
        "//src/utils:stream_to_print",
        "//src/utils:string_compare",
        "//src/utils:string_view",
        "//src/utils:tiny_string",
        "//src/utils:utils_config",
        "//src/utils/traits:print_to_trait",
        "//src/utils/traits:type_traits",
    ],
)

cc_library(
    name = "alpaca_devices",
    srcs = ["alpaca_devices.cc"],
    hdrs = ["alpaca_devices.h"],
    deps = [
        ":alpaca_response",
        ":ascom_error_codes",
        ":constants",
        ":json_response",
        ":literals",
        ":request_listener",
        ":server_description",
        "//src/device_types:device_impl_base",
        "//src/utils:any_printable",
        "//src/utils:array_view",
        "//src/utils:hex_escape",
        "//src/utils:json_encoder",
        "//src/utils:logging",
        "//src/utils:platform",
        "//src/utils:platform_ethernet",
        "//src/utils:string_view",
    ],
)

cc_library(
    name = "alpaca_discovery_server",
    srcs = ["alpaca_discovery_server.cc"],
    hdrs = ["alpaca_discovery_server.h"],
    deps = [
        "//src/utils:hex_escape",
        "//src/utils:literal",
        "//src/utils:o_print_stream",
        "//src/utils:platform",
        "//src/utils:platform_ethernet",
        "//src/utils:string_compare",
        "//src/utils:string_view",
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
        "//src/utils:logging",
        "//src/utils:platform",
        "//src/utils:string_view",
    ],
)

cc_library(
    name = "alpaca_response",
    srcs = ["alpaca_response.cc"],
    hdrs = ["alpaca_response.h"],
    deps = [
        ":alpaca_request",
        ":ascom_error_codes",
        ":constants",
        ":http_response_header",
        ":json_response",
        ":literals",
        "//src/utils:any_printable",
        "//src/utils:array_view",
        "//src/utils:counting_print",
        "//src/utils:json_encoder",
        "//src/utils:json_encoder_helpers",
        "//src/utils:platform",
        "//src/utils:printable_cat",
        "//src/utils:status_or",
    ],
)

cc_library(
    name = "ascom_error_codes",
    srcs = ["ascom_error_codes.cc"],
    hdrs = ["ascom_error_codes.h"],
    deps = [
        ":literals",
        "//src/utils:platform",
        "//src/utils:status",
    ],
)

# config.h specifies the set of features available in the server (e.g. storing extra
# parameters beyond those with hardcoded support in AlpacaRequest and RequestDecoder).
cc_library(
    name = "config",
    hdrs = ["config.h"],
)

cc_library(
    name = "constants",
    srcs = ["constants.cc"],
    hdrs = ["constants.h"],
    deps = [
        "//src/utils:inline_literal",
        "//src/utils:platform",
        "//src/utils:print_misc",
        "//src/utils:printable_progmem_string",
    ],
)

cc_library(
    name = "device_info",
    srcs = ["device_info.cc"],
    hdrs = ["device_info.h"],
    deps = [
        ":constants",
        ":literals",
        "//src/utils:json_encoder",
        "//src/utils:literal",
        "//src/utils:platform",
        "//src/utils:string_view",
    ],
)

cc_library(
    name = "device_interface",
    srcs = ["device_interface.cc"],
    hdrs = ["device_interface.h"],
    deps = [
        ":alpaca_request",
        ":device_info",
        ":json_response",
        "//src/utils:array_view",
        "//src/utils:json_encoder",
        "//src/utils:json_encoder_helpers",
        "//src/utils:platform",
    ],
)

cc_library(
    name = "extra_parameters",
    hdrs = ["extra_parameters.h"],
    deps = [
        ":config",
        ":constants",
        "//src/utils:platform",
        "//src/utils:string_view",
        "//src/utils:tiny_string",
    ],
)

cc_library(
    name = "http_response_header",
    srcs = ["http_response_header.cc"],
    hdrs = ["http_response_header.h"],
    deps = [
        ":constants",
        ":literals",
        "//src/utils:literal",
        "//src/utils:platform",
    ],
)

cc_library(
    name = "json_response",
    hdrs = ["json_response.h"],
    deps = [
        ":alpaca_request",
        ":literals",
        "//src/utils:any_printable",
        "//src/utils:json_encoder",
        "//src/utils:platform",
        "//src/utils:string_view",
    ],
)

cc_library(
    name = "literals",
    srcs = ["literals.cc"],
    hdrs = ["literals.h"],
    textual_hdrs = ["literals.inc"],
    deps = [
        "//src/utils:literal",
        "//src/utils:platform",
    ],
)

cc_library(
    name = "log_experiment",
    hdrs = ["LogExperiment.h"],
    deps = [
        "//src/utils:inline_literal",
        "//src/utils:logging",
        "//src/utils:platform",
    ],
)

cc_library(
    name = "match_literals",
    srcs = ["match_literals.cc"],
    hdrs = ["match_literals.h"],
    deps = [
        ":constants",
        ":literals",
        "//src/utils:array_view",
        "//src/utils:literal",
        "//src/utils:logging",
        "//src/utils:string_compare",
        "//src/utils:string_view",
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
        ":literals",
        ":match_literals",
        ":request_decoder_listener",
        "//src/utils:hex_escape",
        "//src/utils:logging",
        "//src/utils:platform",
        "//src/utils:string_compare",
        "//src/utils:string_view",
    ],
)

cc_library(
    name = "request_decoder_listener",
    srcs = ["request_decoder_listener.cc"],
    hdrs = ["request_decoder_listener.h"],
    deps = [
        ":constants",
        "//src/utils:hex_escape",
        "//src/utils:logging",
        "//src/utils:platform",
        "//src/utils:string_view",
    ],
)

cc_library(
    name = "request_listener",
    hdrs = ["request_listener.h"],
    deps = [
        ":alpaca_request",
        ":constants",
        "//src/utils:platform",
    ],
)

cc_library(
    name = "server_connection",
    srcs = ["server_connection.cc"],
    hdrs = ["server_connection.h"],
    deps = [
        ":alpaca_request",
        ":alpaca_response",
        ":config",
        ":constants",
        ":literals",
        ":request_decoder",
        ":request_listener",
        "//src/utils:platform",
        "//src/utils:platform_ethernet",
        "//src/utils:socket_listener",
        "//src/utils:string_view",
    ],
)

cc_library(
    name = "server_description",
    srcs = ["server_description.cc"],
    hdrs = ["server_description.h"],
    deps = [
        ":literals",
        "//src/utils:json_encoder",
        "//src/utils:literal",
        "//src/utils:platform",
        "//src/utils:string_view",
    ],
)

cc_library(
    name = "server_socket_and_connection",
    srcs = ["server_socket_and_connection.cc"],
    hdrs = ["server_socket_and_connection.h"],
    deps = [
        ":server_connection",
        "//src/utils:platform",
        "//src/utils:server_socket",
    ],
)

cc_library(
    name = "server_sockets_and_connections",
    srcs = ["server_sockets_and_connections.cc"],
    hdrs = ["server_sockets_and_connections.h"],
    deps = [
        ":request_listener",
        ":server_socket_and_connection",
        "//src/utils:platform",
    ],
)

cc_library(
    name = "tiny_alpaca_server",
    srcs = ["tiny_alpaca_server.cc"],
    hdrs = ["tiny_alpaca_server.h"],
    deps = [
        ":alpaca_devices",
        ":alpaca_discovery_server",
        ":alpaca_response",
        ":constants",
        ":http_response_header",
        ":literals",
        ":server_description",
        ":server_sockets_and_connections",
        "//src/device_types:device_impl_base",
        "//src/utils:array_view",
        "//src/utils:counting_print",
        "//src/utils:json_encoder",
        "//src/utils:json_encoder_helpers",
        "//src/utils:platform",
        "//src/utils:printable_cat",
    ],
)
