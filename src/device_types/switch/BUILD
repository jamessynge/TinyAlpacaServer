# TODO(jamessynge): describe this package.

cc_library(
    name = "multi_switch_adapter",
    srcs = ["multi_switch_adapter.cc"],
    hdrs = ["multi_switch_adapter.h"],
    deps = [
        ":switch_adapter",
        ":switch_interface",
        "//src:alpaca_response",
        "//src:ascom_error_codes",
        "//src:constants",
        "//src:device_info",
        "//src:literals",
        "//src/utils:array_view",
        "//src/utils:platform",
        "//src/utils:status_or",
    ],
)

cc_library(
    name = "switch_adapter",
    srcs = ["switch_adapter.cc"],
    hdrs = ["switch_adapter.h"],
    deps = [
        "//src:alpaca_response",
        "//src:ascom_error_codes",
        "//src:constants",
        "//src:literals",
        "//src/device_types:device_impl_base",
        "//src/utils:platform",
        "//src/utils:status_or",
    ],
)

cc_library(
    name = "switch_interface",
    hdrs = ["switch_interface.h"],
    deps = [
        "//src:alpaca_request",
        "//src/utils:platform",
        "//src/utils:status",
        "//src/utils:status_or",
    ],
)

cc_library(
    name = "toggle_switch_base",
    srcs = ["toggle_switch_base.cc"],
    hdrs = ["toggle_switch_base.h"],
    deps = [
        ":switch_interface",
        "//src/utils:platform",
    ],
)
