# Ideas for the future of Tiny Alpaca Server.

cc_library(
    name = "progmem_pointer",
    hdrs = ["progmem_pointer.h"],
    deps = [
        ":platform",
    ],
)

cc_library(
    name = "pwm_led",
    srcs = ["pwm_led.cc"],
    hdrs = ["pwm_led.h"],
    deps = [
        ":logging",
        ":platform",
        ":pwm_led",
    ],
)

cc_library(
    name = "time",
    srcs = ["time.cc"],
    hdrs = ["time.h"],
    deps = [":platform"],
)
