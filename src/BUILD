# Bazel BUILD file for this package.

cc_library(
    name = "TinyAlpacaServer",
    hdrs = ["TinyAlpacaServer.h"],
    deps = [
        ":alpaca_request",
        ":alpaca_response",
        ":any_string",
        ":config",
        ":constants",
        ":counting_bitbucket",
        ":device_api_handler_base",
        ":device_info",
        ":escaping",
        ":extra_parameters",
        ":http_response_header",
        ":json_encoder",
        ":json_response",
        ":literal",
        ":literal_token",
        ":literals",
        ":logging",
        ":match_literals",
        ":platform",
        ":platform_ethernet",
        ":request_decoder",
        ":request_decoder_listener",
        ":server_description",
        ":string_compare",
        ":string_view",
        ":tiny_alpaca_server",
        ":tiny_string",
    ],
)

cc_library(
    name = "alpaca_request",
    srcs = ["alpaca_request.cc"],
    hdrs = ["alpaca_request.h"],
    deps = [
        ":config",
        ":constants",
        ":extra_parameters",
        ":logging",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "alpaca_response",
    srcs = ["alpaca_response.cc"],
    hdrs = ["alpaca_response.h"],
    deps = [
        ":alpaca_request",
        ":any_string",
        ":http_response_header",
        ":json_encoder",
        ":json_response",
        ":platform",
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

cc_library(
    name = "ascom_error_codes",
    srcs = ["ascom_error_codes.cc"],
    hdrs = ["ascom_error_codes.h"],
    deps = [
        ":platform",
        ":status",
    ],
)

# config.h specifies the set of features available (e.g. std::string) on
# the target platform of the Tiny Alpaca Server.
cc_library(
    name = "config",
    hdrs = ["config.h"],
)

cc_library(
    name = "constants",
    srcs = ["constants.cc"],
    hdrs = ["constants.h"],
    deps = [":platform"],
)

cc_library(
    name = "counting_bitbucket",
    hdrs = ["counting_bitbucket.h"],
    deps = [
        ":platform",
    ],
)

cc_library(
    name = "device_api_handler_base",
    srcs = ["device_api_handler_base.cc"],
    hdrs = ["device_api_handler_base.h"],
    deps = [
        ":alpaca_request",
        ":alpaca_response",
        ":constants",
        ":counting_bitbucket",
        ":device_info",
        ":http_response_header",
        ":json_encoder",
        ":literal",
        ":platform",
    ],
)

cc_library(
    name = "device_info",
    srcs = ["device_info.cc"],
    hdrs = ["device_info.h"],
    deps = [
        ":constants",
        ":json_encoder",
        ":literal",
        ":literals",
        ":platform",
        ":string_view",
    ],
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
        ":constants",
        ":platform",
        ":string_view",
        ":tiny_string",
    ],
)

cc_library(
    name = "http_response_header",
    srcs = ["http_response_header.cc"],
    hdrs = ["http_response_header.h"],
    deps = [
        ":any_string",
        ":constants",
        ":literal",
        ":literals",
        ":platform",
    ],
)

cc_library(
    name = "json_encoder",
    srcs = ["json_encoder.cc"],
    hdrs = ["json_encoder.h"],
    deps = [
        ":any_string",
        ":counting_bitbucket",
        ":literals",
        ":platform",
    ],
)

cc_library(
    name = "json_response",
    hdrs = ["json_response.h"],
    deps = [
        ":alpaca_request",
        ":any_string",
        ":json_encoder",
        ":literals",
        ":platform",
        ":string_view",
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
        ":constants",
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
    deps = [
        ":arduino",
        ":pgmspace",
        ":print",
    ],
)

cc_library(
    name = "platform_ethernet",
    hdrs = ["platform_ethernet.h"],
    deps = [
        ":alpaca_request",
        ":config",
        ":constants",
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
        ":constants",
        ":logging",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "server_description",
    srcs = ["server_description.cc"],
    hdrs = ["server_description.h"],
    deps = [
        ":json_encoder",
        ":literal",
        ":literals",
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
    srcs = ["tiny_alpaca_server.cc"],
    hdrs = ["tiny_alpaca_server.h"],
    deps = [":platform"],
)

cc_library(
    name = "tiny_string",
    hdrs = ["tiny_string.h"],
    deps = [
        ":logging",
        ":platform",
    ],
)
