# Support libraries for the Tiny Alpaca Server.

cc_library(
    name = "host_printable",
    hdrs = ["host_printable.h"],
    deps = ["//tiny-alpaca-server:config"],
)

cc_library(
    name = "string_view",
    srcs = ["string_view.cc"],
    hdrs = ["string_view.h"],
    deps = [
        ":host_printable",
        "//base:logging",
        "//tiny-alpaca-server:config",
        "//absl/strings",
    ],
)

cc_library(
    name = "logging",
    hdrs = ["logging.h"],
)

cc_test(
    name = "string_view_test",
    srcs = ["string_view_test.cc"],
    deps = [
        ":string_view",
        "//base:logging",
        "//tiny-alpaca-server:config",
        "//testing/base/public:gunit_main",
    ],
)
