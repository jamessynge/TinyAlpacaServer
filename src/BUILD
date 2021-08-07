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
        "//TinyAlpacaServer/src/device_types:device_impl_base",
        "//TinyAlpacaServer/src/device_types/cover_calibrator:cover_calibrator_adapter",
        "//TinyAlpacaServer/src/device_types/cover_calibrator:cover_calibrator_constants",
        "//TinyAlpacaServer/src/device_types/observing_conditions:observing_conditions_adapter",
        "//TinyAlpacaServer/src/device_types/switch:multi_switch_adapter",
        "//TinyAlpacaServer/src/device_types/switch:switch_adapter",
        "//TinyAlpacaServer/src/device_types/switch:switch_interface",
        "//TinyAlpacaServer/src/device_types/switch:toggle_switch_base",
        "//TinyAlpacaServer/src/utils:addresses",
        "//TinyAlpacaServer/src/utils:any_printable",
        "//TinyAlpacaServer/src/utils:array",
        "//TinyAlpacaServer/src/utils:array_view",
        "//TinyAlpacaServer/src/utils:avr_timer_counter",
        "//TinyAlpacaServer/src/utils:connection",
        "//TinyAlpacaServer/src/utils:counting_print",
        "//TinyAlpacaServer/src/utils:eeprom_io",
        "//TinyAlpacaServer/src/utils:hex_escape",
        "//TinyAlpacaServer/src/utils:ip_device",
        "//TinyAlpacaServer/src/utils:json_encoder",
        "//TinyAlpacaServer/src/utils:json_encoder_helpers",
        "//TinyAlpacaServer/src/utils:literal",
        "//TinyAlpacaServer/src/utils:moving_average",
        "//TinyAlpacaServer/src/utils:platform_ethernet",
        "//TinyAlpacaServer/src/utils:print_misc",
        "//TinyAlpacaServer/src/utils:printable_cat",
        "//TinyAlpacaServer/src/utils:server_socket",
        "//TinyAlpacaServer/src/utils:socket_listener",
        "//TinyAlpacaServer/src/utils:status",
        "//TinyAlpacaServer/src/utils:status_or",
        "//TinyAlpacaServer/src/utils:stream_to_print",
        "//TinyAlpacaServer/src/utils:string_compare",
        "//TinyAlpacaServer/src/utils:string_view",
        "//TinyAlpacaServer/src/utils:tiny_string",
        "//mcucore/src:McuCore",
    ],
)

