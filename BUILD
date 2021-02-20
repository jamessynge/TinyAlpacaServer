# Bazel BUILD file for this package.

# config.h specifies the set of features available (e.g. std::string) on
# the target platform of the Tiny Alpaca Server.
cc_library(
    name = "config",
    hdrs = ["config.h"],
)

cc_library(
    name = "counting_bitbucket",
    hdrs = ["counting_bitbucket.h"],
    deps = [":host_printable"],
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
    name = "alpaca_request",
    srcs = ["alpaca_request.cc"],
    hdrs = ["alpaca_request.h"],
    deps = [
        ":decoder_constants",
        ":string_view",
    ],
)

cc_library(
    name = "decoder_constants",
    srcs = ["decoder_constants.cc"],
    hdrs = ["decoder_constants.h"],
)

cc_library(
    name = "host_printable",
    hdrs = ["host_printable.h"],
    deps = [":config"],
)

cc_library(
    name = "json_encoder",
    srcs = ["json_encoder.cc"],
    hdrs = ["json_encoder.h"],
    deps = [
        ":host_printable",
        ":string_view",
        "//base:logging",
    ],
)

cc_test(
    name = "json_encoder_test",
    srcs = ["json_encoder_test.cc"],
    deps = [
        ":counting_bitbucket",
        ":host_printable",
        ":json_encoder",
        "//base:logging",
        "//testing/base/public:gunit_main",
        "//absl/strings",
    ],
)

cc_library(
    name = "logging",
    hdrs = ["logging.h"],
)

cc_library(
    name = "request_decoder",
    srcs = ["request_decoder.cc"],
    hdrs = ["request_decoder.h"],
    deps = [
        ":alpaca_request",
        ":config",
        ":decoder_constants",
        ":request_decoder_listener",
        ":string_view",
        ":token",
        ":tokens",
        "//base:logging",
        "//absl/strings",
    ],
)

cc_test(
    name = "request_decoder_test",
    timeout = "short",
    srcs = ["request_decoder_test.cc"],
    deps = [
        ":alpaca_request",
        ":decoder_constants",
        ":request_decoder",
        ":request_decoder_listener",
        ":request_decoder_listener_mock",
        ":string_view",
        "//base:logging",
        "//testing/base/public:gunit_main",
        "//absl/flags:flag",
        "//absl/strings",
    ],
)

cc_library(
    name = "request_decoder_listener",
    srcs = ["request_decoder_listener.cc"],
    hdrs = ["request_decoder_listener.h"],
    deps = [
        ":decoder_constants",
        ":string_view",
        "//base:logging",
    ],
)

cc_library(
    name = "request_decoder_listener_mock",
    hdrs = ["request_decoder_listener_mock.h"],
    deps = [
        ":decoder_constants",
        ":request_decoder_listener",
        ":string_view",
        "//testing/base/public:gunit_headers",
    ],
)

cc_library(
    name = "string_view",
    srcs = ["string_view.cc"],
    hdrs = ["string_view.h"],
    deps = [
        ":config",
        ":host_printable",
        "//base:logging",
        "//absl/strings",
    ],
)

cc_test(
    name = "string_view_test",
    srcs = ["string_view_test.cc"],
    deps = [
        ":config",
        ":string_view",
        "//base:logging",
        "//testing/base/public:gunit_main",
    ],
)

cc_library(
    name = "token",
    hdrs = ["token.h"],
    deps = [
        ":string_view",
        "//absl/strings",
    ],
)

cc_library(
    name = "tokens",
    hdrs = ["tokens.h"],
    deps = [
        ":config",
        ":decoder_constants",
        ":string_view",
        ":token",
    ],
)
