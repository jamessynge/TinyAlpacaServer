# Libraries intended to simplify the task of writing tests of Tiny Alpaca Server.

cc_library(
    name = "json_test_utils",
    srcs = ["json_test_utils.cc"],
    hdrs = ["json_test_utils.h"],
    deps = [
        ":any_printable",
        ":json_encoder",
        ":platform",
    ],
)

cc_library(
    name = "mock_ethernet_client",
    hdrs = ["mock_ethernet_client.h"],
    deps = [
        ":ethernet_client",
        "//googletest:gunit_headers",
    ],
)

cc_library(
    name = "mock_request_listener",
    hdrs = ["mock_request_listener.h"],
    deps = [
        ":request_listener",
        "//googletest:gunit_headers",
    ],
)

cc_library(
    name = "print_to_std_string",
    hdrs = ["print_to_std_string.h"],
    deps = [
        ":o_print_stream",
        ":print",
    ],
)

cc_library(
    name = "sample_printable",
    hdrs = ["sample_printable.h"],
    deps = [":print"],
)
