# Libraries intended to simplify the task of writing tests of Tiny Alpaca Server.

cc_library(
    name = "decode_and_dispatch_test_base",
    testonly = True,
    srcs = ["decode_and_dispatch_test_base.cc"],
    hdrs = ["decode_and_dispatch_test_base.h"],
    deps = [
        ":http_request",
        ":http_response",
        ":json_decoder",
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
        "//TinyAlpacaServer/src/utils:array_view",
        "//TinyAlpacaServer/src/utils:platform_ethernet",
        "//TinyAlpacaServer/src/utils:status",
        "//TinyAlpacaServer/src/utils:string_view",
        "//absl/status",
        "//absl/strings",
        "//googletest:gunit_headers",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/extras/test_tools:sample_printable",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//util/task:status",
    ],
)

cc_library(
    name = "fake_platform_ethernet",
    hdrs = ["fake_platform_ethernet.h"],
    deps = ["//TinyAlpacaServer/src/utils:platform_ethernet"],
)

cc_library(
    name = "http_request",
    srcs = ["http_request.cc"],
    hdrs = ["http_request.h"],
    deps = [
        "//absl/status",
        "//absl/status:statusor",
        "//absl/strings",
        "//mcucore/extras/test_tools:case_insensitive_less",
    ],
)

cc_library(
    name = "http_response",
    srcs = ["http_response.cc"],
    hdrs = ["http_response.h"],
    deps = [
        ":json_decoder",
        "//absl/status",
        "//absl/status:statusor",
        "//absl/strings",
        "//mcucore/extras/test_tools:case_insensitive_less",
        "//util/task:status",
    ],
)

cc_test(
    name = "http_response_test",
    srcs = ["http_response_test.cc"],
    deps = [
        ":http_response",
        ":json_decoder",
        "//absl/status",
        "//absl/status:statusor",
        "//googletest:gunit_main",
        "//util/task:status",
    ],
)

cc_library(
    name = "json_decoder",
    srcs = ["json_decoder.cc"],
    hdrs = ["json_decoder.h"],
    deps = [
        "//absl/status",
        "//absl/status:statusor",
        "//absl/strings",
        "//glog:logging",
        "//googletest:gunit_headers",
        "//mcucore/src:logging",
        "//util/task:status",
    ],
)

cc_test(
    name = "json_decoder_test",
    srcs = ["json_decoder_test.cc"],
    deps = [
        ":json_decoder",
        "//absl/status:statusor",
        "//googletest:gunit_main",
        "//util/task:status",
    ],
)

cc_library(
    name = "json_test_utils",
    srcs = ["json_test_utils.cc"],
    hdrs = ["json_test_utils.h"],
    deps = [
        "//TinyAlpacaServer/src/utils:any_printable",
        "//TinyAlpacaServer/src/utils:json_encoder",
        "//mcucore/extras/host/arduino:print",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "literal_utils",
    hdrs = ["literal_utils.h"],
    deps = ["//TinyAlpacaServer/src/utils:literal"],
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
        "//TinyAlpacaServer/src/utils:status",
        "//TinyAlpacaServer/src/utils:status_or",
        "//googletest:gunit_headers",
    ],
)

cc_library(
    name = "mock_request_decoder_listener",
    hdrs = ["mock_request_decoder_listener.h"],
    deps = [
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:request_decoder_listener",
        "//TinyAlpacaServer/src/utils:string_view",
        "//googletest:gunit_headers",
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
    name = "mock_socket_listener",
    hdrs = ["mock_socket_listener.h"],
    deps = [
        "//TinyAlpacaServer/src/utils:connection",
        "//TinyAlpacaServer/src/utils:socket_listener",
        "//googletest:gunit_headers",
    ],
)

cc_library(
    name = "mock_switch_group",
    hdrs = ["mock_switch_group.h"],
    deps = [
        "//TinyAlpacaServer/src:device_info",
        "//TinyAlpacaServer/src/device_types/switch:switch_adapter",
        "//TinyAlpacaServer/src/utils:status",
        "//TinyAlpacaServer/src/utils:status_or",
        "//googletest:gunit_headers",
    ],
)

cc_library(
    name = "print_value_to_std_string",
    hdrs = ["print_value_to_std_string.h"],
    deps = [
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/src:o_print_stream",
    ],
)

cc_library(
    name = "progmem_string_view_utils",
    hdrs = ["progmem_string_view_utils.h"],
    deps = ["//mcucore/src:progmem_string_view"],
)

cc_library(
    name = "string_io_connection",
    hdrs = ["string_io_connection.h"],
    deps = ["//TinyAlpacaServer/src/utils:connection"],
)

cc_test(
    name = "string_io_connection_test",
    srcs = ["string_io_connection_test.cc"],
    deps = [
        ":string_io_connection",
        "//googletest:gunit_main",
    ],
)

cc_library(
    name = "string_view_utils",
    hdrs = ["string_view_utils.h"],
    deps = ["//TinyAlpacaServer/src/utils:string_view"],
)

cc_library(
    name = "test_tiny_alpaca_server",
    testonly = True,
    srcs = ["test_tiny_alpaca_server.cc"],
    hdrs = ["test_tiny_alpaca_server.h"],
    deps = [
        ":string_io_connection",
        "//TinyAlpacaServer/src:device_interface",
        "//TinyAlpacaServer/src:server_connection",
        "//TinyAlpacaServer/src:server_description",
        "//TinyAlpacaServer/src:tiny_alpaca_server",
        "//TinyAlpacaServer/src/utils:array_view",
        "//absl/strings",
        "//mcucore/extras/host/ethernet5500:ethernet_config",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:o_print_stream",
    ],
)
