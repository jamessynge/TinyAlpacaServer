# TODO(jamessynge): describe this package.

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
    name = "counting_bitbucket_test",
    srcs = ["counting_bitbucket_test.cc"],
    deps = [
        ":counting_bitbucket",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "escaping_test",
    srcs = ["escaping_test.cc"],
    deps = [
        ":escaping",
        ":literal",
        ":print_to_std_string",
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
    name = "json_encoder_test",
    srcs = ["json_encoder_test.cc"],
    deps = [
        ":counting_bitbucket",
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
        ":literal",
        ":print_to_std_string",
        ":string_compare",
        ":string_view",
        "//absl/strings",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "literal_token_test",
    srcs = ["literal_token_test.cc"],
    deps = [
        ":array_view",
        ":literal_token",
        ":platform",
        ":string_view",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "o_print_stream_test",
    srcs = ["o_print_stream_test.cc"],
    deps = [
        ":o_print_stream",
        ":print_to_std_string",
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
