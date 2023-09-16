# Defines base classes for implementing ASCOM Alpaca Switch devices.

load(
    "//mcucore/extras/bazel:arduino_cc_library.bzl",
    "arduino_cc_library",
)

arduino_cc_library(
    name = "multi_switch_adapter",
    srcs = ["multi_switch_adapter.cc"],
    hdrs = ["multi_switch_adapter.h"],
    deps = [
        ":switch_adapter",
        ":switch_interface",
        "//TinyAlpacaServer/src:device_description",
        "//TinyAlpacaServer/src:literals",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/container:array_view",
        "//mcucore/src/status:status_or",
    ],
)

arduino_cc_library(
    name = "switch_adapter",
    srcs = ["switch_adapter.cc"],
    hdrs = ["switch_adapter.h"],
    deps = [
        "//TinyAlpacaServer/src:alpaca_response",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:eeprom_ids",
        "//TinyAlpacaServer/src:literals",
        "//TinyAlpacaServer/src/device_types:device_impl_base",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/log",
        "//mcucore/src/print:any_printable",
        "//mcucore/src/print:print_to_buffer",
        "//mcucore/src/status:status_or",
        "//mcucore/src/strings:string_view",
    ],
)

arduino_cc_library(
    name = "switch_interface",
    hdrs = ["switch_interface.h"],
    deps = [
        "//TinyAlpacaServer/src:alpaca_request",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/status",
        "//mcucore/src/status:status_or",
    ],
)

arduino_cc_library(
    name = "toggle_switch_base",
    srcs = ["toggle_switch_base.cc"],
    hdrs = ["toggle_switch_base.h"],
    deps = [
        ":switch_interface",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/status",
        "//mcucore/src/status:status_or",
    ],
)
