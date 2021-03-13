# Tests of host_arduino targets.

cc_test(
    name = "pgmspace_test",
    srcs = ["pgmspace_test.cc"],
    deps = [
        ":pgmspace",
        "//absl/strings",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "print_test",
    srcs = ["print_test.cc"],
    deps = [
        ":print",
        ":print_to_std_string",
        ":sample_printable",
        "//googletest:gunit_main",
    ],
)
