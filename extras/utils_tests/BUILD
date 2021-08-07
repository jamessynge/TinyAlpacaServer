# Tests of Tiny Alpaca Server src/utils/...

cc_test(
    name = "any_printable_test",
    srcs = ["any_printable_test.cc"],
    deps = [
        "//TinyAlpacaServer/src/utils:any_printable",
        "//TinyAlpacaServer/src/utils:literal",
        "//TinyAlpacaServer/src/utils:string_view",
        "//googletest:gunit_main",
        "//mcucore/extras/host/arduino:print",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/src:inline_literal",
    ],
)

cc_test(
    name = "array_test",
    srcs = ["array_test.cc"],
    deps = [
        "//TinyAlpacaServer/src/utils:array",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "array_view_test",
    srcs = ["array_view_test.cc"],
    deps = [
        "//TinyAlpacaServer/src/utils:array_view",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "avr_timer_counter_test",
    srcs = ["avr_timer_counter_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:print_value_to_std_string",
        "//TinyAlpacaServer/src/utils:avr_timer_counter",
        "//absl/time",
        "//glog:logging",
        "//googletest:gunit_main",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/test_tools:print_to_std_string",
    ],
)

cc_test(
    name = "counting_print_test",
    srcs = ["counting_print_test.cc"],
    deps = [
        "//TinyAlpacaServer/src/utils:counting_print",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "hex_escape_test",
    srcs = ["hex_escape_test.cc"],
    deps = [
        "//TinyAlpacaServer/src/utils:hex_escape",
        "//TinyAlpacaServer/src/utils:literal",
        "//TinyAlpacaServer/src/utils:string_view",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/extras/test_tools:sample_printable",
        "//util/gtl:map_util",
    ],
)

cc_test(
    name = "is_integral_test",
    srcs = ["is_integral_test.cc"],
    deps = [
        "//googletest:gunit_main",
        "//mcucore/src:type_traits",
    ],
)

cc_test(
    name = "json_encoder_test",
    srcs = ["json_encoder_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:json_test_utils",
        "//TinyAlpacaServer/src/utils:json_encoder",
        "//TinyAlpacaServer/src/utils:literal",
        "//TinyAlpacaServer/src/utils:string_view",
        "//absl/strings",
        "//glog:logging",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/extras/test_tools:sample_printable",
    ],
)

cc_test(
    name = "literal_test",
    srcs = ["literal_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:literal_utils",
        "//TinyAlpacaServer/extras/test_tools:string_view_utils",
        "//TinyAlpacaServer/src/utils:hex_escape",
        "//TinyAlpacaServer/src/utils:literal",
        "//TinyAlpacaServer/src/utils:string_compare",
        "//TinyAlpacaServer/src/utils:string_view",
        "//absl/strings",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
    ],
)

cc_test(
    name = "moving_average_test",
    srcs = ["moving_average_test.cc"],
    deps = [
        "//TinyAlpacaServer/src/utils:moving_average",
        "//absl/random",
        "//absl/random:distributions",
        "//absl/time",
        "//glog:logging",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "printable_cat_test",
    srcs = ["printable_cat_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:print_value_to_std_string",
        "//TinyAlpacaServer/src/utils:any_printable",
        "//TinyAlpacaServer/src/utils:literal",
        "//TinyAlpacaServer/src/utils:printable_cat",
        "//TinyAlpacaServer/src/utils:string_view",
        "//absl/strings",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "server_socket_test",
    srcs = ["server_socket_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:mock_socket_listener",
        "//TinyAlpacaServer/src/utils:server_socket",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "status_or_test",
    srcs = ["status_or_test.cc"],
    deps = [
        "//TinyAlpacaServer/src/utils:literal",
        "//TinyAlpacaServer/src/utils:status",
        "//TinyAlpacaServer/src/utils:status_or",
        "//TinyAlpacaServer/src/utils:string_view",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "status_test",
    srcs = ["status_test.cc"],
    deps = [
        "//TinyAlpacaServer/src/utils:literal",
        "//TinyAlpacaServer/src/utils:status",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "stream_to_print_test",
    srcs = ["stream_to_print_test.cc"],
    deps = [
        "//TinyAlpacaServer/src/utils:stream_to_print",
        "//googletest:gunit_main",
        "//mcucore/extras/host/arduino:print",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/extras/test_tools:sample_printable",
        "//mcucore/src:o_print_stream",
    ],
)

cc_test(
    name = "string_view_test",
    srcs = ["string_view_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:string_view_utils",
        "//TinyAlpacaServer/src/utils:string_view",
        "//glog:logging",
        "//googletest:gunit_main",
    ],
)
