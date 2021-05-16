# Contains tests of code that is in the src directory, to be executed on host.
# We don't put the tests there because otherwise the Arduino IDE tries to treat
# them as code to be compiled for the target microcontroller.

cc_test(
    name = "alpaca_devices_test",
    srcs = ["alpaca_devices_test.cc"],
    deps = [
        "//absl/strings",
        "//extras/test_tools:mock_device_interface",
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src:alpaca_devices",
        "//src:ascom_error_codes",
        "//src:constants",
        "//src:device_interface",
        "//src/utils:json_encoder_helpers",
    ],
)

cc_test(
    name = "alpaca_response_test",
    srcs = ["alpaca_response_test.cc"],
    deps = [
        "//absl/strings",
        "//extras/test_tools:json_test_utils",
        "//extras/test_tools:print_to_std_string",
        "//extras/test_tools:sample_printable",
        "//googletest:gunit_main",
        "//src:alpaca_response",
        "//src:ascom_error_codes",
        "//src:constants",
        "//src:literals",
        "//src/utils:any_printable",
        "//src/utils:array_view",
        "//src/utils:literal",
    ],
)

cc_test(
    name = "config_test",
    srcs = ["config_test.cc"],
    deps = [
        "//googletest:gunit_main",
        "//src:config",
        "//src/utils:platform",
    ],
)

cc_test(
    name = "device_info_test",
    srcs = ["device_info_test.cc"],
    deps = [
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src:device_info",
    ],
)

cc_test(
    name = "http_response_header_test",
    srcs = ["http_response_header_test.cc"],
    deps = [
        "//absl/strings",
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src:constants",
        "//src:http_response_header",
        "//src:literals",
    ],
)

cc_test(
    name = "json_response_test",
    srcs = ["json_response_test.cc"],
    deps = [
        "//extras/test_tools:json_test_utils",
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src:json_response",
        "//src/utils:any_printable",
    ],
)

cc_test(
    name = "literals_test",
    srcs = ["literals_test.cc"],
    deps = [
        "//extras/test_tools:string_view_utils",
        "//googletest:gunit_main",
        "//src:literals",
        "//src/utils:string_compare",
        "//src/utils:string_view",
    ],
)

cc_test(
    name = "match_literals_test",
    srcs = ["match_literals_test.cc"],
    deps = [
        "//absl/strings",
        "//extras/test_tools:print_to_std_string",
        "//extras/test_tools:string_view_utils",
        "//googletest:gunit_main",
        "//src:constants",
        "//src:match_literals",
        "//src/utils:logging",
    ],
)

cc_library(
    name = "request_decoder_listener_mock",
    hdrs = ["request_decoder_listener_mock.h"],
    deps = [
        "//googletest:gunit_headers",
        "//src:request_decoder_listener",
    ],
)

cc_test(
    name = "request_decoder_test",
    timeout = "short",
    srcs = ["request_decoder_test.cc"],
    deps = [
        ":request_decoder_listener_mock",
        "//absl/flags:flag",
        "//absl/strings",
        "//core:logging",
        "//googletest:gunit_main",
        "//src:alpaca_request",
        "//src:constants",
        "//src:request_decoder",
        "//src:request_decoder_listener",
        "//src/utils:string_view",
    ],
)

cc_test(
    name = "server_description_test",
    srcs = ["server_description_test.cc"],
    deps = [
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src:server_description",
        "//src/utils:json_encoder_helpers",
    ],
)
