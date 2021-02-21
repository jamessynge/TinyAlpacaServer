# Bazel BUILD file for this package.

cc_library(
    name = "alpaca_request",
    srcs = ["alpaca_request.cc"],
    hdrs = ["alpaca_request.h"],
    deps = [
        ":config",
        ":decoder_constants",
        ":extra_parameters",
        ":platform",
        ":string_view",
    ],
)

# config.h specifies the set of features available (e.g. std::string) on
# the target platform of the Tiny Alpaca Server.
cc_library(
    name = "config",
    hdrs = ["config.h"],
)

cc_library(
    name = "counting_bitbucket",
    hdrs = ["counting_bitbucket.h"],
    deps = [
        ":platform",
    ],
)

cc_library(
    name = "decoder_constants",
    srcs = ["decoder_constants.cc"],
    hdrs = ["decoder_constants.h"],
    deps = [":platform"],
)

cc_library(
    name = "extra_parameters",
    hdrs = ["extra_parameters.h"],
    deps = [
        ":config",
        ":decoder_constants",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "json_encoder",
    srcs = ["json_encoder.cc"],
    hdrs = ["json_encoder.h"],
    deps = [
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "logging",
    hdrs = ["logging.h"],
    deps = ["//core:logging"],
)

cc_library(
    name = "platform",
    hdrs = ["platform.h"],
    deps = [
        ":arduino",
        "//base",
    ],
)

cc_library(
    name = "request_decoder",
    srcs = ["request_decoder.cc"],
    hdrs = ["request_decoder.h"],
    deps = [
        ":alpaca_request",
        ":config",
        ":decoder_constants",
        ":platform",
        ":request_decoder_listener",
        ":string_view",
        ":token",
        ":tokens",
    ],
)

cc_library(
    name = "request_decoder_listener",
    srcs = ["request_decoder_listener.cc"],
    hdrs = ["request_decoder_listener.h"],
    deps = [
        ":decoder_constants",
        ":logging",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "string_view",
    srcs = ["string_view.cc"],
    hdrs = ["string_view.h"],
    deps = [
        ":logging",
        ":platform",
        "//absl/strings",
    ],
)

cc_library(
    name = "token",
    hdrs = ["token.h"],
    deps = [
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "tokens",
    hdrs = ["tokens.h"],
    deps = [
        ":config",
        ":decoder_constants",
        ":platform",
        ":string_view",
        ":token",
    ],
)
