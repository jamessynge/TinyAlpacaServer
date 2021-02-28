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

cc_library(
    name = "any_string",
    srcs = ["any_string.cc"],
    hdrs = ["any_string.h"],
    deps = [
        ":literal",
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
    name = "escaping",
    srcs = ["escaping.cc"],
    hdrs = ["escaping.h"],
    deps = [
        ":logging",
        ":platform",
    ],
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
        ":any_string",
        ":literals",
        ":platform",
        ":print",
    ],
)

cc_library(
    name = "json_response",
    hdrs = ["json_response.h"],
    deps = [
        ":alpaca_request",
        ":json_encoder",
        ":literals",
        ":platform",
    ],
)

cc_library(
    name = "literal",
    srcs = ["literal.cc"],
    hdrs = ["literal.h"],
    deps = [
        ":escaping",
        ":logging",
        ":platform",
    ],
)

cc_library(
    name = "literal_token",
    hdrs = ["literal_token.h"],
    deps = [
        ":literal",
        ":platform",
        ":string_compare",
        ":string_view",
    ],
)

cc_library(
    name = "literals",
    hdrs = ["literals.h"],
    textual_hdrs = ["literals.inc"],
    deps = [
        ":literal",
        ":platform",
    ],
)

cc_library(
    name = "logging",
    hdrs = ["logging.h"],
    deps = ["//core:logging"],
)

cc_library(
    name = "match_literals",
    srcs = ["match_literals.cc"],
    hdrs = ["match_literals.h"],
    deps = [
        ":decoder_constants",
        ":literal",
        ":literal_token",
        ":literals",
        ":logging",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "platform",
    hdrs = ["platform.h"],
    deps = [":arduino"],
)

cc_library(
    name = "request_decoder",
    srcs = ["request_decoder.cc"],
    hdrs = ["request_decoder.h"],
    deps = [
        ":alpaca_request",
        ":config",
        ":decoder_constants",
        ":literals",
        ":logging",
        ":match_literals",
        ":platform",
        ":request_decoder_listener",
        ":string_compare",
        ":string_view",
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
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [":platform"],
)

cc_library(
    name = "server_description",
    hdrs = ["server_description.h"],
    deps = [
        ":json_encoder",
        ":literal",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "string_compare",
    srcs = ["string_compare.cc"],
    hdrs = ["string_compare.h"],
    deps = [
        ":literal",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "string_view",
    srcs = [
        "string_view.cc",
    ],
    hdrs = [
        "string_view.h",
    ],
    deps = [
        ":escaping",
        ":logging",
        ":platform",
        "//absl/strings",
    ],
)

cc_library(
    name = "tiny_alpaca_server",
    hdrs = ["TinyAlpacaServer.h"],
    deps = [
        ":alpaca_request",
        ":config",
        ":counting_bitbucket",
        ":decoder_constants",
        ":escaping",
        ":extra_parameters",
        ":json_encoder",
        ":json_response",
        ":literal",
        ":literal_token",
        ":literals",
        ":logging",
        ":match_literals",
        ":platform",
        ":request_decoder",
        ":request_decoder_listener",
        ":server",
        ":server_description",
        ":string_view",
    ],
)
