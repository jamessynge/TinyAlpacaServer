# Demo of how to use Tiny Alpaca Server.

cc_binary(
    name = "TinyAlpacaServerDemo",
    srcs = ["TinyAlpacaServerDemo.ino.cc"],
    deps = [
        ":TinyAlpacaServer",
        ":arduino_main",
        ":pretend_devices",
    ],
)

cc_library(
    name = "pretend_devices",
    hdrs = ["pretend_devices.h"],
)
