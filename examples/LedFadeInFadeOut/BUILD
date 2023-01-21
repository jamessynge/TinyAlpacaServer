# SweepEverything serves as a test of the timer/counter code and the pin mapping.

load(
    "//mcucore/extras/bazel:arduino_cc_sketch.bzl",
    "arduino_cc_sketch",
)

arduino_cc_sketch(
    name = "led_fade_in_fade_out",
    srcs = ["LedFadeInFadeOut.ino.cc"],
    deps = [
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/host/arduino:arduino_main",
        "//mcucore/src:McuCore",
    ],
)
