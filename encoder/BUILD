# A library for generating HTTP reponse messages for the ASCOM Alpaca REST API,
# primarily focused (so far) on streaming production of the required JSON body.

cc_library(
    name = "counting_bitbucket",
    hdrs = ["counting_bitbucket.h"],
    deps = ["//tiny-alpaca-server/common:host_printable"],
)

cc_test(
    name = "counting_bitbucket_test",
    srcs = ["counting_bitbucket_test.cc"],
    deps = [
        ":counting_bitbucket",
        "//testing/base/public:gunit_main",
    ],
)

cc_library(
    name = "json_encoder",
    srcs = ["json_encoder.cc"],
    hdrs = ["json_encoder.h"],
    deps = [
        "//base:logging",
        "//tiny-alpaca-server/common:host_printable",
        "//tiny-alpaca-server/common:string_view",
    ],
)

cc_test(
    name = "json_encoder_test",
    srcs = ["json_encoder_test.cc"],
    deps = [
        ":counting_bitbucket",
        ":json_encoder",
        "//base:logging",
        "//tiny-alpaca-server/common:host_printable",
        "//testing/base/public:gunit_main",
        "//absl/strings",
    ],
)
