# Defines a base class for Cover Calibrator devices.

load(
    "//TinyAlpacaServer/extras/dev_tools:build_config.bzl",
    "build_config",
)

package(
    default_copts = build_config.default_copts,
)

cc_library(
    name = "cover_calibrator_adapter",
    srcs = ["cover_calibrator_adapter.cc"],
    hdrs = ["cover_calibrator_adapter.h"],
    deps = [
        ":cover_calibrator_constants",
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
    name = "cover_calibrator_constants",
    srcs = ["cover_calibrator_constants.cc"],
    hdrs = ["cover_calibrator_constants.h"],
    deps = [
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:print_misc",
        "//mcucore/src:progmem_string_data",
    ],
)
