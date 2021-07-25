# Implementation of the AstroMakers Cover Calibrator device driver, i.e. an
# Arduino sketch which offers an ASCOM Alpaca server with a CoverCalibrator
# device.

cc_library(
    name = "am_weather_box",
    srcs = ["am_weather_box.cc"],
    hdrs = ["am_weather_box.h"],
    deps = [
        ":constants",
        "//extras/host/arduino",
        "//extras/host/mlx90614",
        "//src:TinyAlpacaServer",
        "//src:ascom_error_codes",
    ],
)

cc_library(
    name = "constants",
    hdrs = ["constants.h"],
    deps = ["//extras/host/arduino"],
)

cc_library(
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [
        ":am_weather_box",
        "//extras/host/arduino",
        "//src:TinyAlpacaServer",
    ],
)
