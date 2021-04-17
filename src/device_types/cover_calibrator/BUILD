# Defines a base type for Cover Calibrator devices.

cc_library(
    name = "cover_calibrator_adapter",
    srcs = ["cover_calibrator_adapter.cc"],
    hdrs = ["cover_calibrator_adapter.h"],
    deps = [
        ":alpaca_response",
        ":ascom_error_codes",
        ":constants",
        ":cover_calibrator_constants",
        ":device_impl_base",
        ":literals",
        ":platform",
        ":status_or",
    ],
)

cc_library(
    name = "cover_calibrator_constants",
    srcs = ["cover_calibrator_constants.cc"],
    hdrs = ["cover_calibrator_constants.h"],
    deps = [
        ":inline_literal",
        ":platform",
        ":print_misc",
        ":printable_progmem_string",
    ],
)
