# With Arduino Builder, can be used to measure the amount of memory
# required when various objects/code is referenced by the sketch.

load(
    "//mcucore/extras/bazel:arduino_cc_sketch.bzl",
    "arduino_cc_sketch",
)

arduino_cc_sketch(
    name = "encoder_memory_measurements",
    srcs = ["EncoderMemoryMeasurements.ino.cc"],
    deps = [
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/host/arduino:arduino_main",
    ],
)
