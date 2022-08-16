# Defines a base type for Observing Conditions devices.

load(
    "//TinyAlpacaServer/extras/dev_tools:build_config.bzl",
    "build_config",
)

package(
    default_copts = build_config.default_copts,
)

cc_library(
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