cc_library(
    name = "alpaca_devices",
    srcs = ["alpaca_devices.cc"],
    hdrs = ["alpaca_devices.h"],
    deps = [
        ":alpaca_request",
        ":alpaca_response",
        ":constants",
        ":device_interface",
        ":json_response",
        ":literals",
        ":request_listener",
        ":server_description",
        "//TinyAlpacaServer/src/device_types:device_impl_base",
        "//TinyAlpacaServer/src/utils:array_view",
        "//TinyAlpacaServer/src/utils:hex_escape",
        "//TinyAlpacaServer/src/utils:json_encoder",
        "//TinyAlpacaServer/src/utils:platform_ethernet",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "alpaca_discovery_server",
    srcs = ["alpaca_discovery_server.cc"],
    hdrs = ["alpaca_discovery_server.h"],
    deps = [
        "//TinyAlpacaServer/src/utils:hex_escape",
        "//TinyAlpacaServer/src/utils:literal",
        "//TinyAlpacaServer/src/utils:platform_ethernet",
        "//TinyAlpacaServer/src/utils:string_compare",
        "//TinyAlpacaServer/src/utils:string_view",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
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
        "//TinyAlpacaServer/src/utils:string_view",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
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
        "//TinyAlpacaServer/src/utils:any_printable",
        "//TinyAlpacaServer/src/utils:array_view",
        "//TinyAlpacaServer/src/utils:counting_print",
        "//TinyAlpacaServer/src/utils:json_encoder",
        "//TinyAlpacaServer/src/utils:json_encoder_helpers",
        "//TinyAlpacaServer/src/utils:printable_cat",
        "//TinyAlpacaServer/src/utils:status_or",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "ascom_error_codes",
    srcs = ["ascom_error_codes.cc"],
    hdrs = ["ascom_error_codes.h"],
    deps = [
        ":literals",
        "//TinyAlpacaServer/src/utils:status",
        "//mcucore/src:mcucore_platform",
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
        "//TinyAlpacaServer/src/utils:print_misc",
        "//mcucore/src:inline_literal",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "device_info",
    srcs = ["device_info.cc"],
    hdrs = ["device_info.h"],
    deps = [
        ":constants",
        ":literals",
        "//TinyAlpacaServer/src/utils:json_encoder",
        "//TinyAlpacaServer/src/utils:literal",
        "//TinyAlpacaServer/src/utils:string_view",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:progmem_string",
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
        "//TinyAlpacaServer/src/utils:array_view",
        "//TinyAlpacaServer/src/utils:json_encoder",
        "//TinyAlpacaServer/src/utils:json_encoder_helpers",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "extra_parameters",
    hdrs = ["extra_parameters.h"],
    deps = [
        ":config",
        ":constants",
        "//TinyAlpacaServer/src/utils:string_view",
        "//TinyAlpacaServer/src/utils:tiny_string",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "http_response_header",
    srcs = ["http_response_header.cc"],
    hdrs = ["http_response_header.h"],
    deps = [
        ":constants",
        ":literals",
        "//TinyAlpacaServer/src/utils:literal",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "json_response",
    hdrs = ["json_response.h"],
    deps = [
        ":alpaca_request",
        ":literals",
        "//TinyAlpacaServer/src/utils:any_printable",
        "//TinyAlpacaServer/src/utils:json_encoder",
        "//TinyAlpacaServer/src/utils:string_view",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "literals",
    srcs = ["literals.cc"],
    hdrs = ["literals.h"],
    textual_hdrs = ["literals.inc"],
    deps = [
        "//TinyAlpacaServer/src/utils:literal",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "match_literals",
    srcs = ["match_literals.cc"],
    hdrs = ["match_literals.h"],
    deps = [
        ":constants",
        ":literals",
        "//TinyAlpacaServer/src/utils:array_view",
        "//TinyAlpacaServer/src/utils:literal",
        "//TinyAlpacaServer/src/utils:string_compare",
        "//TinyAlpacaServer/src/utils:string_view",
        "//mcucore/src:logging",
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
        "//TinyAlpacaServer/src/utils:hex_escape",
        "//TinyAlpacaServer/src/utils:string_compare",
        "//TinyAlpacaServer/src/utils:string_view",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "request_decoder_listener",
    srcs = ["request_decoder_listener.cc"],
    hdrs = ["request_decoder_listener.h"],
    deps = [
        ":constants",
        "//TinyAlpacaServer/src/utils:hex_escape",
        "//TinyAlpacaServer/src/utils:string_view",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "request_listener",
    hdrs = ["request_listener.h"],
    deps = [
        ":alpaca_request",
        ":constants",
        "//mcucore/src:mcucore_platform",
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
        "//TinyAlpacaServer/src/utils:platform_ethernet",
        "//TinyAlpacaServer/src/utils:socket_listener",
        "//TinyAlpacaServer/src/utils:string_view",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "server_description",
    srcs = ["server_description.cc"],
    hdrs = ["server_description.h"],
    deps = [
        ":literals",
        "//TinyAlpacaServer/src/utils:json_encoder",
        "//TinyAlpacaServer/src/utils:string_view",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:progmem_string",
    ],
)

cc_library(
    name = "server_socket_and_connection",
    srcs = ["server_socket_and_connection.cc"],
    hdrs = ["server_socket_and_connection.h"],
    deps = [
        ":server_connection",
        "//TinyAlpacaServer/src/utils:server_socket",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "server_sockets_and_connections",
    srcs = ["server_sockets_and_connections.cc"],
    hdrs = ["server_sockets_and_connections.h"],
    deps = [
        ":request_listener",
        ":server_socket_and_connection",
        "//mcucore/src:mcucore_platform",
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
        "//TinyAlpacaServer/src/device_types:device_impl_base",
        "//TinyAlpacaServer/src/utils:any_printable",
        "//TinyAlpacaServer/src/utils:array_view",
        "//TinyAlpacaServer/src/utils:counting_print",
        "//TinyAlpacaServer/src/utils:json_encoder",
        "//TinyAlpacaServer/src/utils:json_encoder_helpers",
        "//TinyAlpacaServer/src/utils:printable_cat",
        "//mcucore/src:mcucore_platform",
    ],
)
