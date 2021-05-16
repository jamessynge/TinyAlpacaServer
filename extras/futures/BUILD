# Ideas for the future of Tiny Alpaca Server.

cc_library(
    name = "progmem_pointer",
    hdrs = ["progmem_pointer.h"],
    deps = [
        "//src/utils:platform",
    ],
)

cc_library(
    name = "pwm_led",
    srcs = ["pwm_led.cc"],
    hdrs = ["pwm_led.h"],
    deps = ["//extras/host/arduino"],
)

cc_library(
    name = "ramp_pwm_led",
    srcs = ["ramp_pwm_led.cc"],
    hdrs = ["ramp_pwm_led.h"],
    deps = [
        ":pwm_led",
        "//extras/host/arduino",
        "//src/utils:logging",
        "//src/utils:platform",
    ],
)

cc_library(
    name = "time",
    srcs = ["time.cc"],
    hdrs = ["time.h"],
    deps = ["//src/utils:platform"],
)
