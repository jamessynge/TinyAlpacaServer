# Implementation of the AstroMakers Cover Calibrator device driver, i.e. an
# Arduino sketch which offers an ASCOM Alpaca server with a CoverCalibrator
# device.

cc_library(
    name = "cover",
    srcs = ["cover.cc"],
    hdrs = ["cover.h"],
    deps = [
        ":TinyAlpacaServer",
        ":avr_io",
        ":avr_timer_counter",
        ":constants",
        ":cover_calibrator_constants",
    ],
)

cc_library(
    name = "cover_calibrator",
    srcs = ["cover_calibrator.cc"],
    hdrs = ["cover_calibrator.h"],
    deps = [
        ":TinyAlpacaServer",
        ":constants",
        ":cover",
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

cc_library(
    name = "constants",
    hdrs = ["constants.h"],
)
