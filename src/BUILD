# Bazel BUILD file for Tiny Alpaca Server.
# NOTE: Needs much work to support building on an arbitrary machine.

load(
    "//TinyAlpacaServer/extras/dev_tools:build_config.bzl",
    "build_config",
)

package(
    default_copts = build_config.default_copts,
)

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
        ":configured_devices_response",
        ":constants",
        ":device_description",
        ":device_interface",
        ":eeprom_ids",
        ":extra_parameters",
        ":http_response_header",
        ":json_response",
        ":literals",
        ":match_literals",
        ":request_decoder",
        ":request_decoder_listener",
        ":request_listener",
        ":server_connection",
        ":server_context",
        ":server_description",
        ":server_socket_and_connection",
        ":server_sockets_and_connections",
        ":tiny_alpaca_device_server",
        ":tiny_alpaca_network_server",
        "//TinyAlpacaServer/src/device_types:device_impl_base",
        "//TinyAlpacaServer/src/device_types/cover_calibrator:cover_calibrator_adapter",
        "//TinyAlpacaServer/src/device_types/cover_calibrator:cover_calibrator_constants",
        "//TinyAlpacaServer/src/device_types/observing_conditions:observing_conditions_adapter",
        "//TinyAlpacaServer/src/device_types/switch:multi_switch_adapter",
        "//TinyAlpacaServer/src/device_types/switch:switch_adapter",
        "//TinyAlpacaServer/src/device_types/switch:switch_interface",
        "//TinyAlpacaServer/src/device_types/switch:toggle_switch_base",
        "//TinyAlpacaServer/src/utils:moving_average",
    ],
)

cc_library(
    name = "alpaca_devices",
    srcs = ["alpaca_devices.cc"],
    hdrs = ["alpaca_devices.h"],
    deps = [
        ":alpaca_request",
        ":alpaca_response",
        ":configured_devices_response",
        ":constants",
        ":device_description",
        ":device_interface",
        ":literals",
        ":server_context",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/container:array_view",
        "//mcucore/src/json:json_encoder",
        "//mcucore/src/log",
        "//mcucore/src/print:hex_escape",
        "//mcucore/src/print:printable_cat",
        "//mcucore/src/strings:progmem_string_data",
        "//mcunet/src:platform_network",
    ],
)

cc_library(
    name = "alpaca_discovery_server",
    srcs = ["alpaca_discovery_server.cc"],
    hdrs = ["alpaca_discovery_server.h"],
    deps = [
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/log",
        "//mcucore/src/print:hex_escape",
        "//mcucore/src/strings:progmem_string_data",
        "//mcucore/src/strings:string_compare",
        "//mcucore/src/strings:string_view",
        "//mcunet/src:platform_network",
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
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/log",
        "//mcucore/src/strings:string_view",
        "//mcucore/src/strings:tiny_string",
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
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/container:array_view",
        "//mcucore/src/json:json_encoder",
        "//mcucore/src/json:json_encoder_helpers",
        "//mcucore/src/print:any_printable",
        "//mcucore/src/print:counting_print",
        "//mcucore/src/print:printable_cat",
        "//mcucore/src/status:status_or",
        "//mcucore/src/strings:progmem_string",
    ],
)

cc_library(
    name = "ascom_error_codes",
    srcs = ["ascom_error_codes.cc"],
    hdrs = ["ascom_error_codes.h"],
    deps = [
        ":literals",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/status",
    ],
)

# config.h specifies the set of features available in the server (e.g. storing extra
# parameters beyond those with hardcoded support in AlpacaRequest and RequestDecoder).
cc_library(
    name = "config",
    hdrs = ["config.h"],
    deps = ["//mcucore/src:mcucore_platform"],
)

cc_library(
    name = "configured_devices_response",
    srcs = ["configured_devices_response.cc"],
    hdrs = ["configured_devices_response.h"],
    deps = [
        ":alpaca_request",
        ":device_interface",
        ":json_response",
        "//mcucore/src/container:array_view",
    ],
)

cc_library(
    name = "constants",
    srcs = ["constants.cc"],
    hdrs = ["constants.h"],
    deps = [
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/container:flash_string_table",
        "//mcucore/src/print:print_misc",
        "//mcucore/src/print:print_to_buffer",
        "//mcucore/src/strings:progmem_string_data",
    ],
)

cc_library(
    name = "device_description",
    srcs = ["device_description.cc"],
    hdrs = ["device_description.h"],
    deps = [
        ":constants",
        ":eeprom_ids",
        ":literals",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/eeprom:eeprom_tlv",
        "//mcucore/src/json:json_encoder",
        "//mcucore/src/misc:uuid",
        "//mcucore/src/status:status_or",
        "//mcucore/src/strings:progmem_string",
    ],
)

cc_library(
    name = "device_interface",
    srcs = ["device_interface.cc"],
    hdrs = ["device_interface.h"],
    deps = [
        ":alpaca_request",
        ":constants",
        ":device_description",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/json:json_encoder",
    ],
)

