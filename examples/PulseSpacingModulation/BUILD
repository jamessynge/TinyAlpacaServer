load(
    "//mcucore/extras/bazel:arduino_cc_sketch.bzl",
    "arduino_cc_sketch",
)

arduino_cc_sketch(
    name = "pulse_spacing_modulation",
    srcs = ["PulseSpacingModulation.ino.cc"],
    deps = [
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/host/arduino:arduino_main",
    ],
)
