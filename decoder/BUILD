# HTTP Request Decoder targeted at the ASCOM Alpaca REST API.

cc_library(
    name = "constants",
    srcs = ["constants.cc"],
    hdrs = ["constants.h"],
)

cc_library(
    name = "request",
    srcs = ["request.cc"],
    hdrs = ["request.h"],
    deps = [
        ":constants",
        "//tiny-alpaca-server/common:string_view",
    ],
)

cc_library(
    name = "request_decoder",
    srcs = ["request_decoder.cc"],
    hdrs = ["request_decoder.h"],
    deps = [
        ":constants",
        ":request",
        ":request_decoder_listener",
        ":token",
        ":tokens",
        "//base:logging",
        "//tiny-alpaca-server:config",
        "//tiny-alpaca-server/common:string_view",
        "//absl/strings",
    ],
)

cc_test(
    name = "request_decoder_test",
    timeout = "short",
    srcs = ["request_decoder_test.cc"],
    deps = [
        ":constants",
        ":request",
        ":request_decoder",
        ":request_decoder_listener",
        ":request_decoder_listener_mock",
        "//base:logging",
        "//tiny-alpaca-server/common:string_view",
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
        ":constants",
        "//base:logging",
        "//tiny-alpaca-server/common:string_view",
    ],
)

cc_library(
    name = "request_decoder_listener_mock",
    hdrs = ["request_decoder_listener_mock.h"],
    deps = [
        ":constants",
        ":request_decoder_listener",
        "//tiny-alpaca-server/common:string_view",
        "//testing/base/public:gunit_headers",
    ],
)

cc_library(
    name = "token",
    hdrs = ["token.h"],
    deps = [
        "//tiny-alpaca-server/common:string_view",
        "//absl/strings",
    ],
)

cc_library(
    name = "tokens",
    hdrs = ["tokens.h"],
    deps = [
        ":constants",
        ":token",
        "//tiny-alpaca-server:config",
        "//tiny-alpaca-server/common:string_view",
    ],
)
