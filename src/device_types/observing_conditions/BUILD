# Defines a base type for Observing Conditions devices.

load(
    "//mcucore/extras/bazel:arduino_cc_library.bzl",
    "arduino_cc_library",
)

arduino_cc_library(
    name = "observing_conditions_adapter",
    srcs = ["observing_conditions_adapter.cc"],
    hdrs = ["observing_conditions_adapter.h"],
    deps = [
        "//TinyAlpacaServer/src:alpaca_response",
        "//TinyAlpacaServer/src:ascom_error_codes",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:literals",
        "//TinyAlpacaServer/src/device_types:device_impl_base",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/log",
        "//mcucore/src/print:o_print_stream",
        "//mcucore/src/print:printable_cat",
        "//mcucore/src/status",
        "//mcucore/src/status:status_or",
        "//mcucore/src/strings:progmem_string_data",
        "//mcucore/src/strings:progmem_string_view",
    ],
)
