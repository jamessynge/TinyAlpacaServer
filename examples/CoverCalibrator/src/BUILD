# Implementation of the AstroMakers Cover Calibrator device driver, i.e. an
# Arduino sketch which offers an ASCOM Alpaca server with a CoverCalibrator
# device.

cc_library(
    name = "constants",
    hdrs = ["constants.h"],
)

cc_library(
    name = "cover",
    srcs = ["cover.cc"],
    hdrs = ["cover.h"],
    deps = [
        ":constants",
        "//extras/host/arduino",
        "//src:TinyAlpacaServer",
    ],
)

cc_library(
    name = "cover_calibrator",
    srcs = ["cover_calibrator.cc"],
    hdrs = ["cover_calibrator.h"],
    deps = [
        ":constants",
        ":cover",
        "//extras/host/arduino",
        "//src:TinyAlpacaServer",
    ],
)

cc_library(
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [
        ":cover_calibrator",
        "//extras/host/arduino",
        "//src:TinyAlpacaServer",
    ],
)
