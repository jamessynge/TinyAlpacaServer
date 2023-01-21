# Implementation of the AstroMakers Cover Calibrator device driver, i.e. an
# Arduino sketch which offers an ASCOM Alpaca server with a CoverCalibrator
# device.

load(
    "//mcucore/extras/bazel:arduino_cc_library.bzl",
    "arduino_cc_library",
)

arduino_cc_library(
    name = "am_weather_box",
    srcs = ["am_weather_box.cc"],
    hdrs = ["am_weather_box.h"],
    deps = [
        ":constants",
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/host/mlx90614",
    ],
)

arduino_cc_library(
    name = "constants",
    hdrs = ["constants.h"],
    deps = ["//mcucore/extras/host/arduino"],
)

arduino_cc_library(
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [
        ":am_weather_box",
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/src:McuCore",
        "//mcunet/src:mcu_net",
    ],
)
