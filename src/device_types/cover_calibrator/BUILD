# Defines a base class for Cover Calibrator devices.

load(
    "//mcucore/extras/bazel:arduino_cc_library.bzl",
    "arduino_cc_library",
)

arduino_cc_library(
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
        "//mcucore/src/status:status_or",
    ],
)

arduino_cc_library(
    name = "cover_calibrator_constants",
    srcs = ["cover_calibrator_constants.cc"],
    hdrs = ["cover_calibrator_constants.h"],
    deps = [
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/container:flash_string_table",
        "//mcucore/src/print:print_misc",
        "//mcucore/src/print:print_to_buffer",
        "//mcucore/src/strings:progmem_string_data",
    ],
)
