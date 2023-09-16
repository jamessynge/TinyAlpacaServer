# Implementation of the AstroMakers Cover Calibrator device driver, i.e. an
# Arduino sketch which offers an ASCOM Alpaca server with a CoverCalibrator
# device.

load(
    "//mcucore/extras/bazel:arduino_cc_library.bzl",
    "arduino_cc_library",
)

arduino_cc_library(
    name = "constants",
    hdrs = ["constants.h"],
)

arduino_cc_library(
    name = "cover",
    srcs = ["cover.cc"],
    hdrs = ["cover.h"],
    deps = [
        ":constants",
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/src:McuCore",
    ],
)

arduino_cc_library(
    name = "cover_calibrator",
    srcs = ["cover_calibrator.cc"],
    hdrs = ["cover_calibrator.h"],
    deps = [
        ":constants",
        ":cover",
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/src:McuCore",
    ],
)

cc_library(
    name = "led_channel_enable_switch",
    srcs = ["led_channel_enable_switch.cc"],
    hdrs = ["led_channel_enable_switch.h"],
    deps = [
        ":cover_calibrator",
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//TinyAlpacaServer/src/device_types/switch:toggle_switch_base",
        "//mcucore/src:mcucore_platform",
    ],
)

arduino_cc_library(
    name = "led_channel_switch_group",
    srcs = ["led_channel_switch_group.cc"],
    hdrs = ["led_channel_switch_group.h"],
    deps = [
        ":cover_calibrator",
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/src:McuCore",
    ],
)

arduino_cc_library(
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [
        ":cover_calibrator",
        ":led_channel_switch_group",
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/src:McuCore",
        "//mcunet/src:mcu_net",
    ],
)
