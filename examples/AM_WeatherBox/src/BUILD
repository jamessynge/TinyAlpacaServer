# Implementation of the AstroMakers Cover Calibrator device driver, i.e. an
# Arduino sketch which offers an ASCOM Alpaca server with a CoverCalibrator
# device.

cc_library(
    name = "am_weather_box",
    srcs = ["am_weather_box.cc"],
    hdrs = ["am_weather_box.h"],
    deps = [
        ":constants",
        "//experimental/users/jamessynge/arduino/hostuino/extras/host/arduino",
        "//experimental/users/jamessynge/arduino/hostuino/extras/host/mlx90614",
        "//src:TinyAlpacaServer",
    ],
)

cc_library(
    name = "constants",
    hdrs = ["constants.h"],
    deps = ["//experimental/users/jamessynge/arduino/hostuino/extras/host/arduino"],
)

cc_library(
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [
        ":am_weather_box",
        "//experimental/users/jamessynge/arduino/hostuino/extras/host/arduino",
        "//src:TinyAlpacaServer",
    ],
)
