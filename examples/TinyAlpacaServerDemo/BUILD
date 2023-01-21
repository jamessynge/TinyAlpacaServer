# Demo of how to use Tiny Alpaca Server.

load(
    "//mcucore/extras/bazel:arduino_cc_sketch.bzl",
    "arduino_cc_sketch",
)

arduino_cc_sketch(
    name = "TinyAlpacaServerDemo",
    srcs = ["TinyAlpacaServerDemo.ino.cc"],
    deps = [
        "//TinyAlpacaServer/examples/TinyAlpacaServerDemo/src:server",
        "//mcucore/extras/host/arduino",
        "//mcunet/extras/host/ethernet5500:host_network_main",
    ],
)
