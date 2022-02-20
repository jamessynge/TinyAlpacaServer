# Defines base classes for implementing ASCOM Alpaca Switch devices.

load(
    "//TinyAlpacaServer/extras/dev_tools:build_config.bzl",
    "build_config",
)

package(
    default_copts = build_config.default_copts,
)

cc_library(
    name = "multi_switch_adapter",
    srcs = ["multi_switch_adapter.cc"],
    hdrs = ["multi_switch_adapter.h"],
    deps = [
        ":switch_adapter",
        ":switch_interface",
        "//TinyAlpacaServer/src:alpaca_response",
        "//TinyAlpacaServer/src:ascom_error_codes",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:device_info",
        "//TinyAlpacaServer/src:literals",
        "//mcucore/src:array_view",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:status_or",
    ],
)

cc_library(
    name = "switch_adapter",
    srcs = ["switch_adapter.cc"],
    hdrs = ["switch_adapter.h"],
    deps = [
        "//TinyAlpacaServer/src:alpaca_response",
        "//TinyAlpacaServer/src:ascom_error_codes",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:literals",
        "//TinyAlpacaServer/src/device_types:device_impl_base",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:status_or",
    ],
)

cc_library(
    name = "switch_interface",
    hdrs = ["switch_interface.h"],
    deps = [
        "//TinyAlpacaServer/src:alpaca_request",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:status",
        "//mcucore/src:status_or",
    ],
)

cc_library(
    name = "toggle_switch_base",
    srcs = ["toggle_switch_base.cc"],
    hdrs = ["toggle_switch_base.h"],
    deps = [
        ":switch_interface",
        "//mcucore/src:mcucore_platform",
    ],
)
