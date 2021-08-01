# Demo of how to use Tiny Alpaca Server.

cc_binary(
    name = "TinyAlpacaServerDemo",
    srcs = ["TinyAlpacaServerDemo.ino.cc"],
    deps = [
        ":dht22_handler",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/host/arduino:arduino_main",
        "//src:TinyAlpacaServer",
    ],
)

cc_library(
    name = "dht22_handler",
    srcs = ["dht22_handler.cc"],
    hdrs = ["dht22_handler.h"],
    deps = [
        ":pretend_devices",
        "//src:TinyAlpacaServer",
    ],
)

cc_library(
    name = "pretend_devices",
    hdrs = ["pretend_devices.h"],
)
