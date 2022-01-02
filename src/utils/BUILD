# Provides utility methods and classes, such as PrintCharJsonEscaped and mcucore::StatusOr
# which aren't necessarily specific to Alpaca, but could be useful in any Arduino
# sketch.

cc_library(
    name = "avr_timer_counter",
    srcs = ["avr_timer_counter.cc"],
    hdrs = ["avr_timer_counter.h"],
    deps = [
        "//mcucore/src:counting_print",
        "//mcucore/src:has_print_to",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:print_misc",
        "//mcucore/src:progmem_string_data",
        "//mcucore/src:stream_to_print",
        "//mcucore/src:type_traits",
    ],
)

cc_library(
    name = "moving_average",
    srcs = ["moving_average.cc"],
    hdrs = ["moving_average.h"],
    deps = [
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:progmem_string_data",
    ],
)
