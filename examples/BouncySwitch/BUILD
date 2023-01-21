# Experiment with edge triggered pin interrupts.

load(
    "//mcucore/extras/bazel:arduino_cc_sketch.bzl",
    "arduino_cc_sketch",
)

arduino_cc_sketch(
    name = "bouncy_switch",
    srcs = ["BouncySwitch.ino.cc"],
    deps = [
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/host/arduino:arduino_main",
    ],
)
