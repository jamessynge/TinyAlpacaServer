# Defines a base type for Observing Conditions devices.

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
        "//TinyAlpacaServer/src/device_types/cover_calibrator:cover_calibrator_constants",
        "//TinyAlpacaServer/src/utils:printable_cat",
        "//TinyAlpacaServer/src/utils:status",
        "//TinyAlpacaServer/src/utils:status_or",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)
