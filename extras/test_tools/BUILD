# Libraries intended to simplify the task of writing tests of Tiny Alpaca Server.

cc_library(
    name = "decode_and_dispatch_test_base",
    testonly = True,
    srcs = ["decode_and_dispatch_test_base.cc"],
    hdrs = ["decode_and_dispatch_test_base.h"],
    deps = [
        ":mock_device_interface",
        ":mock_switch_group",
        ":test_tiny_alpaca_server",
        "//TinyAlpacaServer/src:alpaca_devices",
        "//TinyAlpacaServer/src:alpaca_discovery_server",
        "//TinyAlpacaServer/src:alpaca_response",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:device_interface",
        "//TinyAlpacaServer/src:literals",
        "//TinyAlpacaServer/src:request_listener",
        "//TinyAlpacaServer/src:server_connection",
        "//TinyAlpacaServer/src:server_description",
        "//TinyAlpacaServer/src:server_sockets_and_connections",
        "//TinyAlpacaServer/src:tiny_alpaca_server",
        "//TinyAlpacaServer/src/device_types:device_impl_base",
        "//absl/status",
        "//absl/strings",
        "//googletest:gunit_headers",
        "//mcucore/extras/test_tools:http_request",
        "//mcucore/extras/test_tools:http_response",
        "//mcucore/extras/test_tools:json_decoder",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/extras/test_tools:sample_printable",
        "//mcucore/src:array_view",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:status",
        "//mcucore/src:string_view",
        "//mcunet/src:platform_ethernet",
        "//util/task:status",
    ],
)

cc_library(
    name = "mock_device_interface",
    hdrs = ["mock_device_interface.h"],
    deps = [
        "//TinyAlpacaServer/src:alpaca_request",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:device_info",
        "//TinyAlpacaServer/src:device_interface",
        "//googletest:gunit_headers",
    ],
)

cc_library(
    name = "mock_observing_conditions",
    hdrs = ["mock_observing_conditions.h"],
    deps = [
        "//TinyAlpacaServer/src:device_info",
        "//TinyAlpacaServer/src/device_types/observing_conditions:observing_conditions_adapter",
        "//googletest:gunit_headers",
        "//mcucore/src:status",
        "//mcucore/src:status_or",
    ],
)

cc_library(
    name = "mock_request_decoder_listener",
    hdrs = ["mock_request_decoder_listener.h"],
    deps = [
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:request_decoder_listener",
        "//googletest:gunit_headers",
        "//mcucore/src:string_view",
    ],
)

cc_library(
    name = "mock_request_listener",
    hdrs = ["mock_request_listener.h"],
    deps = [
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:request_listener",
        "//googletest:gunit_headers",
    ],
)

cc_library(
    name = "mock_switch_group",
    hdrs = ["mock_switch_group.h"],
    deps = [
        "//TinyAlpacaServer/src:device_info",
        "//TinyAlpacaServer/src/device_types/switch:switch_adapter",
        "//googletest:gunit_headers",
        "//mcucore/src:status",
        "//mcucore/src:status_or",
    ],
)

cc_library(
    name = "test_tiny_alpaca_server",
    testonly = True,
    srcs = ["test_tiny_alpaca_server.cc"],
    hdrs = ["test_tiny_alpaca_server.h"],
    deps = [
        "//TinyAlpacaServer/src:device_interface",
        "//TinyAlpacaServer/src:server_connection",
        "//TinyAlpacaServer/src:server_description",
        "//TinyAlpacaServer/src:tiny_alpaca_server",
        "//absl/strings",
        "//mcucore/src:array_view",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:o_print_stream",
        "//mcucore/src:progmem_string_data",
        "//mcunet/extras/host/ethernet5500:ethernet_config",
        "//mcunet/extras/test_tools:string_io_connection",
    ],
)
