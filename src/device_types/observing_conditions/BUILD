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
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
        "//mcucore/src:printable_cat",
        "//mcucore/src:progmem_string_data",
        "//mcucore/src:progmem_string_view",
        "//mcucore/src:status",
        "//mcucore/src:status_or",
    ],
)
