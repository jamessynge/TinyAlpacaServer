# TODO(jamessynge): describe this package.

cc_library(
    name = "cover_calibrator",
    srcs = ["cover_calibrator.cc"],
    hdrs = ["cover_calibrator.h"],
    deps = [
        ":TinyAlpacaServer",
        ":alpaca_request",
        ":cover",
        ":cover_calibrator_adapter",
        ":cover_calibrator_constants",
        ":pwm_led",
    ],
)

cc_library(
    name = "pwm_led",
    srcs = ["pwm_led.cc"],
    hdrs = ["pwm_led.h"],
    deps = [
        ":TinyAlpacaServer",
        ":cover_calibrator_constants",
    ],
)

# cc_library(
#     name = "server",
#     srcs = ["server.cc"],
#     hdrs = ["server.h"],
#     deps = [
#         ":TinyAlpacaServer",
#     ],
# )
