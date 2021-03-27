# Demo of how to use Tiny Alpaca Server.

cc_binary(
    name = "TinyAlpacaServerDemo",
    srcs = ["TinyAlpacaServerDemo.ino.cc"],
    deps = [
        ":TinyAlpacaServer",
        ":arduino_main",
        ":dht22_handler",
    ],
)

cc_library(
    name = "dht22_handler",
    srcs = ["dht22_handler.cc"],
    hdrs = ["dht22_handler.h"],
    deps = [
        ":TinyAlpacaServer",
        ":pretend_devices",
    ],
)

cc_library(
    name = "pretend_devices",
    hdrs = ["pretend_devices.h"],
)