cc_library(
    name = "eeprom_ids",
    hdrs = ["eeprom_ids.h"],
    deps = ["//mcucore/src:mcucore_platform"],
)

cc_library(
    name = "extra_parameters",
    hdrs = ["extra_parameters.h"],
    deps = [
        ":config",
        ":constants",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/strings:string_view",
        "//mcucore/src/strings:tiny_string",
    ],
)

cc_library(
    name = "http_response_header",
    srcs = ["http_response_header.cc"],
    hdrs = ["http_response_header.h"],
    deps = [
        ":constants",
        ":literals",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/semistd:limits",
        "//mcucore/src/strings:progmem_string",
        "//mcucore/src/strings:progmem_string_view",
    ],
)

cc_library(
    name = "json_response",
    hdrs = ["json_response.h"],
    deps = [
        ":alpaca_request",
        ":literals",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/json:json_encoder",
        "//mcucore/src/print:any_printable",
        "//mcucore/src/strings:progmem_string_view",
    ],
)

cc_library(
    name = "literals",
    srcs = ["literals.cc"],
    hdrs = ["literals.h"],
    textual_hdrs = ["literals.inc"],
    deps = [
        "//mcucore/src:McuCore",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/strings:progmem_string",
        "//mcucore/src/strings:progmem_string_view",
    ],
)

cc_library(
    name = "match_literals",
    srcs = ["match_literals.cc"],
    hdrs = ["match_literals.h"],
    deps = [
        ":constants",
        ":literals",
        "//mcucore/src:mcucore_config",
        "//mcucore/src/container:array_view",
        "//mcucore/src/log",
        "//mcucore/src/print:printable_cat",
        "//mcucore/src/strings:string_compare",
        "//mcucore/src/strings:string_view",
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
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/log",
        "//mcucore/src/print:hex_escape",
        "//mcucore/src/strings:progmem_string_data",
        "//mcucore/src/strings:progmem_string_view",
        "//mcucore/src/strings:string_compare",
        "//mcucore/src/strings:string_view",
    ],
)

cc_library(
    name = "request_decoder_listener",
    srcs = ["request_decoder_listener.cc"],
    hdrs = ["request_decoder_listener.h"],
    deps = [
        ":config",
        ":constants",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/log",
        "//mcucore/src/print:hex_escape",
        "//mcucore/src/strings:string_view",
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
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/strings:string_view",
        "//mcunet/src:connection",
        "//mcunet/src:platform_network",
        "//mcunet/src:socket_listener",
    ],
)

cc_library(
    name = "server_context",
    srcs = ["server_context.cc"],
    hdrs = ["server_context.h"],
    deps = [
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/eeprom:eeprom_tlv",
    ],
)

cc_library(
    name = "server_description",
    srcs = ["server_description.cc"],
    hdrs = ["server_description.h"],
    deps = [
        ":literals",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/json:json_encoder",
        "//mcucore/src/strings:progmem_string",
        "//mcucore/src/strings:string_view",
    ],
)

cc_library(
    name = "server_socket_and_connection",
    srcs = ["server_socket_and_connection.cc"],
    hdrs = ["server_socket_and_connection.h"],
    deps = [
        ":request_listener",
        ":server_connection",
        "//mcucore/src:mcucore_platform",
        "//mcunet/src:server_socket",
    ],
)

cc_library(
    name = "server_sockets_and_connections",
    srcs = ["server_sockets_and_connections.cc"],
    hdrs = ["server_sockets_and_connections.h"],
    deps = [
        ":config",
        ":request_listener",
        ":server_socket_and_connection",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/log",
    ],
)

cc_library(
    name = "tiny_alpaca_device_server",
    srcs = ["tiny_alpaca_device_server.cc"],
    hdrs = ["tiny_alpaca_device_server.h"],
    deps = [
        ":alpaca_devices",
        ":alpaca_discovery_server",
        ":alpaca_response",
        ":constants",
        ":device_interface",
        ":http_response_header",
        ":literals",
        ":server_context",
        ":server_description",
        ":server_sockets_and_connections",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/container:array_view",
        "//mcucore/src/json:json_encoder",
        "//mcucore/src/json:json_encoder_helpers",
        "//mcucore/src/print:any_printable",
        "//mcucore/src/print:printable_cat",
        "//mcucore/src/strings:progmem_string_data",
    ],
)

cc_library(
    name = "tiny_alpaca_network_server",
    srcs = ["tiny_alpaca_network_server.cc"],
    hdrs = ["tiny_alpaca_network_server.h"],
    deps = [
        ":alpaca_devices",
        ":alpaca_discovery_server",
        ":alpaca_response",
        ":constants",
        ":device_interface",
        ":http_response_header",
        ":literals",
        ":server_context",
        ":server_description",
        ":server_sockets_and_connections",
        ":tiny_alpaca_device_server",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/container:array_view",
        "//mcucore/src/json:json_encoder",
        "//mcucore/src/json:json_encoder_helpers",
        "//mcucore/src/print:any_printable",
        "//mcucore/src/print:printable_cat",
        "//mcucore/src/strings:progmem_string_data",
    ],
)
