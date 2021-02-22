# TODO(jamessynge): describe this package.

cc_test(
    name = "counting_bitbucket_test",
    srcs = ["counting_bitbucket_test.cc"],
    deps = [
        "//experimental/users/jamessynge/tiny-alpaca-server:counting_bitbucket",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "json_encoder_test",
    srcs = ["json_encoder_test.cc"],
    deps = [
        ":test_utils",
        "//absl/strings",
        "//core:logging",
        "//experimental/users/jamessynge/tiny-alpaca-server:counting_bitbucket",
        "//experimental/users/jamessynge/tiny-alpaca-server:json_encoder",
        "//googletest:gunit_main",
    ],
)

cc_library(
    name = "request_decoder_listener_mock",
    hdrs = ["request_decoder_listener_mock.h"],
    deps = [
        "//experimental/users/jamessynge/tiny-alpaca-server:request_decoder_listener",
        "//googletest:gunit_headers",
    ],
)

cc_test(
    name = "request_decoder_test",
    timeout = "short",
    srcs = ["request_decoder_test.cc"],
    deps = [
        ":request_decoder_listener_mock",
        "//absl/flags:flag",
        "//absl/strings",
        "//core:logging",
        "//experimental/users/jamessynge/tiny-alpaca-server:alpaca_request",
        "//experimental/users/jamessynge/tiny-alpaca-server:decoder_constants",
        "//experimental/users/jamessynge/tiny-alpaca-server:request_decoder",
        "//experimental/users/jamessynge/tiny-alpaca-server:request_decoder_listener",
        "//experimental/users/jamessynge/tiny-alpaca-server:string_view",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "string_view_test",
    srcs = ["string_view_test.cc"],
    deps = [
        "//core:logging",
        "//experimental/users/jamessynge/tiny-alpaca-server:config",
        "//experimental/users/jamessynge/tiny-alpaca-server:string_view",
        "//googletest:gunit_main",
    ],
)

cc_test(
    name = "config_test",
    srcs = ["config_test.cc"],
    deps = [
        "//experimental/users/jamessynge/tiny-alpaca-server:config",
        "//experimental/users/jamessynge/tiny-alpaca-server:platform",
        "//googletest:gunit_main",
    ],
)

cc_library(
    name = "test_utils",
    hdrs = ["test_utils.h"],
    deps = [":print"],
)
