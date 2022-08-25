# Tests of Tiny Alpaca Server src/utils/...

cc_test(
    name = "avr_timer_counter_test",
    srcs = ["avr_timer_counter_test.cc"],
    deps = [
        "//TinyAlpacaServer/src/utils:avr_timer_counter",
        "//absl/log",
        "//absl/time",
        "//googletest:gunit_main",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/extras/test_tools:print_value_to_std_string",
    ],
)

cc_test(
    name = "moving_average_test",
    srcs = ["moving_average_test.cc"],
    deps = [
        "//TinyAlpacaServer/src/utils:moving_average",
        "//absl/log",
        "//absl/random",
        "//absl/random:distributions",
        "//absl/time",
        "//googletest:gunit_main",
    ],
)
