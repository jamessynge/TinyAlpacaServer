# Libraries intended to simplify the task of writing tests of Tiny Alpaca Server.

cc_library(
    name = "fake_platform_ethernet",
    hdrs = ["fake_platform_ethernet.h"],
    deps = ["//src/utils:platform_ethernet"],
)

cc_library(
    name = "http_response",
    srcs = ["http_response.cc"],
    hdrs = ["http_response.h"],
    deps = [
        "//absl/status:statusor",
        "//absl/strings",
    ],
)

cc_test(
    name = "http_response_test",
    srcs = ["http_response_test.cc"],
    deps = [
        ":http_response",
        "//googletest:gunit_main",
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
        "//core:logging",
        "//googletest:gunit_headers",
        "//util/task:status",
    ],
)

cc_test(
    name = "json_decoder_test",
    srcs = ["json_decoder_test.cc"],
    deps = [
        ":json_decoder",
        "//googletest:gunit_main",
    ],
)

cc_library(
    name = "json_test_utils",
    srcs = ["json_test_utils.cc"],
    hdrs = ["json_test_utils.h"],
    deps = [
        "//src/utils:any_printable",
        "//src/utils:json_encoder",
        "//src/utils:platform",
    ],
)

cc_library(
    name = "literal_utils",
    hdrs = ["literal_utils.h"],
    deps = ["//src/utils:literal"],
)

cc_library(
    name = "mock_device_interface",
    hdrs = ["mock_device_interface.h"],
    deps = [
        "//googletest:gunit_headers",
        "//src:device_interface",
    ],
)

cc_library(
    name = "mock_ethernet_client",
    hdrs = ["mock_ethernet_client.h"],
    deps = [
        "//extras/host/ethernet3:ethernet_client",
        "//googletest:gunit_headers",
    ],
)

cc_library(
    name = "mock_request_decoder_listener",
    hdrs = ["mock_request_decoder_listener.h"],
    deps = [
        "//googletest:gunit_headers",
        "//src:request_decoder_listener",
    ],
)

cc_library(
    name = "mock_request_listener",
    hdrs = ["mock_request_listener.h"],
    deps = [
        "//googletest:gunit_headers",
        "//src:request_listener",
    ],
)

cc_library(
    name = "mock_socket_listener",
    hdrs = ["mock_socket_listener.h"],
    deps = [
        "//googletest:gunit_headers",
        "//src/utils:socket_listener",
    ],
)

cc_library(
    name = "mock_switch_group",
    hdrs = ["mock_switch_group.h"],
    deps = [
        ":mock_device_interface",
        "//googletest:gunit_headers",
        "//src/device_types/switch:switch_adapter",
    ],
)

cc_library(
    name = "print_to_std_string",
    hdrs = ["print_to_std_string.h"],
    deps = [
        "//extras/host/arduino:print",
        "//src/utils:o_print_stream",
    ],
)

cc_library(
    name = "sample_printable",
    hdrs = ["sample_printable.h"],
    deps = ["//extras/host/arduino:print"],
)

cc_library(
    name = "string_io_connection",
    hdrs = ["string_io_connection.h"],
    deps = ["//src/utils:connection"],
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
    deps = ["//src/utils:string_view"],
)

cc_library(
    name = "test_tiny_alpaca_server",
    hdrs = ["test_tiny_alpaca_server.h"],
    deps = [
        ":mock_device_interface",
        ":mock_switch_group",
        ":print_to_std_string",
        "//absl/strings",
        "//googletest:gunit_headers",
        "//src:alpaca_devices",
        "//src:alpaca_discovery_server",
        "//src:alpaca_response",
        "//src:constants",
        "//src:literals",
        "//src:request_listener",
        "//src:server_connection",
        "//src:server_description",
        "//src:server_sockets_and_connections",
        "//src:tiny_alpaca_server",
        "//src/device_types:device_impl_base",
        "//src/utils:array_view",
        "//src/utils:platform",
        "//src/utils:platform_ethernet",
        "//src/utils:status",
        "//src/utils:string_view",
    ],
)

cc_library(
    name = "test_tiny_alpaca_server_cc",
    testonly = True,
    srcs = ["test_tiny_alpaca_server.cc"],
    hdrs = ["test_tiny_alpaca_server.h"],
    deps = [
        ":mock_device_interface",
        ":mock_switch_group",
        ":print_to_std_string",
        ":string_io_connection",
        "//absl/strings",
        "//googletest:gunit_headers",
        "//src:alpaca_devices",
        "//src:alpaca_discovery_server",
        "//src:alpaca_response",
        "//src:constants",
        "//src:literals",
        "//src:request_listener",
        "//src:server_connection",
        "//src:server_description",
        "//src:server_sockets_and_connections",
        "//src:tiny_alpaca_server",
        "//src/device_types:device_impl_base",
        "//src/utils:array_view",
        "//src/utils:logging",
        "//src/utils:platform",
        "//src/utils:platform_ethernet",
        "//src/utils:status",
        "//src/utils:string_view",
    ],
)
