# Defines a base type for Cover Calibrator devices.

cc_library(
    name = "cover_calibrator_adapter",
    srcs = ["cover_calibrator_adapter.cc"],
    hdrs = ["cover_calibrator_adapter.h"],
    deps = [
        ":cover_calibrator_constants",
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
    name = "cover_calibrator_constants",
    srcs = ["cover_calibrator_constants.cc"],
    hdrs = ["cover_calibrator_constants.h"],
    deps = [
        "//src/utils:inline_literal",
        "//src/utils:platform",
        "//src/utils:print_misc",
    ],
)
