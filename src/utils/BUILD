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
    name = "avr_timer_counter",
    srcs = ["avr_timer_counter.cc"],
    hdrs = ["avr_timer_counter.h"],
    deps = [
        "//mcucore/src:counting_print",
        "//mcucore/src:flash_string_table",
        "//mcucore/src:has_print_to",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:print_misc",
        "//mcucore/src:progmem_string_data",
        "//mcucore/src:stream_to_print",
        "//mcucore/src/semistd:type_traits",
    ],
)

cc_library(
    name = "moving_average",
    srcs = ["moving_average.cc"],
    hdrs = ["moving_average.h"],
    deps = [
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:progmem_string_data",
    ],
)
