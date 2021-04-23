# Implementation of the AstroMakers Cover Calibrator device driver, i.e. an
# Arduino sketch which offers an ASCOM Alpaca server with a CoverCalibrator
# device.

cc_library(
    name = "cover",
    srcs = ["cover.cc"],
    hdrs = ["cover.h"],
    deps = [
        ":TinyAlpacaServer",
        ":cover_calibrator_constants",
    ],
)

cc_library(
    name = "cover_calibrator",
    srcs = ["cover_calibrator.cc"],
    hdrs = ["cover_calibrator.h"],
    deps = [
        ":TinyAlpacaServer",
        ":alpaca_request",
        ":ascom_error_codes",
        ":cover",
        ":cover_calibrator_adapter",
        ":cover_calibrator_constants",
        ":ramp_pwm_led",
        ":status",
    ],
)

cc_library(
    name = "pwm_led",
    srcs = ["pwm_led.cc"],
    hdrs = ["pwm_led.h"],
    deps = [
        ":TinyAlpacaServer",
        ":platform",
        ":pwm_led",
    ],
)

cc_library(
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [
        ":TinyAlpacaServer",
        ":cover_calibrator",
    ],
)
