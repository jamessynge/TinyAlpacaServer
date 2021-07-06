# Libraries intended to simplify the task of writing tests of Tiny Alpaca Server.

cc_library(
    name = "fake_platform_ethernet",
    hdrs = ["fake_platform_ethernet.h"],
    deps = ["//src/utils:platform_ethernet"],
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
    name = "string_view_utils",
    hdrs = ["string_view_utils.h"],
    deps = ["//src/utils:string_view"],
)
