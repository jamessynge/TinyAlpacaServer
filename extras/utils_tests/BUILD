# Tests of Tiny Alpaca Server src/utils/...

cc_test(
    name = "any_printable_test",
    srcs = ["any_printable_test.cc"],
    deps = [
        ":any_printable",
        ":print_to_std_string",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "array_test",
    srcs = ["array_test.cc"],
    deps = [
        ":array",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "array_view_test",
    srcs = ["array_view_test.cc"],
    deps = [
        ":array_view",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "counting_print_test",
    srcs = ["counting_print_test.cc"],
    deps = [
        ":counting_print",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "hex_escape_test",
    srcs = ["hex_escape_test.cc"],
    deps = [
        ":hex_escape",
        ":literal",
        ":print_to_std_string",
        ":sample_printable",
        ":string_view",
        "//googletest:gunit_main",
        "//util/gtl:map_util",
    ],
)

cc_test(
    name = "inline_literal_test",
    srcs = ["inline_literal_test.cc"],
    deps = [
        ":inline_literal",
        ":print_to_std_string",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "is_integral_test",
    srcs = ["is_integral_test.cc"],
    deps = [
        ":print_to_std_string",
        ":type_traits",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "json_encoder_test",
    srcs = ["json_encoder_test.cc"],
    deps = [
        ":counting_print",
        ":json_encoder",
        ":json_test_utils",
        ":print_to_std_string",
        ":sample_printable",
        "//absl/strings",
        "//core:logging",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "literal_test",
    srcs = ["literal_test.cc"],
    deps = [
        ":hex_escape",
        ":literal",
        ":print_to_std_string",
        ":string_compare",
        ":string_view",
        "//absl/strings",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "log_sink_test",
    srcs = ["log_sink_test.cc"],
    deps = [
        ":log_sink",
        ":print_to_std_string",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "moving_average_test",
    srcs = ["moving_average_test.cc"],
    deps = [
        ":moving_average",
        ":print_to_std_string",
        "//absl/random",
        "//absl/random:distributions",
        "//absl/time",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "o_print_stream_test",
    srcs = ["o_print_stream_test.cc"],
    deps = [
        ":o_print_stream",
        ":print_to_std_string",
        ":sample_printable",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "printable_cat_test",
    srcs = ["printable_cat_test.cc"],
    deps = [
        ":any_printable",
        ":literal",
        ":print_to_std_string",
        ":printable_cat",
        ":string_view",
        "//absl/strings",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "status_or_test",
    srcs = ["status_or_test.cc"],
    deps = [
        ":status_or",
        ":string_view",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "status_test",
    srcs = ["status_test.cc"],
    deps = [
        ":status",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "string_view_test",
    srcs = ["string_view_test.cc"],
    deps = [
        ":config",
        ":string_view",
        "//core:logging",
        "//googletest:gunit_main",
    ],
)
