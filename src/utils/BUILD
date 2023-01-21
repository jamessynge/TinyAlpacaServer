# Provides utility methods and classes, such as PrintCharJsonEscaped and mcucore::StatusOr
# which aren't necessarily specific to Alpaca, but could be useful in any Arduino
# sketch.

load(
    "//mcucore/extras/bazel:arduino_cc_library.bzl",
    "arduino_cc_library",
)

arduino_cc_library(
    name = "moving_average",
    srcs = ["moving_average.cc"],
    hdrs = ["moving_average.h"],
    deps = [
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/log",
        "//mcucore/src/strings:progmem_string_data",
    ],
)
