# Provides utility methods and classes, such as PrintCharJsonEscaped and mcucore::StatusOr
# which aren't necessarily specific to Alpaca, but could be useful in any Arduino
# sketch.

load(
    "//TinyAlpacaServer/extras/dev_tools:build_config.bzl",
    "build_config",
)

package(
    default_copts = build_config.default_copts,
)

cc_library(
    name = "moving_average",
    srcs = ["moving_average.cc"],
    hdrs = ["moving_average.h"],
    deps = [
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/log",
        "//mcucore/src/strings:progmem_string_data",
    ],
)
