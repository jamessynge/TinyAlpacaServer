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
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/container:flash_string_table",
        "//mcucore/src/log",
        "//mcucore/src/print:counting_print",
        "//mcucore/src/print:has_print_to",
        "//mcucore/src/print:print_misc",
        "//mcucore/src/print:print_to_buffer",
        "//mcucore/src/print:stream_to_print",
        "//mcucore/src/semistd:type_traits",
        "//mcucore/src/strings:progmem_string_data",
    ],
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
