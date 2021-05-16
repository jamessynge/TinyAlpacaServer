# Tests of Tiny Alpaca Server src/utils/...

cc_test(
    name = "any_printable_test",
    srcs = ["any_printable_test.cc"],
    deps = [
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src/utils:any_printable",
    ],
)

cc_test(
    name = "array_test",
    srcs = ["array_test.cc"],
    deps = [
        "//googletest:gunit_main",
        "//src/utils:array",
    ],
)

cc_test(
    name = "array_view_test",
    srcs = ["array_view_test.cc"],
    deps = [
        "//googletest:gunit_main",
        "//src/utils:array_view",
    ],
)

cc_test(
    name = "avr_timer_counter_test",
    srcs = ["avr_timer_counter_test.cc"],
    deps = [
        "//absl/time",
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src/utils:avr_timer_counter",
        "//src/utils:o_print_stream",
    ],
)

cc_test(
    name = "counting_print_test",
    srcs = ["counting_print_test.cc"],
    deps = [
        "//googletest:gunit_main",
        "//src/utils:counting_print",
    ],
)

cc_test(
    name = "hex_escape_test",
    srcs = ["hex_escape_test.cc"],
    deps = [
        "//extras/test_tools:print_to_std_string",
        "//extras/test_tools:sample_printable",
        "//googletest:gunit_main",
        "//src/utils:hex_escape",
        "//src/utils:literal",
        "//src/utils:string_view",
        "//util/gtl:map_util",
    ],
)

cc_test(
    name = "inline_literal_test",
    srcs = ["inline_literal_test.cc"],
    deps = [
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src/utils:inline_literal",
    ],
)

cc_test(
    name = "is_integral_test",
    srcs = ["is_integral_test.cc"],
    deps = [
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src/utils/traits:type_traits",
    ],
)

cc_test(
    name = "json_encoder_test",
    srcs = ["json_encoder_test.cc"],
    deps = [
        "//absl/strings",
        "//core:logging",
        "//extras/test_tools:json_test_utils",
        "//extras/test_tools:print_to_std_string",
        "//extras/test_tools:sample_printable",
        "//googletest:gunit_main",
        "//src/utils:counting_print",
        "//src/utils:json_encoder",
    ],
)

cc_test(
    name = "literal_test",
    srcs = ["literal_test.cc"],
    deps = [
        "//absl/strings",
        "//extras/test_tools:literal_utils",
        "//extras/test_tools:print_to_std_string",
        "//extras/test_tools:string_view_utils",
        "//googletest:gunit_main",
        "//src/utils:hex_escape",
        "//src/utils:literal",
        "//src/utils:string_compare",
        "//src/utils:string_view",
    ],
)

cc_test(
    name = "log_sink_test",
    srcs = ["log_sink_test.cc"],
    deps = [
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src/utils:log_sink",
    ],
)

cc_test(
    name = "moving_average_test",
    srcs = ["moving_average_test.cc"],
    deps = [
        "//absl/random",
        "//absl/random:distributions",
        "//absl/time",
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src/utils:moving_average",
    ],
)

cc_test(
    name = "o_print_stream_test",
    srcs = ["o_print_stream_test.cc"],
    deps = [
        "//extras/test_tools:print_to_std_string",
        "//extras/test_tools:sample_printable",
        "//googletest:gunit_main",
        "//src/utils:o_print_stream",
    ],
)

cc_test(
    name = "printable_cat_test",
    srcs = ["printable_cat_test.cc"],
    deps = [
        "//absl/strings",
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src/utils:any_printable",
        "//src/utils:literal",
        "//src/utils:printable_cat",
        "//src/utils:string_view",
    ],
)

cc_test(
    name = "status_or_test",
    srcs = ["status_or_test.cc"],
    deps = [
        "//googletest:gunit_main",
        "//src/utils:status_or",
        "//src/utils:string_view",
    ],
)

cc_test(
    name = "status_test",
    srcs = ["status_test.cc"],
    deps = [
        "//googletest:gunit_main",
        "//src/utils:status",
    ],
)

cc_test(
    name = "stream_to_print_test",
    srcs = ["stream_to_print_test.cc"],
    deps = [
        "//extras/test_tools:print_to_std_string",
        "//extras/test_tools:sample_printable",
        "//googletest:gunit_main",
        "//src/utils:stream_to_print",
    ],
)

cc_test(
    name = "string_view_test",
    srcs = ["string_view_test.cc"],
    deps = [
        "//absl/strings",
        "//core:logging",
        "//extras/test_tools:string_view_utils",
        "//googletest:gunit_main",
        "//src:config",
        "//src/utils:string_view",
    ],
)
