# TODO(jamessynge): describe this package.

cc_test(
    name = "array_test",
    srcs = ["array_test.cc"],
    deps = [
        ":array",
        ":print_to_std_string",
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
    name = "json_encoder_test",
    srcs = ["json_encoder_test.cc"],
    deps = [
        ":counting_bitbucket",
        ":json_encoder",
        ":json_test_utils",
        ":print_to_std_string",
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
        ":literal_token",
        ":platform",
        ":print_to_std_string",
        ":string_view",
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
