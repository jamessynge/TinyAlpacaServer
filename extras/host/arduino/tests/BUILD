# Tests of host_arduino targets.

cc_test(
    name = "pgmspace_test",
    srcs = ["pgmspace_test.cc"],
    deps = [
        "//absl/strings",
        "//extras/host/arduino:pgmspace",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "print_test",
    srcs = ["print_test.cc"],
    deps = [
        "//absl/strings",
        "//extras/host/arduino:print",
        "//extras/test_tools:print_to_std_string",
        "//extras/test_tools:sample_printable",
        "//googletest:gunit_main",
    ],
)
