# Implementation of the AstroMakers Cover Calibrator device driver, i.e. an
# Arduino sketch which offers an ASCOM Alpaca server with a CoverCalibrator
# device.

load(
    "//TinyAlpacaServer/extras/dev_tools:build_config.bzl",
    "build_config",
)

package(
    default_copts = build_config.default_copts,
)

cc_library(
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

cc_library(
    name = "constants",
    hdrs = ["constants.h"],
    deps = ["//mcucore/extras/host/arduino"],
)

cc_library(
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [
        ":am_weather_box",
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/src:McuCore",
        "//mcucore/src:eeprom_tlv",
        "//mcucore/src:jitter_random",
        "//mcunet/src:mcu_net",
    ],
)
