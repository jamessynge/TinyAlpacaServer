# Contains tests of code that is in the src directory, to be executed on host.
# We don't put the tests there because otherwise the Arduino IDE tries to treat
# them as code to be compiled for the target microcontroller.

cc_test(
    name = "alpaca_response_test",
    srcs = ["alpaca_response_test.cc"],
    deps = [
        ":alpaca_response",
        ":json_test_utils",
        ":literal",
        ":literals",
        ":print_to_std_string",
        "//absl/strings",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "config_test",
    srcs = ["config_test.cc"],
    deps = [
        ":config",
        ":platform",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "device_info_test",
    srcs = ["device_info_test.cc"],
    deps = [
        ":device_info",
        ":print_to_std_string",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "http_response_header_test",
    srcs = ["http_response_header_test.cc"],
    deps = [
        ":constants",
        ":http_response_header",
        ":literals",
        ":print_to_std_string",
        "//absl/strings",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "json_response_test",
    srcs = ["json_response_test.cc"],
    deps = [
        ":json_response",
        ":json_test_utils",
        ":print_to_std_string",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "literals_test",
    srcs = ["literals_test.cc"],
    deps = [
        ":literals",
        ":string_compare",
        ":string_view",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "match_literals_test",
    srcs = ["match_literals_test.cc"],
    deps = [
        ":constants",
        ":match_literals",
        ":print_to_std_string",
        "//absl/strings",
        "//googletest:gunit_main",
    ],
)

cc_library(
    name = "request_decoder_listener_mock",
    hdrs = ["request_decoder_listener_mock.h"],
    deps = [
        ":request_decoder_listener",
        "//googletest:gunit_headers",
    ],
)

cc_test(
    name = "request_decoder_test",
    timeout = "short",
    srcs = ["request_decoder_test.cc"],
    deps = [
        ":alpaca_request",
        ":constants",
        ":request_decoder",
        ":request_decoder_listener",
        ":request_decoder_listener_mock",
        ":string_view",
        "//absl/flags:flag",
        "//absl/strings",
        "//core:logging",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "server_description_test",
    srcs = ["server_description_test.cc"],
    deps = [
        ":print_to_std_string",
        ":server_description",
        "//googletest:gunit_main",
    ],
)
